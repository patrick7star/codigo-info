/*   Vários modalidades de interface do menu do programa serão listadas 
 * aqui.
 *   Separando o menu já aqui, porque uma futura personalização fará tal 
 * trecho de código ficar ainda mais comprido e complexo. 
 */

#ifndef __MENU_H__
#define __MENU_H__
 
 struct Opcao { char curta; char* longa; char* descricao; char* metavar; };

 void menu_interface_do_programa (char* args[], int total);
 void mostrar_manual(
      const char* nome, const char* modo_de_uso, const char* descricao, 
      struct Opcao* opcoes, const int n
 ); 

#endif
