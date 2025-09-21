// Declaração das funções abaixos:
#include "menu.h"
// Biblioteca padrão do C:
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
// Outros módulos do projeto:
#include "filtro.h"
#include "variaveis_de_ambiente.h"
#include "linque.h"
// Biblioteca do Linux(Glibc):
#include <unistd.h>

const char* const RECUO = "\t\b\b\b";
const int IgualStr = 0;
const char* const VAZIA = "";


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

static void visualiza_opcao(struct Opcao * obj) {
/*   Visualização de uma opção definida. Se adapta com o que foi demanada.
 * Ou seja, se não forma curta, ela não aparece, como foi definda. Não aceita
 * 'Opção' sem descrição, ou forma longa do comando. */
   bool algumas_da_opcoes_invalida = {
      strcmp(obj->descricao, VAZIA) == IgualStr ||
      strcmp(obj->longa, VAZIA) == IgualStr
   };
   char argumento[UCHAR_MAX];

   if (algumas_da_opcoes_invalida) {
      perror("'longa' e 'descrição' tem que ter algo válido.");
      abort();
   }
         
   if ((*obj).metavar == NULL) 
      strcpy(argumento, VAZIA);
   else {
      strcat(argumento, "=");
      strcat(argumento, (*obj).metavar);
   }

   if ((*obj).curta == 0x0)
      printf( "%s--%s%s\t%s.\n", RECUO, obj->longa, argumento, obj->descricao);
   else
      printf(
         "%s-%c --%s%s\n\t%s.\n", 
         RECUO, obj->curta, obj->longa, argumento, obj->descricao
      );
}

extern void mostrar_manual(const char* nome, const char* modo_de_uso, 
  const char* descricao, struct Opcao* opcoes, const int n) 
{
/*   Apresenta um manual simples pra qualquer programa criado. Ele necessita
 * apenas do nome do programa, seu modo de uso, a descrição do que ele faz,
 * e a lista de opções disponíveis. Está última é definda com um atalho,
 * sua forma longa, e descrição do comando. */
   printf(
      "NOME\n%s%s%s\b%s\n\nDESCRIÇÃO\n%s%s\n\nOPÇÕES:\n",
      RECUO, nome,RECUO, modo_de_uso, RECUO, descricao
   );

   for (int k = 1; k <= n; k++, opcoes++)
      visualiza_opcao(opcoes);
}

static void folha_de_ajuda(void) {
/*   Nova função que define como usa o programa. Apesar de o resultado ser 
 * bem similar, está é muito melhor, já que, tem uma esquematica bem mais
 * genérica, assim, para futuras aplicações, copiar tal código ajuda muito
 * na construção. */
   const char* const PROG        = "codigo_info";
   const char* const SINOPSE = "[OPTIONS] [DIR]"; 
   const char* const DESCRICAO = {
      "O programa tem várias capilaridades na verdade. O foco "
      "principal é o código base do computador, assim com algumas defini"
      "ções da atual máquina. Por exemplo, a função principal dele foi de"
      "finida em listar todos tipos de arquivos, dada uma determinada raíz"
      ". Entretanto, uma função que ele faz é listar os diretórios listados "
      "na variável PATH, que é igualmente importante no processo de desen"
      "volvimento."
   };
   struct Opcao OPCOES[] = {
      (struct Opcao) {
         'P', "path-var",
         "Lista todos caminhos anexados no caminho PATH",
         NULL
      },
      (struct Opcao) {
         'i', "info-projeto",
         "Informação processada de todos arquivos de tal projeto",
         "RAIZ"
      },
      (struct Opcao) {
         'L', "repositorio-links",
         "Os linques dos repostiórios de linques de programas feitos, e se "
         "os linques ali, ainda são válidos", NULL
      },
      (struct Opcao) {
         't', "todos-comandos",
         "Todo histórico de comandos já digitado, e sua respectiva "
         "frequência", NULL
      },
      (struct Opcao) {
         'M', "comandos-mais-relevantes",
         "Apenas os comandos com alguma relevância numérica", NULL
      },
      (struct Opcao) {
         'e', "rust-cache",
         "A o gerenciador de pacotes externos faz um bocado de downloads, "
         "este aqui lista e organiza-os.", NULL
      }
   };
   const int size = sizeof(struct Opcao);
   const int M = sizeof(OPCOES) / size;

   mostrar_manual(PROG, SINOPSE, DESCRICAO, OPCOES, M);
   putchar('\n');
}

extern void menu_interface_do_programa(char* args[], int total)
{
   const char* const CURTAS = "hi::P::L::M::t::e::";
   int result = getopt(total, args, CURTAS);
   const int Falhou = -1;

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
         // folha_de_ajuda();
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

#ifdef __unit_tests__
#include <stdlib.h>

int main(int total, char* argumentos[]) 
{
   struct Opcao options[] = {
      (struct Opcao){'l', "listagem", "lista todas certas coisas", NULL},
      (struct Opcao){'c', "caminho", "mostra caminho executado", NULL},
      (struct Opcao)
        {'s', "suave", "algo bem suave já foi construída aqui", NULL},
      (struct Opcao) {
         0x00, "comando-inútil", 
         "Não consigo observar uma descrição prá algo bem inútil",
         NULL
      }
   };
   const int N = sizeof(options) / sizeof(struct Opcao);

   mostrar_manual(
      "CódigoInfo", "[OPTIONS] <DIR>", 
      "O programa tem várias capilaridades, sendo a principal dela cuidar "
      "de vários interminentes problemas, que irei falar a seguir. I know, "
      "you think still it there", options, N
   );
   return EXIT_SUCCESS;
}

#endif
