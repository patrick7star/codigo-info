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
static Caminho caminho_cmd_frequencia(void);
static Caminho caminho_pacotes_externos(void);

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
   Caminho caminho = caminho_cmd_frequencia();

   #ifdef __debug__
   printf("Caminho: '%s'\n", caminho);
   #endif

   if (execl(caminho, "cmd-frquencia", OPCAO, NULL) == -1)
   {
      perror("O programa falhou na execução.");
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
   return computa_caminho_externo(RESTANTE);
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                       Testes Unitários
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
#ifdef __unit_tests__
#include "teste.h"

TESTE execucao_crua_do_pacotes_externos(void);
TESTE execucao_do_cmd_frequencia_tendencia(void);
TESTE execucao_do_cmd_frequencia_tudo(void);
TESTE mexendo_com_chamada_execl(void);
TESTE caminhos_de_ambos_executaveis(void);

int main(int total, char* argumentos[])
{
   executa_testes_b(
      false, 1,
         Unit(mexendo_com_chamada_execl, true)
   );
   executa_testes_b(
      true, 4,
         Unit(caminhos_de_ambos_executaveis, true),
         Unit(execucao_crua_do_pacotes_externos, false),
         Unit(execucao_do_cmd_frequencia_tendencia, false),
         Unit(execucao_do_cmd_frequencia_tudo, true)
   );

   return EXIT_SUCCESS;
}

TESTE caminhos_de_ambos_executaveis(void)
{
   Caminho path_a = caminho_cmd_frequencia();
   Caminho path_b = caminho_pacotes_externos();

   printf("Cmd-Frequência: '%s'\nPacotes Externos: '%s'\n", path_a, path_b);
   libera_caminho_externo(path_a);
   libera_caminho_externo(path_b);
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

TESTE execucao_crua_do_pacotes_externos(void)
   { pacotes_externos(); }

TESTE execucao_do_cmd_frequencia_tendencia(void)
   { cmd_frequencia("-M"); }

#endif
