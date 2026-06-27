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
#include "caminho-base.h"

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
   int exitcode;

   // Visualizando caminho usado.
   exitcode = execl(caminho, "pacotes-externos", "--listagem", (char*)NULL);

   if (exitcode == -1) {
      puts("O programa falhou na execução.");
      perror(strerror(errno));
      puts(caminho);

   } else
      puts("O programa executou normalmente.");
   libera_caminho_externo(caminho);
}

void cmd_frequencia(const char* OPCAO)
{
   Caminho caminho;
   const char* TERMINO = (char*)NULL;
   int exitcode; 

   caminho = caminho_cmd_frequencia();
   #ifdef __debug__
   printf("Caminho: '%s'\n", caminho);
   #endif
   exitcode = execl(caminho, "cmd-frquencia", OPCAO, TERMINO);

   if (exitcode == -1) {
      puts("O programa falhou na execução.");
      perror(strerror(errno));
      abort();

   } 
   // Nunca é alcançado, pois função 'exec' substitui toda memória do processo.
   // Más, por uma boa prática, aqui continua.
   libera_caminho_externo(caminho);
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
   // Caminho output = caminho_base_do_programa();

   // Juntando os caminhos ...
   // junta_caminhos(output, RESTANTE);
   // return output;
   return computa_caminho_externo(RESTANTE);
}

static Caminho caminho_pacotes_externos(void)
{
/* O mesmo que acima, porém o executável referente é outro do projeto.
 * neste caso é o 'programa-externos'. */
   const Caminho RESTANTE = {
      #ifdef __debug__
      // (Por algum motivo não funciona, ao menos no Linux[WSL])
      "bin/programs/pacotes-externos-debug"
      #else
      "bin/programs/pacotes-externos"
      #endif
   };
   /*
   Caminho output = caminho_base_do_programa();

   #ifdef __debug__
      printf("Restante: '%s'\n", RESTANTE);
      printf("Base: '%s'\n", output);
   #endif

   // Juntando os caminhos ...
   strcat(output, RESTANTE);
   return output;*/
   return computa_caminho_externo(RESTANTE);
}

static void free_caminho(Caminho In)
   { free(In); }

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                       Testes Unitários
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
#ifdef __unit_tests__
#include "teste.h"

TESTE obtendo_respectivos_caminhos(void);
TESTE execucao_crua_do_pacotes_externos(void);
TESTE execucao_do_cmd_frequencia_tendencia(void);
TESTE execucao_do_cmd_frequencia_tudo(void);
TESTE mexendo_com_chamada_execl(void);

int main(int total, char* argumentos[])
{
   executa_testes_b(
      false, 1,
         Unit(mexendo_com_chamada_execl, true)
   );
   executa_testes_b(
      true, 4,
         Unit(obtendo_respectivos_caminhos, false),
         Unit(execucao_crua_do_pacotes_externos, false),
         Unit(execucao_do_cmd_frequencia_tendencia, false),
         Unit(execucao_do_cmd_frequencia_tudo, true)
   );

   return EXIT_SUCCESS;
}

TESTE mexendo_com_chamada_execl(void)
{
   char* TERMINO = (char*)NULL;
   const char* argumentos = "-M";
   int result;
   Caminho CMD;

   CMD = computa_caminho_externo("bin/programs/cmd-frequencia-debug");
   result = execl(CMD, "cmd-frequencia-debug", argumentos, TERMINO);

   if (result == -1)
      perror(strerror(errno));
   printf("Caminho: '%s'\n", CMD);
   libera_caminho_externo(CMD);
}

TESTE execucao_do_cmd_frequencia_tudo(void)
   { cmd_frequencia("-t"); }

TESTE obtendo_respectivos_caminhos(void)
{
   Caminho In_a = caminho_cmd_frequencia();
   Caminho In_b = caminho_pacotes_externos();

   puts(In_a);
   puts(In_b);

   free_caminho(In_a);
   free_caminho(In_b);
}

TESTE execucao_crua_do_pacotes_externos(void)
   { pacotes_externos(); }

TESTE execucao_do_cmd_frequencia_tendencia(void)
   { cmd_frequencia("-M"); }

#endif
