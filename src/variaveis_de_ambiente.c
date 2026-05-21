/*
 *   Cuida da listagem -- e outras coisas mais; de variáveis de ambientes
 * que este computador tem. Uma das mais famosas e importantes são a PATH,
 * ele dependendo de como ela está, fica ilegível. Tal módulo está aqui para
 * consertar isso.
 */

// Declaração das funções abaixo.
#include "variaveis_de_ambiente.h"
// Biblioteca padrão do C:
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>
// Biblioteca externa:
#include "listaarray_ref.h"

extern char** environ;

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
void realiza_impressao_colorida_da_entrada(char*, wchar_t);
void impressao_formatada_sem_coloracao(char* caminho, wchar_t indicador);

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
extern ListaStrings filtra_caminhos_de_path(void)
{
   char* conteudo = getenv("PATH");
   const ListaStrings VAZIO = { NULL, 0 };

   if (strlen(conteudo) == 0)
      return VAZIO;

   return reparte_ascii(conteudo, ":");
}

extern void mostra_conteudo_da_variavel_path(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   wchar_t INDICADOR = L'\u2794';
   const size_t QUANTIA = out.total;
   char** lista = out.lista;

   printf("\nTodos caminhos que pertecem ao PATH:\n\n");

   for (size_t i = 1; i <= QUANTIA; i++)
      realiza_impressao_colorida_da_entrada(lista[i - 1], INDICADOR);
   puts("\n");
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                      Funções Auxiliares
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
void realiza_impressao_colorida_da_entrada(char* caminho, wchar_t indicador)
{
/* Pega o caminho que é prá imprimir, e o símbolo Unicode do indicador,
 * arranja de forma bonita e agradável, então imprime com cores. Usando é
 * claro da tabela de cores ANSI.
 */
   // ANSI escape code colors.
   const char* AZUL    = "\033[1;94m",
             * FIM     = "\033[0m",
             * TAB     = "\t\b\b",
             * AMARELO = "\033[1;93m";

   // Estruturação das partes, aplicação das cores, tabulação, então aplica
   // a impressão.
   printf(
      "%s%s%-5lc%s %s%s%s\n", TAB, AMARELO, indicador,
      FIM, AZUL, caminho, FIM
   );
}

void impressao_formatada_sem_coloracao(char* caminho, wchar_t indicador)
{
/* Realiza uma impressão como a de cima, porém em preto e branco, sem qualquer
 * cor. */
   const char* TAB= "\t\b\b";

   printf("%s%-5lc %s\n", TAB, indicador, caminho);
}

#if defined(__unit_tests__) && defined(__linux__)
/* == === === === === === === === === === === === === === === ==== == === =
*                       Testes Unitários
* == === === === === === === === === === === === === === === ==== == === */
#include "teste.h"
#include "estringue.h"
#include "locale.h"
#include <stddef.h>
#include <time.h>


TESTE verificacao_da_filtragem_de_caminhos_do_path(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   debug_lista_strings(&out);
}

TESTE funcao_que_enlata_toda_visualizacao(void)
   { mostra_conteudo_da_variavel_path(); }

TESTE visualiza_mas_em_preto_e_branco(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   wchar_t INDICADOR = L'\u2794';
   const size_t QUANTIA = out.total;
   char** lista = out.lista;

   printf("\nTodos caminhos que pertecem ao PATH:\n\n");

   for (size_t i = 1; i <= QUANTIA; i++)
      impressao_formatada_sem_coloracao(lista[i - 1], INDICADOR);
   puts("\n");
}


int main(int total, char* args[], char* envs[])
{
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   executa_testes_b(
     true, 3,
        Unit(verificacao_da_filtragem_de_caminhos_do_path, true),
        Unit(funcao_que_enlata_toda_visualizacao, true),
        Unit(visualiza_mas_em_preto_e_branco, true)
   );

   return EXIT_SUCCESS;
}

#endif
