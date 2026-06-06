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
#include "funcionalidades.h"
#include "impressao.h"
#include "estringue.h"

// Biblioteca do Linux(Glibc):
#include <unistd.h>
#include <getopt.h>

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada                      
 * 
 *  Declarações das funções auxliares que serão utilizadas abaixo pelos as
 * parte pública do módulo. Elas estando sendo declaradas aqui -- na verdade
 * colocadas agora aqui; porque agora quero organizar o código de um modo
 * que as funções principais(as públicas) fiquem no topo. Entretanto, apenas
 * recorta-las e colar no topo não funciona. Colocar elas no topo, facilita 
 * a manutenção, e busca durante o desenvolvimento, já que não é preciso
 * ficar vasculhando ao longo do código funções públicas.
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */ 
const char* const RECUO = "\t\b\b\b";
const int IgualStr = 0;
const char* const VAZIA = "";

#ifdef __debug__
 static void variaveis_externas(void); 
 static void visualiza_argumentos(char* args[], int quantia); 
#endif
static void visualiza_opcao(struct Opcao * obj); 
static void folha_de_ajuda(void); 

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública                      
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */ 
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
extern void menu_interface_do_programa(char* args[], int total)
{
   const char* const CURTAS = "hi::P::L::M::t::e::";
   int result = getopt(total, args, CURTAS);
   const int Falhou = -1;
   char opcao, *caminho, *OPCAO;

   #ifdef __debug__
   visualiza_argumentos(args, total);
   #endif

   while (result != Falhou) {
      opcao = (char)result;

      if (opcao == '?')
         puts("Opção não existe!");
      else if (opcao == 'h')
         folha_de_ajuda();
      else if (opcao == 'P')
         mostra_conteudo_da_variavel_path();

      else if (opcao == 'i') {
         caminho = args[optind];

         DirInfo agregado = processa_projeto(caminho);
         visualiza_diretorio_info(&agregado);

      } else if (opcao == 'L') {
         printf("\nEstado escolhido: '%s'\n", optarg);
         info_sobre_repositorio_de_linques_ordenada();
      } else if (opcao == 't' || opcao == 'M') {
         OPCAO = args[1];
         cmd_frequencia(OPCAO);
      } else if (opcao == 'e') 
         { pacotes_externos(); }

      #ifdef __debug__
      variaveis_externas();
      #endif

      result = getopt(total, args, CURTAS);
   }
}

static OrdemDirEnt str_to_ode(const char* In)
{
/* Converte uma string da ordem, seja a forma que ela for(apesar que tem que
 * ser escrita de forma correta) na respectiva 'OrdemDirEnt'. */
   int t = strlen(In);
   char Out[2 * t];
   char* In_a = minuscula_ascii((char*)In);
   bool ordem_criacao, ordem_acesso, ordem_aleatoria, ordem_sistema,
        ordem_alfabetica, ordem_funcional;

   /* Faz a cópia da string minúscula, então libera a memória alocada no 
    * processo da transformação. */
   strcpy(Out, In_a);
   free(In_a);
   
   ordem_alfabetica = (
      strcmp(Out, "alfabética") == 0 || strcmp(Out, "alfabetica") == 0 ||
      strcmp(Out, "alfabético") == 0 || strcmp(Out, "alfabetico") == 0
   );
   ordem_aleatoria = (
      strcmp(Out, "aleatória") == 0 || strcmp(Out, "aleatoria") == 0 ||
      strcmp(Out, "aleatório") == 0 || strcmp(Out, "aleatorio") == 0 
   );
   ordem_sistema = (strcmp(Out, "sistema") == 0);
   ordem_acesso = (strcmp(Out, "acesso") == 0);
   ordem_criacao = (
      strcmp(Out, "criação") == 0 || 
      strcmp(Out, "criacão") == 0 || 
      strcmp(Out, "criaçao") == 0 || 
      strcmp(Out, "criacao") == 0
   );
   ordem_funcional = (strcmp(Out, "funcional") == 0);

   if (ordem_alfabetica)
      return Alfabetica;
   else if (ordem_sistema)
      return Sistema;
   else if (ordem_aleatoria)
      return Aleatoria;
   else if (ordem_acesso)
      return Acesso;
   else if (ordem_criacao)
      return Criacao;
   else if (ordem_funcional)
      return Funcional;
   else
      return Nenhuma;
}

extern void menu_interface_do_programa_i(char* args[], int total)
{
/* Um parser de menu codificado de forma bem mais simples, e com a mesma
 * agilidade do que o antigo. Aqui também aceita-se a forma longa do comando,
 * o que o antigo não aceitava. 
 */
   const struct option LONGAS[] = {
      {"help", no_argument, NULL, 'h'}, 
      {"info-projeto", required_argument, NULL, 'i'}, 
      {"path-var", no_argument, NULL, 'P'}, 
      {"repositorio-links", optional_argument, NULL, 'L'}, 
      {"cmd-mais-frequentes", no_argument, NULL, 'M'}, 
      {"cmd-frequencia", no_argument, NULL, 't'}, 
      {"rust-cache", no_argument, NULL, 'e'}
   };
   const char* const CURTAS = "hi:PL::Mte";
   const int Falhou = -1;
   int indice, result = getopt_long(total, args, CURTAS, LONGAS, &indice);

   if (result == Falhou) return;

   #ifdef __debug__
   printf("Índice: %d\n", indice);
   #endif

   switch((char)result)
   {
      case '?':
         puts("Opção da não existe!");
         break;
      case 'h':
         folha_de_ajuda();
         break;
      case 'P':
         // mostra_conteudo_da_variavel_path();
         mostra_conteudo_da_variavel_path_organizado();
         break;
      case 'e':
         #ifdef __debug__
         puts("Por motivos de precaução, evita rodar este comando em DEBUG.");
         #else
         pacotes_externos(); 
         #endif
         break;
      case 'i':
         DirInfo agregado = processa_projeto(optarg);
         visualiza_diretorio_info(&agregado);
         break;
      case 'L':
         OrdemDirEnt estado;
         const char* ordemstr;

         if (optarg != NULL)
         {
            estado = str_to_ode(optarg);
            ordemstr = ordemdirent_to_str(estado);
            printf("Você alternou prá ordem '%s'.\n", ordemstr);
            salva_ode(estado);
         }
         info_sobre_repositorio_de_linques_ordenada();
         break;
      case 't':
         cmd_frequencia("-t");
         break;
      case 'M':
         cmd_frequencia("-M");
         break;
      default:
         perror("Algum erro não previsto!");
         abort();
   }
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                      Funções Auxiliares
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */ 
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
   char argumento[UCHAR_MAX] = {0x00};

   if (algumas_da_opcoes_invalida) {
      perror("'longa' e 'descrição' tem que ter algo válido.");
      abort();
   }
         
   if ((*obj).metavar == NULL) 
      strcpy(argumento, VAZIA);
   else {
      strcat(argumento, " <");
      strcat(argumento, (*obj).metavar);
      strcat(argumento, ">");
   }

   if ((*obj).curta == 0x0)
      printf( "%s--%s%s\t%s.\n", RECUO, obj->longa, argumento, obj->descricao);
   else
      printf(
         "%s-%c --%s%s\n%s.\n\n", 
         RECUO, obj->curta, obj->longa, argumento, obj->descricao
      );
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
      "volvimento"
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
      "\t  Os linques dos repostiórios de linques de programas feitos, e se\n"
      "\tos linques ali, ainda são válidos. Quando passado o argumento,\n"
      "\tele não exibe apenas como é coletado do sistema arquivo, e sim\n"
      "\taplica uma ordenação ou algoritmos similares para ordernar a\n"
      "\texibição. A estado já fica registrado prá próxima chamada, se\n"
      "\tnão foi pedido outros tipo de 'modo' algum tipo de ordenação entre\n"
      "\tas seguintes opções: Nenhuma, Alfabético, Aleatória, Criação,\n"
      "\tAcesso, Funcional e Sistema.\n"
      "\t  Observe que ao digitar, você pode variar um pouco a escrita das\n"
      "\tmodos listados, seu 'case' ou mesmo as acentuações, o programa\n"
      "\tprovavelmente adivinhará sua escolha, nem todos modos, porém a\n"
      "\tmaioria das escritas são pegas", "ORDENACAO"
      },
      (struct Opcao) {
         't', "todos-comandos",
         "\t\tTodo histórico de comandos já digitado, e sua respectiva\n"
         "\tfrequências", NULL
      },
      (struct Opcao) {
         'M', "comandos-mais-relevantes",
         "\tApenas os comandos com alguma relevância numérica", NULL
      },
      (struct Opcao) {
         'e', "rust-cache",
         "\t\tA o gerenciador de pacotes externos faz um bocado de downloads,\n"
         "\teste aqui lista e organiza-os", NULL
      }
   };
   const int size = sizeof(struct Opcao);
   const int M = sizeof(OPCOES) / size;

   mostrar_manual(PROG, SINOPSE, DESCRICAO, OPCOES, M);
   putchar('\n');
}


#ifdef __unit_tests__
/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                            Testes Unitários   
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */ 
 #include "teste.h"

// Quebra galho para transferir argumentos.
static char** ARGS; static int TOTAL;

TESTE breve_promo_do_menu(void);
TESTE descobrindo_forma_longa(void);
TESTE novo_menu_extendido(void);
TESTE conversao_em_enums(void);
TESTE como_esta_o_menu(void)
   { folha_de_ajuda(); }

int main(int total, char* argumentos[]) 
{
   // Transmitindo argumentos.
   TOTAL = total; ARGS = argumentos;

   executa_testes_b(
      true, 4,
         Unit(como_esta_o_menu, true),
         // Geralmente desativado, pois as entradas e avaliações são manuais.
         Unit(novo_menu_extendido, false),
         Unit(conversao_em_enums, false),
         Unit(breve_promo_do_menu, false),
         Unit(descobrindo_forma_longa, false)
   );
   return EXIT_SUCCESS;
}

TESTE conversao_em_enums(void)
{
   const char* inputs[] = {
      "alfabética", "ALFABÉTICA", "alfabetica", "Alfabética", "ALFABETICA",
      "Aleatório", "aleatorio", "Aleatório", "aleatório", "Sistema", 
      "sistema", "Acesso", "acesso",  "aleatorio", "Criação", "criacão", 
      "Criaçao", "CRIACAO", "Nenhuma", "NENHUMA", "nenhuma", "NeNhUmA"
   };
   const int N = sizeof(inputs) / sizeof(char*);
   int n; OrdemDirEnt ordem; char* ordemstr;

   for (n = 0; n < N; n++) {
      ordem = str_to_ode(inputs[n]);
      ordemstr = ordemdirent_to_str(ordem);
      printf("%s ==> %s(%d)\n", inputs[n], ordemstr, ordem);
   }
}

TESTE breve_promo_do_menu(void)
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

}

TESTE descobrindo_forma_longa(void)
{
   char** argumentos = ARGS;
   int total = TOTAL, flag;

   const struct option long_options[] = {
      (struct option){
         .name="banana", 
         .has_arg=no_argument, 
         .flag=&flag,
         .val='b'
      },
      (struct option){ "morango", no_argument, NULL, 'm' },
      (struct option){ "pera", no_argument, 0, 'p' },
      (struct option) { "ecrã", no_argument, NULL, 'e'},
      (struct option) { "laranja", optional_argument, 0, 'L'}
   };
   const char short_options[] = "bmpeL::";

   int index;
   int result = getopt_long(
      total, argumentos, short_options, 
      long_options, &index
   );

   printf("Size of 'longopts': %d bytes\n", sizeof(long_options));
   printf("Result: %c\n", (char)result);
   printf("Índice: %d\n", index);
   printf("Flag: %d\n", flag);
   printf("Opção: '%s'\n", long_options[index]);

   printf("OptErr: %s\n", strerror(opterr));
   printf("OptArg: %s\n", optarg);
   printf("OptInd: %d\n", optind);

   puts("\nOutro disparo...");
   result = getopt_long(
      total, argumentos, short_options, 
      long_options, &index
   );

   printf("Size of 'longopts': %d bytes\n", sizeof(long_options));
   printf("Result: %c\n", (char)result);
   printf("Índice: %d\n", index);
   printf("Flag: %d\n", flag);
   printf("Opção: '%s'\n", long_options[index]);

   printf("OptErr: %s\n", strerror(opterr));
   printf("OptArg: %s\n", optarg);
   printf("OptInd: %d\n", optind);
}

TESTE novo_menu_extendido(void)
{
   char** argumentos = ARGS;
   int total = TOTAL, flag;

   menu_interface_do_programa_i(ARGS, TOTAL);
}

#endif
