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
typedef Caminho const ConstPath;

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 *
 * Declarada aqui, pois é preciso que as funções públicas "sabiam" que as
 * funções que são chamadas existem.
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
static void junta_caminhos(Caminho a, ConstPath b);
static Caminho caminho_base_do_programa(void);
static Caminho caminho_cmd_frequencia(void);
static Caminho caminho_pacotes_externos(void);
static void free_caminho(Caminho In);

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
void pacotes_externos(void)
{
   Caminho caminho = caminho_pacotes_externos();
   int exitcode = execl(
      caminho, "pacotes-externos",
      "--listagem", (char*)NULL
   );

   #ifdef __debug__
   // Visualizando caminho usado.
   printf("Caminho: '%s'\n", caminho);
   #endif

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
/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                   Interface Privada Implementação
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
static void junta_caminhos(Caminho a, ConstPath b)
{
// Adiciona o caminho 'b' no caminho 'a'. Este último é modificado.
   ConstPath SEP = "/";

   strcat(a, SEP);
   strcat(a, b);
}

static Caminho caminho_base_do_programa(void)
{
/* Retorna uma string com a base do programa, onde dali, é possível computar
 * todos os caminhos dos demais executáveis. Memória que será alocada é de
 * responsabilidade do chamador em destrui-la. Tal caminho é obitido baseado
 * na definição da variável de ambiente 'HOME', se ela não estiver
 * definida, o programa será interrompido. */
   const int MAXIMO = 500, sz = sizeof(char);
   Caminho output = NULL, caminho = NULL;
   const char* VARIAVEL = "HOME";
   ConstPath COMPLEMENTO = "programas/codigo-info";

   output = calloc(MAXIMO, sz);
   caminho = getenv(VARIAVEL);

   if (strlen(caminho) > MAXIMO) {
      perror("O caminho da variável excede o tamanho do buffer!");
      abort();
   } else if (caminho == NULL) {
      perror("A variável não está definida!");
      abort();
   } else {
      strcpy(output, caminho);
      junta_caminhos(output, COMPLEMENTO);
   }

   return output;
}

static Caminho caminho_cmd_frequencia(void)
{
/* Retorna o caminho do executável dentro do diretório do projeto. Não é
 * verificado a validade de tal caminho, se ele existe ou não, será
 * retornado um. */
   const Caminho RESTANTE = {
      #ifdef __debug__
      "bin/programs/cmd-frequencia-debug"
      #else
      "bin/programs/cmd-frequencia"
      #endif
   };
   Caminho output = caminho_base_do_programa();

   // Juntando os caminhos ...
   junta_caminhos(output, RESTANTE);
   return output;
}

static Caminho caminho_pacotes_externos(void)
{
/* O mesmo que acima, porém o executável referente é outro do projeto.
 * neste caso é o 'programa-externos'. */
   const Caminho RESTANTE = {
      #ifdef __debug__
      // (Por algum motivo não funciona, ao menos no Linux[WSL])
      "/bin/programs/pacotes-externos-debug"
      #else
      "/bin/programs/pacotes-externos"
      #endif
   };
   Caminho output = caminho_base_do_programa();

   #ifdef __debug__
      printf("Restante: '%s'\n", RESTANTE);
      printf("Base: '%s'\n", output);
   #endif

   // Juntando os caminhos ...
   strcat(output, RESTANTE);
   return output;
}

static void free_caminho(Caminho In)
   { free(In); }

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                       Testes Unitários
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
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
