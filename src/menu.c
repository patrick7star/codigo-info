// Declaração das funções abaixos:
#include "menu.h"
// Biblioteca padrão do C:
#include <stdio.h>
// Outros módulos do projeto:
#include "filtro.h"
#include "variaveis_de_ambiente.h"
// Biblioteca do Linux(Glibc):
#include <unistd.h>


#ifdef __debug__
static void variaveis_externas(void) {
   const char* const SEP = "\t\b\b";

   if (optarg == NULL)
      printf(
         "%sopterr: %d\n%soptind: %d\n%soptopt: %c\n%soptarg: %s\n\n", 
         SEP, opterr, SEP, optind, SEP, optopt, SEP, optarg
      );
   else
      printf(
         "%sopterr: %d\n%soptind: %d\n%soptopt: %c\n%soptarg: '%s'\n\n", 
         SEP, opterr, SEP, optind, SEP, optopt, SEP, optarg
      );
}
#endif

static void folha_de_ajuda(void) {
   const char* const PROG        = "codigo_info";
   const char* const RECUO       = "\n\t";

   printf(
      "\nNAME%s%s - Visualize e processo seus códigos bases, e atuais "
      "configurações da sua máquina. \n\n"

      "SINOPSE%s %s -h\n\t %s [OPTIONS] [DIR] \n\n"

      "DESCRIÇÃO%sO programa tem várias capilaridades na verdade. O foco "
      "principal é o código base do computador, assim com algumas defini"
      "ções da atual máquina. Por exemplo, a função principal dele foi de"
      "finida em listar todos tipos de arquivos, dada uma determinada raíz"
      ". Entretanto, uma função que ele faz é listar os diretórios listados "
      "na variável PATH, que é igualmente importante no processo de desen"
      "volvimento.\n\tAs seguintes simples opções estão disponíveis, que "
      "foram as duas funcionalidades mencionadas acima:\n"
      "\t\t-P, --path-var\n\t\t\b lista todos caminhos anexados no caminho PATH.\n"
      "\t\t-i, --info-projeto <RAIZ>\n\t\t\b informação processada de todos arquivos de tal projeto.\n\n", 
      RECUO, PROG, RECUO, PROG, PROG, RECUO
   );
}

extern void menu_interface_do_programa(char* args[], int total)
{
   const char* const CURTAS = "hi::P::";
   int result = getopt(total, args, CURTAS);
   const int Falhou = -1;

   while (result != Falhou) {
      char opcao = (char)result;

      if (opcao == '?')
         puts("Opção não existe!");
      else if (opcao == 'h')
         folha_de_ajuda();
      else if (opcao == 'P')
         mostra_conteudo_da_variavel_path();
      else if (opcao == 'i') 
      {
         char* caminho = args[optind];

         DirInfo agregado = processa_projeto(caminho);
         visualiza_diretorio_info(&agregado);
      }

      #ifdef __debug__
      variaveis_externas();
      #endif

      result = getopt(total, args, CURTAS);
   }
}
