// Declaração das funções abaixos:
#include "menu.h"
// Biblioteca padrão do C:
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
// Outros módulos do projeto:
#include "filtro.h"
#include "variaveis_de_ambiente.h"
#include "linque.h"
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
      "\t\t-i, --info-projeto <RAIZ>\n\t\t\b informação processada de todos arquivos de tal projeto.\n" 
      "\t\t-L, --repositorio-links \n\t\t\b os linques dos repostiórios de "
      "\n\t\tlinques de programas feitos, e se os linques ali, ainda são válidos"
      "\n\t\t-t, --todos-comandos \n\t\t\b Todo histórico de comandos já "
      "\n\t\tdigitado, e sua respectiva frequência."
      "\n\t\t-M, --comandos-mais-relevantes \n\t\t\b Apenas os comandos com "
      "\n\t\talguma relevância numérica."
      "\n\t\t-e, --rust-antigo-cache \n\t\t\b A o gerenciador de pacotes  "
      "\n\t\texternos faz um bocado de downloads, este aqui lista e organiza-os."
      // Quebra-de-linha no final.
      "\n\n",
      RECUO, PROG, RECUO, PROG, PROG, RECUO
   );
}

#ifdef __debug__
static void visualiza_argumentos(char* args[], int quantia) {
   const char* TAB = "\t\b\b\b";

   printf("Atual diretório: %s\n", getcwd(NULL, 255));
   printf("Quantidade: %d\n", quantia);

   if (quantia > 0) {
      for (int n = 1; n <= quantia; n++)
         printf("%s==> '%s'\n", TAB, args[n - 1]);
   }
}
#endif


extern void menu_interface_do_programa(char* args[], int total)
{
   const char* const CURTAS = "hi::P::L::M::t::e::";
   int result = getopt(total, args, CURTAS);
   const int Falhou = -1;
   const int MAX = 2 * UCHAR_MAX;

   #ifdef __debug__
   const char* programas[] = {
      "/home/alice/Documents/códigos/c-language/codigo-info/bin/programs/cmd-frequencia-debug",
      "/home/alice/Documents/códigos/c-language/codigo-info/bin/programs/pacotes-externos-debug",
   };
   #else
   const char* programas[] = {
      "/home/alice/Documents/códigos/c-language/codigo-info/bin/programs/cmd-frequencia",
      "/home/alice/Documents/códigos/c-language/codigo-info/bin/programs/pacotes-externos",
   };
   #endif
   int exitcode;

   #ifdef __debug__
   visualiza_argumentos(args, total);
   #endif

   while (result != Falhou) {
      char opcao = (char)result;

      if (opcao == '?')
         puts("Opção não existe!");
      else if (opcao == 'h')
         folha_de_ajuda();
      else if (opcao == 'P')
         mostra_conteudo_da_variavel_path();

      else if (opcao == 'i') {
         char* caminho = args[optind];

         DirInfo agregado = processa_projeto(caminho);
         visualiza_diretorio_info(&agregado);

      } else if (opcao == 'L') 
         info_sobre_repositorio_de_linques();
      else if (opcao == 't' || opcao == 'M') {
         exitcode = execl(
            programas[0], "cmd-frquencia", 
            args[1], (char*)NULL
         );

         if (exitcode == -1) {
            puts("O programa falhou na execução.");
            perror(strerror(errno));

         } else
            puts("O programa executou normalmente.");
      } else if (opcao == 'e') {
         exitcode = execl(
            programas[1], "pacotes-externos", 
            NULL, (char*)NULL
         );

         if (exitcode == -1) {
            puts("O programa falhou na execução.");
            perror(strerror(errno));

         } else
            puts("O programa executou normalmente.");
      }


      #ifdef __debug__
      variaveis_externas();
      #endif

      result = getopt(total, args, CURTAS);
   }
}
