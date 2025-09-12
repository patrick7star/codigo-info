// Declaração das funções abaixos:
#include "filtro.h"
// Módulos deste projeto:
#include "classificacao.h"
// Biblioteca padrão do C:
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
// Biblioteca padrão Unix:
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
// Biblioteca externa:
#include "legivel.h"
#include "memoria.h"
#include "tempo.h"

// Nemômicos dos resultados das operações:
#define STAT_FALHOU -1
#define STAT_SUCESSO 0
// Variáveis globais do escopo global.
static time_t inicio;
static bool iniciado = false;
// enum Results { Okay = 0, Failed = -1 };

typedef struct arquivo_info { 
   // linhas não-vázias e vázias:
   uint32_t linhas; 
   uint32_t linhas_vazias; 

   // maior coluna de caractéres do arquivo:
   uint16_t maxima_coluna;
} ArquivoInfo;



struct arquivo_info contabiliza_linhas(FILE* arquivo) {
   struct arquivo_info coletado = {0, 0, 0};
   char lido = '\0';
   uint16_t coluna = 0;

   /* lendo cada caractére do arquivo passado, então contabiliza as
    * quebra-de-linha, linhas vázias e 'coluna máxima' do arquivo. */
   do {
      lido = fgetc(arquivo);

      /* sempre que achar uma coluna máxima, atualizar a variável que
       * registra tal informação.
       */
      if (coletado.maxima_coluna < coluna) 
         { coletado.maxima_coluna = coluna; }

      // se achar uma quebra de linha dois casos serão tratados...
      if (lido == '\n') {
         /* primeiro este, o da linha vázia. Como a coluna neste ponto
          * estará nula, esta é a única conclusão a se tirar. Conta ela, e
          * pula a contagem de linhas normais abaixo. */
         if (coluna == 0) {
            coletado.linhas_vazias++;
            continue;
         }

         /* segundo caso é o da linha normal. Apenas contabiliza se chegar 
          * até tal parte, porém também fica o cargo de zerá a variável que
          * registra a coluna, já que uma nova linha começa. No fim, também
          * reseta o ciclo, assim evita de a coluna, recem resetada, conte
          * esta parte. 
          */
         coletado.linhas++;
         coluna = 0;
         continue;
      }
      // marca a atual coluna no arquivo.
      coluna++;
   } while (lido != EOF);

   return coletado;
}

#ifdef __dead_code__
static void visualiza_info_do_arquivo(char* caminho) {
   /* visualização do arquivo passado como argumento, todas informações 
    * codificadas para serem mostradas. */
   FILE* arq = fopen(caminho, "rt");
   struct arquivo_info dado = contabiliza_linhas(arq);
   uint32_t ql = dado.linhas + dado.linhas_vazias;
   TipoDeArquivo extensao = tipo_do_arquivo(caminho);

   printf ("total de linhas: %14u[%u brancas]\n", ql, dado.linhas_vazias);
   printf ("maior coluna: %17u\n", dado.maxima_coluna);
   printf ("tipo de arquivo: %21s\n", ta_to_str(extensao));
   tipo_do_arquivo(caminho);
   fclose(arq);
}

static void mostra_nome_do_arquivo(char* caminho) {
   /* formata melhor o nome do arquivo dado o caminho.
    * Nota: não cuida da validade do caminho. 
    */
   char* nome_do_arquivo = strrchr(caminho, '/');
   char recuo[30];

   // preenchendo com espaçadores ...
   memset(recuo, ' ', 19);

   // pulando o 'slash'...
   printf ("\narquivo: %s|%s|\n", recuo, ++nome_do_arquivo);
}
#endif

static char* forma_caminho(char* base, char* entrada) {
   size_t n = strlen(entrada) + strlen(base) + 3;
   size_t t = sizeof (char);
   char* novo_caminho = malloc(n * t);

   strcpy (novo_caminho, base);
   strcat (novo_caminho, "/");
   strcat (novo_caminho, entrada);

   return novo_caminho;
}

static void auxiliar_do_filtro(vetor_t* coletador, DIR* diretorio,
  char* caminho) 
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

extern vetor_t* filtra_todos_arquivos(char* caminho) {
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

static void info_da_navegacao(vetor_t* lista, size_t* depth) {
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

static void auxiliar_do_filtro_visualiza
  (vetor_t* coletador, DIR* diretorio, char* caminho, size_t* profundidade) 
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

extern vetor_t* filtra_todos_arquivos_visualiza(char* caminho) {
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

char* hashtable_formatacao(HashTable m) {
   IteradorHT iter = cria_iter_ht(m);
   const int sz = sizeof(char), QTD = 4000;
   char* fmt = malloc(QTD * sz), buffer[100];

   // Coloca o primeiro demilitador antes da iteração.
   memset(buffer, '\0', 100);
   if (tamanho_ht(m) >= 5)
      strcpy(fmt, "{\n");
   else
      strcpy(fmt, "{");

   while (!consumido_iter_ht(iter))
   {
      IterOutputHT a = next_ht(iter);
      char* chave = (char*)a.key;
      int valor = *((int*)a.value);

      if (tamanho_ht(m) >= 5)
         sprintf(buffer, "\t\b\b%s: %d,\n", chave, valor);
      else
         sprintf(buffer, "%s: %d, ", chave, valor);
      strcat(fmt, buffer);
   }
   // Coloca o último delimitador, depois de todas concatenações. E pronto,
   // está finalizado a formatação.
   if (tamanho_ht(m) >= 5)
      strcat(fmt, "  }");
   else
      strcat(fmt, "\b\b}");
   return fmt;
}

void visualiza_diretorio_info (DiretorioInfo* i) {
   HashTable dicio = (*i).todos_tipos;
   char* frequencias = hashtable_formatacao (dicio);
   char caminho_normal[PATH_MAX];

   // Tenta resolve atalhos como "." ou "..".
   if (realpath(i->caminho_do_projeto, caminho_normal) == NULL) {
      perror(strerror(errno));
      abort();
   }

   #ifdef __debug__
   printf("PATH_MAX: %u bytes\n", PATH_MAX);
   #endif

   printf (
      "\n  Caminho: '%s'\n"
      "  Qtd. de arquivos: %s\n"
      "  Média de linhas por arquivo: %0.1f\n"
      "  Total de linhas: %s\n"
      "  Tipos de arquivos: %s\n\n",
      caminho_normal,
      valor_legivel (i->total_de_arquivos),
      i->media_de_linhas,
      valor_legivel (i->total_de_linhas),
      frequencias
   );
}

static double decorrido_timeval(struct timeval f, struct timeval i) { 
// Marca o tempo decorrido entre dois 'timeval', e computa os em segundos.
   double segundos = (double)(f.tv_sec - i.tv_sec); 
   double microsegs = (double)(f.tv_usec - i.tv_usec); 

   return segundos + microsegs / 1.0e6;
}

static size_t key_hash(Generico key, size_t p) 
{
// O mesmo hash que uso na maioria das narrow strings.
   char* chave = (char*)key;
   size_t len = strlen(chave);
   size_t code = (int)chave[len / 2];

   return (len * code) % p;
}

static bool key_eq(Generico a, Generico b) 
// Apenas embrulhando a comparação de strings.
   { return strcmp((char*)a, (char*)b) == 0; }

struct diretorio_info processa_projeto(char* caminho) {
   DiretorioInfo coletado = {
      .total_de_arquivos  = 0LLU, 
      .media_de_linhas    = 0.0f, 
      .todos_tipos        = cria_ht(key_hash, key_eq)
   };
   struct stat atributos;
   vetor_t* lista_caminhos;
   ArquivoInfo info_arquivo;
   size_t n_arquivos;

   if (stat (caminho, &atributos) == STAT_FALHOU) { 
      printf ("caminho: '%s'\n", caminho);
      perror ("não existe tal caminho."); abort(); 
   } else if (!S_ISDIR(atributos.st_mode))
      { perror ("não é para não diretórios."); abort(); }

   /* Aqui começa o processamento ...*/
   lista_caminhos = filtra_todos_arquivos_visualiza(caminho);
   coletado.total_de_arquivos = tamanho_al(lista_caminhos);
   coletado.caminho_do_projeto = caminho;
   n_arquivos = coletado.total_de_arquivos;

   puts ("Começou o processo de varredura dos arquivos ...");
   size_t processados = 0, rejeitados = 0;
   size_t inicialmente = tamanho_al (lista_caminhos);
   // Definindo uma pausa de 300 milisegundos.
   struct timeval inicio, final;

   gettimeofday(&inicio, NULL);
   /* retirando cada caminho extraído, e processando seu conteúdo interno.*/
   while (!vazia_al(lista_caminhos)) {
      char* arquivo = remove_al(lista_caminhos);
      FILE* stream = fopen (arquivo, "rt");
      size_t n_bytes = sizeof (TipoDeArquivo);
      TipoDeArquivo* tipo = malloc (n_bytes);
      size_t n_linhas;

      // inserindo tipo de arquivo, contabilizando na verdade.
      *tipo = tipo_do_arquivo(arquivo);
      char* chave = (char*)ta_to_str (*tipo);
      hashtable_t* mapa = coletado.todos_tipos;

      if (contem_ht (mapa, chave)) {
         generico_t valor = obtem_ht (mapa, chave);
         int* atual = (int*)valor;

         *atual = *atual + 1;
      } else {
         int* valor = box_int(1);
         insere_ht (mapa, chave, valor);
      }

      if (pode_ser_lido (tipo_do_arquivo (arquivo)))
         /* Apenas faz o processo de contagem de linhas, e demais dados
          * analisados em alguns tipos de arquivos. */
         info_arquivo = contabiliza_linhas(stream);
      else {
         rejeitados++;
         // Pula pois arquivo não é analisável.
         fclose (stream);
         // Liberando 'caminho' criado.
         free (arquivo);
         continue;
      }
      n_linhas = info_arquivo.linhas + info_arquivo.linhas_vazias;
      coletado.total_de_linhas += n_linhas;
      coletado.media_de_linhas += (float)n_linhas / n_arquivos;
      fclose(stream);

      // Segundo registro de tempo seguido.
      gettimeofday(&final, NULL);

      if (decorrido_timeval(final, inicio) >= 0.300) 
      {
         char* rejeicoesstr = valor_legivel (rejeitados);
         printf (
            "Arquivos processados e rejeitados [%5lu/%8s] de %lu.\n", 
            processados++, rejeicoesstr, inicialmente
         );
         // liberando string depois de impressão na tela...
         free (rejeicoesstr);
         // Reseta contagem, registrando um novo tempo do marcador inicial.
         gettimeofday(&inicio, NULL);
      }
   } 
   char* rejeicoesstr = valor_legivel (rejeitados);
   printf (
      "Arquivos processados e rejeitados [%5lu/%8s] de %lu.\n", 
      processados++, rejeicoesstr, inicialmente
   );
   // liberando string depois de impressão na tela...
   free (rejeicoesstr);
   return coletado;
}


bool e_um_diretorio(char* caminho) {
   struct stat atributos;
   assert (stat(caminho, &atributos) == STAT_SUCESSO);
   return (bool)S_ISDIR(atributos.st_mode);
}

bool existe (char* caminho) {
   struct stat atributos;
   // se o resultado for zero, então quer dizer que existe.
   return stat(caminho, &atributos) == STAT_SUCESSO;
}


#ifdef __unit_tests__
/* == === === === === === === === === === === === === === === ==== == === =
 *                       Testes Unitários
 * == === === === === === === === === === === === === === === ==== == === */ 
void testando_com_caminhos_do_argumento(char* args[], int t) {
   for (size_t i = 1; i <= t; i++) {
      char* caminho = args[i];
      if (existe(caminho)) {
         printf ("caminho buscado: '%s'\n", caminho);
         struct stat atributos;
         mode_t tipo_do_caminho;

         stat (caminho, &atributos);
         tipo_do_caminho = atributos.st_mode;

         if (S_ISDIR(tipo_do_caminho))
            filtra_todos_arquivos(caminho);
         else
            puts ("não é um diretório!!!");
      }
   }
}

void funcao_existencia_do_caminho(char* args[], int t) {
   for (size_t k = 1; k <= t; k++) {
      char* caminho = args[k - 1];
      struct stat atributos;

      if (stat(caminho, &atributos) == -1)
         printf ("[não existe");
      else
         printf ("[existe");

      mode_t tipo = atributos.st_mode;
      if (S_ISREG(tipo))
         printf (", arquivo");
      else if (S_ISDIR(tipo))
         printf (", diretório");
      else if (S_ISFIFO(tipo))
         printf (", fifo");
      else if (S_ISCHR(tipo))
         printf (", caractére");
      printf ("] '%s'\n", caminho);
   }
}
#endif
