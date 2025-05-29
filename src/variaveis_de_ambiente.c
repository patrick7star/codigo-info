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


ListaStrings filtra_caminhos_de_path(void) 
{
   char* conteudo = getenv("PATH"); 
   const ListaStrings VAZIO = { NULL, 0 };

   if (strlen(conteudo) == 0)
      return VAZIO;

   return reparte_ascii(conteudo, ":");
}

void mostra_conteudo_da_variavel_path(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   wchar_t INDICADOR = L'\u2794';

   printf("\nTodos caminhos que pertecem ao PATH: \n\n");

   for (int i = 1; (size_t)i <= (out).total; i++)
      printf("\t\b\b%-5lc %s\n", INDICADOR, out.lista[i - 1]);
   puts("\n");
}


#ifdef __unit_tests__
/* == === === === === === === === === === === === === === === ==== == === =
*                       Testes Unitários
* == === === === === === === === === === === === === === === ==== == === */ 
#include "teste.h"
#include "locale.h"

static void debug_lista_strings(ListaStrings* list) {
   printf("Lista de Strings: \n");

   for (int i = 1; i <= (*list).total; i++)
      printf("\t\b\b\u2794 %s\n", (*list).lista[i - 1]);
   puts("\n");
}

void verificacao_da_filtragem_de_caminhos_do_path(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   debug_lista_strings(&out);
}

void funcao_que_enlata_toda_visualizacao(void)
   { mostra_conteudo_da_variavel_path(); }

int main(int total, char* args[], char* envs[]) 
{
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   executa_testes_b(
     true, 2, 
        Unit(verificacao_da_filtragem_de_caminhos_do_path, true),
        Unit(funcao_que_enlata_toda_visualizacao, true)
   );

   return EXIT_SUCCESS;
}

#endif
