
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



/*   A parte abaixo se divide em dois modos: o primeiro é o a parte dos
 * testes unitários, e bibliiotecas necessárias para executar-lo; já o 
 * segundo bloco 'main' é deixado para executar o programa de fato.
 *   Tal modo de testes só é acionado se é especificado, na instrunção de 
 * compilação, a constante de debug ou teste unitário, seja como está sendo
 * chamado agora.
 */
#ifdef __unit_tests__
/* == === === === === === === === === === === === === === === ==== == === =
 *                       Testes Unitários
 *                    da Combinação de Arquivos 
 *                          Acima
 * == === === === === === === === === === === === === === === ==== == === */ 



void teste_simples_do_processa_projeto(char* args[], int t) {
   for (size_t k = 1; k <= t; k++) {
      char* path = args[k];
      // pula caminhos inválidos.
      if (!existe(path)) continue;
      struct diretorio_info agregado = processa_projeto(args[k]);
      // visualização da informação agregada.
      visualiza_diretorio_info (&agregado);
   }
}

void verificando_checacao_de_executavel(void) {
   char* entradas[] = {
      "primos/target/release/primos",
      "alternador-wallpapers/target/release/alternador_wallpapers",
		"bin/codigo_info",
		"bin/codigo_info_debug",
   };
   char* raiz = getenv ("RUST_CODES");
   char caminho[300];

   for (size_t i = 1; i <= 2; i++) {
		int indice = i - 1;
      strcpy (caminho, ""); // zerando string.
      strcat (caminho, raiz);
      strcat (caminho, "/");
      strcat (caminho, entradas[indice]);
      assert (e_um_executavel (caminho));
      printf ("[ELF] %s.\n", caminho);
   }

   for (size_t i = 3; i <= 4; i++) {
		char* caminho = entradas[i - 1];
      printf ("[ELF] %s.\n", caminho);
      assert (e_um_executavel (caminho));
	}
}

void main(int total, char* argumentos[]) {
   // funcao_existencia_do_caminho (argumentos, total);
   // testando_com_caminhos_do_argumento(argumentos, total);
   // teste_simples_do_processa_projeto(argumentos, total);
   // verificando_checacao_de_executavel();

   // Ativando caractéres Unicode ...
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);
   
   menu_interface_do_programa(argumentos, total);
}

#elif defined(__release__)
/* Programa bem produzido, com sinais de erros e outros tipos, tudo mansuado
 * o máximo corretamente possível para compreensíveis mensagens de erro. */
int main(int total, char* argumentos[]) 
{
   // Ativando caractéres Unicode ...
   char* lang = getenv("LANG");
   setlocale(LC_CTYPE, lang);

   menu_interface_do_programa(argumentos, total);
   return EXIT_SUCCESS;
}
#endif
