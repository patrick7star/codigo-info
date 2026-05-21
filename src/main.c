
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

// Biblioteca padrão do C:
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <locale.h>
#include <assert.h>
#include <string.h>
// Header files do Glibc:
#include <dirent.h>
#include <sys/time.h>
#include <unistd.h>
// Biblioteca externa com 'utilitários':
#include "legivel.h"
// Módulos deste projeto:
#include "variaveis_de_ambiente.h"
#include "menu.h"
#include "classificacao.h"
#include "filtro.h"
#include "linque.h"


/*   A parte abaixo se divide em dois modos: o primeiro é o a parte dos
 * testes unitários, e bibliiotecas necessárias para executar-lo; já o 
 * segundo bloco 'main' é deixado para executar o programa de fato.
 *   Tal modo de testes só é acionado se é especificado, na instrunção de 
 * compilação, a constante de debug ou teste unitário, seja como está sendo
 * chamado agora.
 */
#ifdef __debug__
/* == === === === === === === === === === === === === === === ==== == === =
 *                       Testes Unitários
 *                    da Combinação de Arquivos 
 *                          Acima
 * == === === === === === === === === === === === === === === ==== == === */ 
void main(int total, char* argumentos[]) 
{
   // Ativando caractéres Unicode ...
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   menu_interface_do_programa_i(argumentos, total);
}

#elif defined(__release__)
/* Programa bem produzido, com sinais de erros e outros tipos, tudo mansuado
 * o máximo corretamente possível para compreensíveis mensagens de erro. */
int main(int total, char* argumentos[]) 
{
   // Ativando caractéres Unicode ...
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   menu_interface_do_programa_i(argumentos, total);
   return EXIT_SUCCESS;
}
#endif
