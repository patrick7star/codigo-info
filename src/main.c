
/*   O programa abaixo dará informação sobre todos os códigos escritos 
 * neste computador, seja ele C/C++, Python, Go, Rust, etc. Qual o tipo de 
 * informação, você pergunta? Bem, o número de linhas de total projeto, a
 * quantidade de funções, bibliotecas usadas, coias do tipo. 
 *
 *   Por que tal projeto está sendo feito em C? Acho que não fiz ainda 
 * qualquer projeto sério em C, apenas em C++ e o de sempre Python e Rust,
 * nada em C, apesar de sua biblioteca utilitários dá muito trabalho de 
 * manter.
 */

// biblioteca padrão do C:
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// header files do Glibc:
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
// biblioteca externa com 'utilitários':
#include "legivel.h"
#include "tempo.h"
#include "hashtable_ref.h"
#include "memoria.h"

// Nemômicos dos resultados das operações:
#define STAT_FALHOU -1
#define STAT_SUCESSO 0

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

bool existe (char* caminho) {
   struct stat atributos;
   // se o resultado for zero, então quer dizer que existe.
   return stat(caminho, &atributos) == STAT_SUCESSO;
}

/* Em outro arquivo para organizar bem os tipos de módulos, e o que eles
 * fazem neste aqui: 
 *
 *          - TipoDeArquivo tipo_do_arquivo (char*)
 *          - const char* ta_to_str (TipoDeArquivo)
 *          - bool pode_ser_lido (TipoDeArquivo)
 */
#include "tipo_arquivo.c"

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

/* Filtra todos arquivos de um diretório, então seleciona apenas aqueles 
 * que são códigos de programação, ou arquivos de texto simples(alguma
 * coisa importante foi escrita nele):
 *
 *       - filtra_todos_arquivos
 *             In:  caminho(char*)
 *             Out: lista(Vetor)
 */
#include "filtro.c"

typedef struct diretorio_info {
   // Raíz que é analisada(referência estática da string).
   char* caminho_do_projeto;
   // Quantidade arquivos dentro deste diretório.
   size_t total_de_arquivos;
   // Média de linhas por arquivo.
   float media_de_linhas;
   // Total de linhas do projeto como todo.
   size_t total_de_linhas;

   // Dicionário com todos 'tipos' coletados e suas respectivas frequências.
   hashtable_t* todos_tipos; // seu tipo é 'char*: int'

} DiretorioInfo, DirInfo, DI, *DirInfoRef, *DIRef;

char* ht_to_str(HashTable m) {
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
   char* frequencias_coletadas = ht_to_str ((*i).todos_tipos);

   printf (
      "\n  caminho: '%s'\n"
      "  qtd. de arquivos: %s\n"
      "  média de linhas por arquivo: %0.1f\n"
      "  total de linhas: %s\n"
      "  tipos de arquivos: %s\n\n",
      i->caminho_do_projeto,
      valor_legivel (i->total_de_arquivos),
      i->media_de_linhas,
      valor_legivel (i->total_de_linhas),
      frequencias_coletadas
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
   // lista_caminhos = filtra_todos_arquivos(caminho);
   lista_caminhos = filtra_todos_arquivos_visualiza(caminho);
   coletado.total_de_arquivos = tamanho_al(lista_caminhos);
   coletado.caminho_do_projeto = caminho;
   n_arquivos = coletado.total_de_arquivos;

   puts ("Começou o processo de varredura dos arquivos ...");
   size_t processados = 0, rejeitados = 0;
   size_t inicialmente = tamanho_al (lista_caminhos);
   // Temporizador contador = cria_temporizador(Miliseg, 300);
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
         /* apenas faz o processo de contagem de linhas, e demais dados
          * analisados em alguns tipos de arquivos. */
         info_arquivo = contabiliza_linhas(stream);
      else {
         rejeitados++;
         // pula pois arquivo não é analisável.
         fclose (stream);
         // liberando 'caminho' criado.
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

/*   A parte abaixo se divide em dois modos: o primeiro é o a parte dos
 * testes unitários, e bibliiotecas necessárias para executar-lo; já o 
 * segundo bloco 'main' é deixado para executar o programa de fato.
 *   Tal modo de testes só é acionado se é especificado, na instrunção de 
 * compilação, a constante de debug ou teste unitário, seja como está sendo
 * chamado agora.
 */
#ifdef __unit_tests__
/* == === === === === === === === === === === === === === === ==== == === =
*                       Testes Unitários
*                    da Combinação de Arquivos 
*                          Acima
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

void teste_simples_do_processa_projeto(char* args[], int t) {
   for (size_t k = 1; k <= t; k++) {
      char* path = args[k];
      // pula caminhos inválidos.
      if (!existe(path)) continue;
      struct diretorio_info agregado = processa_projeto(args[k]);
      // visualização da informação agregada.
      visualiza_diretorio_info (&agregado);
   }
}

void verificando_checacao_de_executavel(void) {
   char* entradas[] = {
      "primos/target/release/primos",
      "alternador-wallpapers/target/release/alternador_wallpapers",
		"bin/codigo_info",
		"bin/codigo_info_debug",
   };
   char* raiz = getenv ("RUST_CODES");
   char caminho[300];

   for (size_t i = 1; i <= 2; i++) {
		int indice = i - 1;
      strcpy (caminho, ""); // zerando string.
      strcat (caminho, raiz);
      strcat (caminho, "/");
      strcat (caminho, entradas[indice]);
      assert (e_um_executavel (caminho));
      printf ("[ELF] %s.\n", caminho);
   }

   for (size_t i = 3; i <= 4; i++) {
		char* caminho = entradas[i - 1];
      printf ("[ELF] %s.\n", caminho);
      assert (e_um_executavel (caminho));
	}
}

// execução dos testes unitários ...
void main(int total, char* argumentos[]) {
   // funcao_existencia_do_caminho (argumentos, total);
   // testando_com_caminhos_do_argumento(argumentos, total);
   teste_simples_do_processa_projeto(argumentos, total);
   // verificando_checacao_de_executavel();
}

#elif defined(release)
/* Programa bem produzido, com sinais de erros e outros tipos, tudo mansuado
 * o máximo corretamente possível para compreensíveis mensagens de erro. */
int main(int total, char* argumentos[]) 
{
   char mensagem_error[300];
   char* caminho;

   // tem que ter o arquivo que será anaĺisado, e ser exatamente um.
   if (total == 1) {
      puts("erro: sem arquivo!");
      fflush(stderr);
      return EXIT_FAILURE;

   } else if (total == 2) {
      caminho = argumentos[1];
      // se o caminho não existe, então sair e alertar.
      if (!existe(caminho)) {
         sprintf (mensagem_error, "tal caminho '%s' não existe!", caminho);
         perror (mensagem_error);
         return EXIT_FAILURE;
      }
      if (e_um_diretorio (caminho)) {
         struct diretorio_info mastigado;
         mastigado = processa_projeto (caminho);
         visualiza_diretorio_info (&mastigado);
      } else 
         visualiza_info_do_arquivo (caminho);

   } else {
      // fazendo com cada um dos argumentos...
      for (size_t k = 1; k < total; k++) {
         caminho = argumentos[k];
         // se o caminho não existe, então sair e alertar.
         if (!existe(caminho)) {
            sprintf (
               mensagem_error, 
               "tal caminho '%s' não existe!", 
               caminho
            );
            perror (mensagem_error);
            return EXIT_FAILURE;
         }
         // passado verificação de existência ...
         mostra_nome_do_arquivo(caminho);
         visualiza_info_do_arquivo(caminho);
      }
   }
   return EXIT_SUCCESS;
}
#endif
