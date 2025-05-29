/*   Este módulo tem como papel específico em fornercer ferramentas de vários
 * tipos de filtragem, feitos durante a exploração de alguns componentes do
 * OS(como o sistema de arquivos, e as variáveis de ambientes, entre outros
 * também).
 */

#ifndef __FILTRO_H__
#define __FILTRO_H__
// Biblioteca padrão do C:
#include <stdbool.h>
// Biblioteca externa:
#include "listaarray_ref.h"
#include "hashtable_ref.h"

 bool existe (char* caminho); 

 typedef struct diretorio_info {
   // Raíz que é analisada(referência estática da string).
   char* caminho_do_projeto;
   // Quantidade arquivos dentro deste diretório.
   size_t total_de_arquivos;
   // Média de linhas por arquivo.
   float media_de_linhas;
   // Total de linhas do projeto como todo.
   size_t total_de_linhas;

   // Dicionário com todos 'tipos' coletados e suas respectivas frequências.
   HashTable todos_tipos; // seu tipo é 'char*: int'

 } DiretorioInfo, DirInfo, DI, *DirInfoRef, *DIRef;

 struct diretorio_info processa_projeto(char* caminho); 
 void visualiza_diretorio_info (DiretorioInfo* i); 

 vetor_t* filtra_todos_arquivos(char* caminho); 
 vetor_t* filtra_todos_arquivos_visualiza(char* caminho);

#endif

