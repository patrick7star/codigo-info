/*   Provem chamadas de funcionalidades externas, ou seja, programas que 
 * foram apenas copiados para o código para extender as fucionalidades
 * destes. Deixar as chamadas e esquemas de execução deles neste arquivo
 * faz algo mais organizado. 
 */

// Biblioteca padrão do C:
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// Biblioteca do Linux(Glibc):
#include <unistd.h>

// Um novo apelido para este tipo de string.
typedef char* Caminho;


static Caminho caminho_cmd_frequencia(void)
{
   const int N = 500, sz = sizeof(char);
   char caminho[N];
   Caminho base, output;
   int length = 0;
   const Caminho RESTANTE = {
      #ifdef __debug__
      "/codigo-info/bin/programs/cmd-frequencia-debug"
      #else
      "/codigo-info/bin/programs/cmd-frequencia"
      #endif
   };

   memset(caminho, 0x00, N);
   base = getenv("CCODES");
   strcpy(caminho, base);
   strcat(caminho, RESTANTE);

   length = strlen(caminho) + 1;
   output = (Caminho)calloc(length, sz);
   strcpy(output, caminho);

   return output;
}

static Caminho caminho_pacotes_externos(void)
{
   const int N = 500, sz = sizeof(char);
   Caminho output, base;
   const Caminho RESTANTE = {
      #ifdef __debug__
      "/codigo-info/bin/programs/pacotes-externos-debug"
      #else
      "/codigo-info/bin/programs/pacotes-externos"
      #endif
   };

   output = (Caminho)calloc(N, sz);
   base = getenv("CCODES");
   strcpy(output, base);
   strcat(output, RESTANTE);

   return output;
}

static void free_caminho(Caminho In)
   { free(In); }

void pacotes_externos(void) 
{
   Caminho caminho = caminho_pacotes_externos();
   int exitcode = execl(
      caminho, "pacotes-externos", 
      NULL, (char*)NULL
   );

   if (exitcode == -1) {
      puts("O programa falhou na execução.");
      perror(strerror(errno));
      puts(caminho);

   } else
      puts("O programa executou normalmente.");
   free_caminho(caminho);
}

void cmd_frequencia(const char* OPCAO)
{
   Caminho caminho = caminho_cmd_frequencia();
   int exitcode = execl(
      caminho, "cmd-frquencia", 
      OPCAO, (char*)NULL
   );

   if (exitcode == -1) {
      puts("O programa falhou na execução.");
      perror(strerror(errno));

   } else
      puts("O programa executou normalmente.");
}

#ifdef __unit_tests__
#include "teste.h"

void obtendo_respectivos_caminhos(void)
{
   Caminho In_a = caminho_cmd_frequencia();
   Caminho In_b = caminho_pacotes_externos();

   puts(In_a);
   puts(In_b);

   free_caminho(In_a);
   free_caminho(In_b);
}

void execucao_crua_do_pacotes_externos(void)
   { pacotes_externos(); }

void execucao_do_cmd_frequencia_tendencia(void)
   { cmd_frequencia("-M"); }

void execucao_do_cmd_frequencia_tudo(void)
   { cmd_frequencia("-t"); }

int main(int total, char* argumentos[]) 
{
   executa_testes_b(
      true, 4,
         Unit(obtendo_respectivos_caminhos, true),
         Unit(execucao_crua_do_pacotes_externos, false),
         Unit(execucao_do_cmd_frequencia_tendencia, false),
         Unit(execucao_do_cmd_frequencia_tudo, true)
   );
   return EXIT_SUCCESS;
}
#endif
