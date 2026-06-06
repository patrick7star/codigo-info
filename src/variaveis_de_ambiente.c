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
#include "conjunto_ref.h"
#include "estringue.h"

extern char** environ;
struct ReparticaoSistema { Set saco_linux; Set saco_win; };
typedef char* Path, *Caminho;

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
void realiza_impressao_colorida_da_entrada(char*, wchar_t);
void impressao_formatada_sem_coloracao(char* caminho, wchar_t indicador);
static bool e_um_caminho_windows(Path caminho);
static struct ReparticaoSistema 
  reparte_lista_de_forma_organizada(ListaStrings input);
static bool e_um_caminho_windows(Path caminho);

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
   free_lista_strings(&out);
}

void mostra_conteudo_da_variavel_path_organizado(void)
{
   ListaStrings input;
   wchar_t INDICADOR = L'\u2794';
   struct ReparticaoSistema output;
   char* remocao = NULL;
   const char* TAB = "\t\b\b\b\b\b\b";

   input = filtra_caminhos_de_path();
   output = reparte_lista_de_forma_organizada(input);

   printf("\nTodos caminhos que pertecem ao PATH:\n\n");
   printf("%sCaminhos do Windows:\n", TAB);

   while (!empty_set(output.saco_win)) {
      remocao = deleta_set(output.saco_win);
      realiza_impressao_colorida_da_entrada(remocao, INDICADOR);
   }

   printf("\n%sCaminhos do Linux:\n", TAB);

   while (!empty_set(output.saco_linux)) {
      remocao = deleta_set(output.saco_linux);
      realiza_impressao_colorida_da_entrada(remocao, INDICADOR);
   }
   puts("\n");

   drop_set(output.saco_linux);
   drop_set(output.saco_win);
   free_lista_strings(&input);
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                      Funções Auxiliares
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
static bool e_um_caminho_windows(Path caminho)
{
   const char* const MATCH = "/mnt/c/";
   char* endereco = strstr(caminho, MATCH);

   // Tem que ter o trecho, e ambos começarem do mesmo endereço.
   return (endereco != NULL && endereco == caminho);
}
static struct ReparticaoSistema 
reparte_lista_de_forma_organizada(ListaStrings input)
{ 
/* O algoritmo pega a lista com os caminhos, então separa, colocando nos 
 * específicos conjuntos, cada caminho equivalente ao sistema. 
 */
   int k, total = input.total;
   char** lista = input.lista;

   Set saco_linux = cria_set(hash_string, eq_string);
   Set saco_win = cria_set(hash_string, eq_string);

   for (k = 0; k < total; k++)
   {
      if (e_um_caminho_windows(lista[k])) 
         add_set(saco_win, lista[k]);
      else 
         add_set(saco_linux, lista[k]);
   }

   return (struct ReparticaoSistema){saco_linux, saco_win};
}

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
#include "locale.h"
#include <stddef.h>
#include <time.h>

TESTE verificacao_da_filtragem_de_caminhos_do_path(void);
TESTE funcao_que_enlata_toda_visualizacao(void);
TESTE visualiza_mas_em_preto_e_branco(void);
TESTE reparticao_da_lista_por_sistema(void);

TESTE nova_visualizacao_com_caminhos_reorganizados(void)
{
   mostra_conteudo_da_variavel_path_organizado();
}

int main(int total, char* args[], char* envs[])
{
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   executa_testes_b(
     true, 5,
        Unit(nova_visualizacao_com_caminhos_reorganizados, true),
        Unit(reparticao_da_lista_por_sistema, false),
        Unit(verificacao_da_filtragem_de_caminhos_do_path, false),
        Unit(funcao_que_enlata_toda_visualizacao, false),
        Unit(visualiza_mas_em_preto_e_branco, false)
   );

   return EXIT_SUCCESS;
}

TESTE reparticao_da_lista_por_sistema(void)
{
   auto input = filtra_caminhos_de_path();
   int k, total = input.total, soma;
   char** lista = input.lista, *remocao = NULL;

   Set saco_linux = cria_set(hash_string, eq_string);
   Set saco_win = cria_set(hash_string, eq_string);

   for (k = 0; k < total; k++)
   {
      if (e_um_caminho_windows(lista[k])) 
         add_set(saco_win, lista[k]);
      else 
         add_set(saco_linux, lista[k]);
   }
   soma = length_set(saco_linux) + length_set(saco_win);

   printf("Tamanho do Linux: %zu\n", length_set(saco_linux));
   printf("Tamanho do Win: %zu\n", length_set(saco_win));
   printf("Total de caminhos: %d\n", soma);

   puts("\nPrimeiros caminhos Windows:");
   while (!empty_set(saco_win))
   {
      remocao = (char*)deleta_set(saco_win);

      printf("- %s\n", remocao);
      free(remocao);
   }
   puts("\nAgora caminhos do Linux:");
   while (!empty_set(saco_linux))
   {
      remocao = (char*)deleta_set(saco_linux);

      printf("- %s\n", remocao);
      free(remocao);
   }
   drop_set(saco_linux);
   drop_set(saco_win);
}



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




#endif
