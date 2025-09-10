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
   const char* AZUL = "\033[1;94m", *FIM = "\033[0m", *TAB = "\t\b\b";
   const char* AMARELO = "\033[1;93m";

   printf("\nTodos caminhos que pertecem ao PATH:\n\n");

   for (int i = 1; (size_t)i <= (out).total; i++) {
      printf(
         "%s%s%-5lc%s %s%s%s\n", TAB, AMARELO, INDICADOR, 
         FIM, AZUL, out.lista[i - 1], FIM
      );
   }
   puts("\n");
}

// static char* generico_to_string(Generico e)
//    { return (char*)e; }

// static bool free_string(Generico e)
//   { free((char*)e); return true; }

void listagem_de_todas_variaveis_de_usuario(void) {
   char** cursor = environ;
   int total = 0;
   Vetor lista = cria_al();
   const int sz = sizeof(char);
   const int IGUAIS = 0;

   while (*cursor != NULL) 
   {
      if (*cursor != NULL) 
      {
         char* igualdade = strchr(*cursor, '=');
         ptrdiff_t quantia = igualdade - *cursor;
         char* nome = malloc(UCHAR_MAX * sz);
         
         strncpy(nome, *cursor, quantia);
         nome[quantia] = '\0';

         if (strcmp(nome, "_") == IGUAIS) 
            free(nome);
         else
            insere_al(lista, nome);
      }
      cursor++;
      total++;
   }

   printf("\nHá %d variáveis de ambiente configuradas.\n", total);
   // NOTA: É preciso desalocar a memória alocada. Terei que desabilitar tal 
   // instrução aqui, pois ainda não consegui deixar estável na biblioteca
   // que importei pra este projeto.
   // destroi_interno_al(lista, free_string);
}

void variaveis_definidas_pelo_usuario(void) {
   char caminho[UCHAR_MAX];
   const char* BASE = getenv("HOME");

   sprintf(caminho, "%s/%s", BASE, ".profile");
   puts(caminho);

   FILE* arquivo = fopen(caminho, "rt");
   ssize_t lido; const int sz = sizeof(int);
   size_t TOTAL = UCHAR_MAX, contado = 0;
   char* buffer = calloc(TOTAL, sz);
   const char* const PATTERN = "export";

   do {
      lido = getline(&buffer, &TOTAL, arquivo);

      // Busca por variáveis criadas(começam com 'export').
      if (strstr(buffer, PATTERN) != NULL)
         printf("[%03ld] %s\n", contado, buffer);

      // Conta o total de linhas iteradas.
      contado++;
   } while(lido != -1);

   puts("Chegou-se ao fim do arquivo.");
   fclose(arquivo);
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

static void debug_lista_strings(ListaStrings* list) {
   printf("Lista de Strings: \n");

   for (int i = 1; i <= (*list).total; i++)
      printf("\t\b\b\u2794 %s\n", (*list).lista[i - 1]);
   puts("\n");
}

static void verificacao_da_filtragem_de_caminhos_do_path(void)
{
   ListaStrings out = filtra_caminhos_de_path();
   debug_lista_strings(&out);
}

static void funcao_que_enlata_toda_visualizacao(void)
   { mostra_conteudo_da_variavel_path(); }

static void sobre_variaveis_de_ambiente(void)
{
  listagem_de_todas_variaveis_de_usuario(); 
  variaveis_definidas_pelo_usuario();
}

static void coloracoes_definidas_via_ls_colors(void) {
   const char* CONTEUDO_VAR = getenv("LS_COLORS"), PADRAO = ':'; 
   long length = strlen(CONTEUDO_VAR);
   ListaStrings output;

   output = split_at(CONTEUDO_VAR, ':');
}


int main(int total, char* args[], char* envs[]) 
{
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   executa_testes_b(
     true, 4, 
        Unit(verificacao_da_filtragem_de_caminhos_do_path, true),
        Unit(funcao_que_enlata_toda_visualizacao, true),
        Unit(sobre_variaveis_de_ambiente, false),
        Unit(coloracoes_definidas_via_ls_colors, true)
   );

   return EXIT_SUCCESS;
}

#endif
