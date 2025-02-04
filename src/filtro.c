
/* Isso é parte do código 'main.c', não um módulo em sí. Está aqui para 
 * organizar mais o código. */

// Biblioteca padrão Unix:
#include <dirent.h>
#include <unistd.h>
// Biblioteca padrão do C:
#include <string.h>
#include <time.h>
// Biblioteca externa:
#include "legivel.h"
#include "listaarray_ref.h"

char* forma_caminho(char* base, char* entrada) {
   size_t n = strlen(entrada) + strlen(base) + 3;
   size_t t = sizeof (char);
   char* novo_caminho = malloc(n * t);

   strcpy (novo_caminho, base);
   strcat (novo_caminho, "/");
   strcat (novo_caminho, entrada);

   return novo_caminho;
}

void auxiliar_do_filtro(vetor_t* coletador, DIR* diretorio, char* caminho) 
{
   struct dirent* entrada = readdir(diretorio);
   const int STR_IGUAIS = 0;

   while (entrada != NULL) {
      char tipo_de_entrada = entrada->d_type;
      char* nome = entrada->d_name;
      char* novo_caminho = forma_caminho(caminho, nome);
      bool e_um_diretorio = tipo_de_entrada == DT_DIR;
      bool e_um_arquivo = tipo_de_entrada == DT_REG;
      bool nenhum_dos_diretorios = {
         !(strcmp(nome, "..") == STR_IGUAIS ||
            strcmp(nome, ".") == STR_IGUAIS)
      };

      if (e_um_arquivo) {
         #if defined(_FILTRA_TODOS_ARQUIVOS)
         printf ("arquivo: '%s'\n", nome);
         #endif
         insere_al(coletador, (char*)novo_caminho);
      } else if (e_um_diretorio && nenhum_dos_diretorios) {
         #if defined(_FILTRA_TODOS_ARQUIVOS)
         printf ("diretório: '%s'\n", nome);
         #endif
         DIR* subdir = opendir(novo_caminho);
         // coletando arquivos também deste subdiretório...
         auxiliar_do_filtro (coletador, subdir, novo_caminho);
         closedir(subdir);
      } 
      #if defined(_FILTRA_TODOS_ARQUIVOS)
      printf ("caminho: '%s'\n\n", novo_caminho);
      #endif
      
      // iterando ainda o diretório.
      entrada = readdir(diretorio);
   }
}

vetor_t* filtra_todos_arquivos(char* caminho) {
   /* Tal raiz tem que ser um diretório com muitos arquivos dentro dele,
    * ou subdiretórios com arquivos, esta função varrerá tudo, e coletará
    * também tudo dentro de tal "container". 
	 */
   vetor_t* coletador = cria_al();
   DIR* raiz = opendir(caminho);

   auxiliar_do_filtro(coletador, raiz, caminho);
   closedir(raiz);

   #if defined(_FILTRA_TODOS_ARQUIVOS)
   printf ("total de arquivos pegos: %lu\n", tamanho_al(coletador));
   #endif

   return coletador;
}

static time_t inicio;
static bool iniciado = false;

void info_da_navegacao(vetor_t* lista, size_t* depth) {
   // só muda isso apenas uma vez.
   if (!iniciado) { iniciado = true; }

   time_t final = time (NULL);

   if (difftime (final, inicio) > 1.0) {
      // resetando contagem...
      inicio = time (NULL);
      // quantidade de arquivos mais legível:
      size_t qtd_of_files = tamanho_al (lista);
      char* valorstr = valor_legivel (qtd_of_files);

      printf (
         // "foram colhidos %lu arquivos e uma profundidade de %lu.\n", 
         // tamanho_al (lista), *depth
         "Foram colhidos %8s arquivos e uma profundidade de %lu.\n", 
         valorstr, *depth
      );
      free (valorstr);
   }
}

void auxiliar_do_filtro_visualiza(vetor_t* coletador, DIR* diretorio, 
  char* caminho, size_t* profundidade) 
{
   /* O mesmo que a função acima, pórem mostra progresso do processo de 
    * navegação no sistema de arquivos. */
   struct dirent* entrada = readdir(diretorio);
   const int STR_IGUAIS = 0;

   while (entrada != NULL) {
      char tipo_de_entrada = entrada->d_type;
      char* nome = entrada->d_name;
      char* novo_caminho = forma_caminho(caminho, nome);
      bool e_um_diretorio = tipo_de_entrada == DT_DIR;
      bool e_um_arquivo = tipo_de_entrada == DT_REG;
      bool nenhum_dos_diretorios = {
         !(strcmp(nome, "..") == STR_IGUAIS ||
            strcmp(nome, ".") == STR_IGUAIS)
      };

      if (e_um_arquivo) {
         insere_al(coletador, (char*)novo_caminho);
         info_da_navegacao (coletador, profundidade);
      } else if (e_um_diretorio && nenhum_dos_diretorios) {
         DIR* subdir = opendir(novo_caminho);
         // incrementa o nível de aninhamento de subdiretórios nesta
         // recursão preste a acontencer.
         *profundidade += 1;
         // coletando arquivos também deste subdiretório...
         auxiliar_do_filtro_visualiza (
            coletador, subdir, 
            novo_caminho, 
            profundidade
         );
         closedir(subdir);
         // quando tiver saído dela, apenas decrementa a contagem(o nível).
         *profundidade -= 1;
      } 
      info_da_navegacao (coletador, profundidade);
      
      // iterando ainda o diretório.
      entrada = readdir(diretorio);
   }
}

vetor_t* filtra_todos_arquivos_visualiza(char* caminho) {
   /* Tal raiz tem que ser um diretório com muitos arquivos dentro dele,
    * ou subdiretórios com arquivos, esta função varrerá tudo, e coletará
    * também tudo dentro de tal "container". 
	 */
   vetor_t* coletador = cria_al();
   DIR* raiz = opendir(caminho);
   size_t profundidade = 0;

   auxiliar_do_filtro_visualiza(coletador, raiz, caminho, &profundidade);
   closedir(raiz);

   return coletador;
}
