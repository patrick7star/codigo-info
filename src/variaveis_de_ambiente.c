/*   
 *   Cuida da listagem -- e outras coisas mais; de variáveis de ambientes 
 * que este computador tem. Uma das mais famosas e importantes são a PATH,
 * ele dependendo de como ela está, fica ilegível. Tal módulo está aqui para
 * consertar isso.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Bibliotecas externas:
#include "estringue.h"


ListaStrings filtra_caminhos_de_path(void) 
{
   char* conteudo = getenv("PATH"); 
   const ListaStrings VAZIO = { NULL, 0 };

   if (strlen(conteudo) == 0)
      return VAZIO;

   return reparte_ascii(conteudo, ":");
}


#ifdef __unit_tests__
/* == === === === === === === === === === === === === === === ==== == === =
*                       Testes Unitários
* == === === === === === === === === === === === === === === ==== == === */ 
#include "teste.h"

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

int main(int total, char* args[], char* envs[]) 
{
   executa_testes_b(
     true, 1, 
        Unit(verificacao_da_filtragem_de_caminhos_do_path, true)
   );

   return EXIT_SUCCESS;
}

#endif
