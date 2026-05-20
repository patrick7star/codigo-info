/* Verifica várias coisas relacionadas a linques no sistema. Eu crio um
 * repositório(um diretório), ligado há uma variável 'LINKS/ou LINQUES'. Lá,
 * coloco todos programas uteis que chamo por terminal, ou aplicações,
 * especialmente CLI, que produzo.
 */

#include "linque.h"
// Biblioteca padrão do C:
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
// API do sistema:
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
// Bibliiotecas externas:
#include "impressao.h"
#include "conjunto_ref.h"

// Caractéres Unicode de representão itens a serem listados.
const wchar_t VALIDO   = L'\U0001f7e2';
const wchar_t INVALIDO = L'\u2b55';
const char* PATH_ORDEMDE = "/home/dee_dee_dexter/programas/codigo-info/data/ordem-de.dat";

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
#if defined(__linux__) && defined(__unit_tests__)
static void struct_dirent_debug(struct dirent* obj);
static const char* dtype_str(uint32_t code);
#endif
static bool lincado_a_algo(char* caminho);
static char* junta_caminhos(char* path_a, char* path_b);
static bool pular_entradas_desnecessarias(struct dirent* obj);
static void status_dos_linques_avaliados(int validos, int invalidos);
static bool um_caminho_no_windows(const char* caminho);

enum os_check { Failed = -1, Okay };

typedef struct {
   // Clone do ponteiro atual.
   struct dirent* lista;
   int quantia;
   OrdemDirEnt ordem;

} ListaDirEnt, LDE;
// Apelidos de uma função que retorna 'void' e recebe uma 'ListaDirEnt'.
typedef void (*SelecaoAlg)(ListaDirEnt);
// Apelidos prá alinhar as declarações abaixo:
typedef SelecaoAlg SAlg;
typedef OrdemDirEnt ODEnt;
typedef const char* CStr;
typedef struct dirent sDE;

static void  status_dos_linques_avaliados_i  (int v, int i, ODEnt o);
static LDE   entradas_do_repositorio_linques (void);
static void  free_lde                        (LDE obj);
static void  ordena_entradas                 (LDE obj);
static void  desordena_entradas              (LDE obj);
static void  alterna                         (sDE * l, int p, int q);
static void  agrupa_entradas                 (LDE obj);
static char* repositorio                     (void);
static SAlg  selecao_do_algoritmo            (ODEnt);
static ODEnt carrega_ode                     (void);
// Funções auxiliares de remendo de código.
static void realiza_nada(LDE obj) { (void)obj; }
static void pass(void) {}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
const char* ordemdirent_to_str(OrdemDirEnt tipo_de_ordem)
{
   switch(tipo_de_ordem)
   {
      case Nenhuma:
         return "Nenhuma";
      case Sistema:
         return "Sistema";
      case Alfabetica:
         return "Alfabética";
      case Criacao:
         return "Criação";
      case Acesso:
         return "Acesso";
      case Aleatoria:
         return "Aleatória";
      default:
         perror("Não é possível alcançar aqui!");
         exit(EXIT_FAILURE);
   }
}

void salva_ode(OrdemDirEnt tipo)
{
   FILE* stream = NULL;
   const int size = sizeof(int);
   int result;

   stream = fopen(PATH_ORDEMDE, "wb");
   result = fwrite(&tipo, size, 1, stream);

   if (result*size == size)
      #ifdef __debug__
      puts("Escrito com sucesso.");
      #else
      pass();
      #endif
   else
      #ifdef __debug__
      printf("Escreveu %d bytes apenas!\n", result * size);
      #else
      pass();
      #endif

   if (fclose(stream) == Failed)
      #ifdef __debug__
      perror(strerror(errno));
      #else
      pass();
      #endif
}
void info_sobre_repositorio_de_linques(void) {
/* Lista todas entradas do diretório que representa o repositório de linques,
 * se houver algum, ou a variável que direciona ela estiver bem definida.
 */
   const char* caminho = repositorio();

   if (caminho == NULL) {
      perror("Variável não definida!");
      abort();
   }

   DIR* iteracao = opendir(caminho);
   struct dirent* atual;
   char* nome, *aux, *base = (char*)caminho;
   wchar_t status;
   int validos = 0, invalidos = 0;

   // Deixa um espaço de uma linha da listagem de itens.
   putchar('\n');

   for (atual=readdir(iteracao); atual != NULL; atual=readdir(iteracao))
   {
      if (pular_entradas_desnecessarias(atual))
         continue;

      nome = (char*)(*atual).d_name;
      aux = junta_caminhos(base, nome);

      if (lincado_a_algo(aux)) {
         status = VALIDO;
         validos++;

      } else {
         status = INVALIDO;
         invalidos++;
      }

      if (um_caminho_no_windows(aux))
         printf("\t\b\b%lc %-18s\t(Windows)\n", status, nome);
      else
         printf("\t\b\b%lc %s\n", status, nome);
      free(aux);
   }
   status_dos_linques_avaliados(validos, invalidos);
}

void info_sobre_repositorio_de_linques_ordenada(void) {
/* Lista todas entradas do diretório que representa o repositório de linques,
 * se houver algum, ou a variável que direciona ela estiver bem definida.
 * Entretanto, está leva tanto a ordenação total, como a ausência dela,
 * portanto como foi inicialmente iterada. Por contar com a função de
 * 'entradas_do_repositorio_linques', o uso é duas vezes maior que a função
 * anterior.
 */
   ListaDirEnt entries = entradas_do_repositorio_linques();
   int posicao, validos = 0, invalidos = 0;
   const int FINAL = entries.quantia - 1;
   char* caminho = repositorio();
   struct dirent* atual = NULL;
   wchar_t status = L'\0';
   char* nome = NULL, *aux = NULL, *base = caminho;
   SelecaoAlg ordenacao;
   OrdemDirEnt ordem;

   // Obtendo atual tipo de ordenação configurado.
   ordem = carrega_ode();
   #ifdef __debug__
   const char* ordemstr = ordemdirent_to_str(ordem);
   printf("Ordem Carregada: '%s'\n", ordemstr);
   #endif
   entries.ordem = ordem;
   // Seleção baseado na ordem, então a aplicação do algoritmo.
   ordenacao = selecao_do_algoritmo(ordem);
   ordenacao(entries);
   // Deixa um espaço de uma linha da listagem de itens.
   putchar('\n');

   for (posicao = 0; posicao < FINAL; posicao++)
   {
      atual = &entries.lista[posicao];
      nome = (char*)(*atual).d_name;
      aux = junta_caminhos(base, nome);

      if (lincado_a_algo(aux)) {
         status = VALIDO;
         validos++;

      } else {
         status = INVALIDO;
         invalidos++;
      }

      if (um_caminho_no_windows(aux))
         printf("\t\b\b%lc %-18s\t(Windows)\n", status, nome);
      else
         printf("\t\b\b%lc %s\n", status, nome);
      free(aux);
   }
   status_dos_linques_avaliados_i(validos, invalidos, entries.ordem);
   free_lde(entries);
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *             Funções Auxiliares(declaração da interface privada)
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
static char* repositorio(void)
   { return getenv("LINKS"); }

static bool a_maior_b_alfabetica(struct dirent a, struct dirent b)
   { return strcmp(a.d_name, b.d_name) < 0; }

static bool a_maior_b_acesso(struct dirent a, struct dirent b)
{
   const int M = 300;
   char path_a[M], path_b[M];
   const char* REPO = repositorio();
   struct stat info_a, info_b;

   sprintf(path_a, "%s/%s", REPO, a.d_name);
   sprintf(path_b, "%s/%s", REPO, b.d_name);
   lstat(path_a, &info_a);
   lstat(path_b, &info_b);

   return info_a.st_atime > info_b.st_atime;
}

static bool a_maior_b_criacao(struct dirent a, struct dirent b)
{
   const int M = 300;
   char path_a[M], path_b[M];
   const char* REPO = repositorio();
   struct stat info_a, info_b;

   sprintf(path_a, "%s/%s", REPO, a.d_name);
   sprintf(path_b, "%s/%s", REPO, b.d_name);
   lstat(path_a, &info_a);
   lstat(path_b, &info_b);

   return info_a.st_ctime > info_b.st_ctime;
}

static void ordena_entradas(ListaDirEnt obj)
{
   bool (*comparacao)(struct dirent, struct dirent);

   // Escolhe a comparação a ser feita baseado na ordem configurada.
   switch (obj.ordem) {
      case Alfabetica:
         comparacao = a_maior_b_alfabetica;
         break;
      case Acesso:
         comparacao = a_maior_b_acesso;
         break;
      case Criacao:
         comparacao = a_maior_b_criacao;
         break;
      default:
         perror("Não codificado para tal escolha!");
         abort();
   };

   const int N = obj.quantia;
   struct dirent auxiliar, *array = obj.lista;
   int i, j, maior;

   // Algoritimo de ordenação(select sort como você pode ver).
   for (i = 0; i < (N - 1); i++)
   {
      maior = i;
      for (j = (i + 1); j < (N - 1); j++)
      {
         if (comparacao(array[j], array[maior]))
            maior = j;
      }

      // Realização do swap.
      auxiliar = array[maior];
      array[maior] = array[i];
      array[i] = auxiliar;
   }
}

static uint32_t semente_aleatoria(void)
{
   const char* const CAMINHO = "/dev/urandom";
   int device = open(CAMINHO, O_NONBLOCK);
   uint32_t buffer = 0x00000000;

   if (device == Failed)
      perror(strerror(errno));
   else
      puts("Aberto pipe com sucesso.");

   if (read(device, &buffer, sizeof(uint32_t)) == sizeof(uint32_t))
      puts("Lido com sucesso.");
   else
      perror(strerror(errno));

   return buffer;
}

static void desordena_entradas(ListaDirEnt obj)
{
/* O algoritmo embaralha das entradas na lista interna do objeto.
 * O algoritmo com que ele faz isso é bem simples. Ele apenas faz metade
 * das iterações, e faz uma sorteio de alguma posição, então troca a sequência
 * inicial até a metade pelos valores sorteados.
 */
   const int CICLOS = obj.quantia / 2;
   int n, selecao, antiga;
   struct dirent* lista = obj.lista;

   srand(semente_aleatoria());

   for (selecao = 0, n = 1; n <= CICLOS; n++)
   {
      antiga = selecao;
      selecao = rand() % obj.quantia;

      if (selecao == antiga)
         continue;

      /*/
      auxiliar = lista[n - 1];
      lista[n - 1] = lista[selecao];
      lista[selecao] = auxiliar; */
      alterna(lista, (n - 1), selecao);
   }
}

static void alterna(struct dirent* lista, int p, int q)
{
   struct dirent auxilar;

   auxilar = lista[p];
   lista[p] = lista[q];
   lista[q] = auxilar;
}


static sDE* alloc_sd(sDE* X)
{
   const int sz = sizeof(struct dirent);
   struct dirent* novo = NULL;

   novo = malloc(sz);

   if (novo == NULL) {
      perror(strerror(errno));
      abort();
   }

   *novo = *X;
   return novo;
}

static void dealloc_sd(sDE* X)
   { free(X); }

static size_t hash_sd(GenT a, size_t C)
{
   sDE* obj = (sDE*)a;
   int64_t inode = (*obj).d_ino;

   return hash_i64(&inode, C);
}

static bool eq_sd(GenT a, GenT b)
{
   char* str_a, *str_b;
   struct dirent* obj_a, *obj_b;

   obj_a = (struct dirent*)a;
   obj_b = (struct dirent*)b;
   str_a = (*obj_a).d_name;
   str_b = (*obj_b).d_name;

   return strcmp(str_a, str_b) == 0;
}

static void agrupa_entradas(ListaDirEnt obj)
{
/* Nota: algoritmo não foi testado com poucas entradas.*/
   char* REPO = (char*)repositorio();
   const int N = obj.quantia;
   struct dirent* lista = obj.lista;
   char* nome = NULL, *caminho = NULL;
   Set saco_lnx = cria_set(hash_sd, eq_sd);
   Set saco_win = cria_set(hash_sd, eq_sd);
   GenT clone = NULL; int n;
   /* Separa os caminhos em dois grandes conjuntos, então insere da array
    * de forma ordenada. */
   for (n = 0; n < N; n++)
   {
      nome = lista[n].d_name;
      caminho = junta_caminhos(REPO, nome);
      clone = (void*)alloc_sd(&lista[n]);

      if (um_caminho_no_windows(caminho))
         add_set(saco_win, clone);
      else
         add_set(saco_lnx, clone);

      free(caminho);
   }

   /* Despejando coisas coletadas dentro da array. Primeiro a parte do Linux,
    * então depois a parte do Windows. Se for de preferência a inversão da 
    * ordem, é preciso apenas inverter os laços-whiles abaixo.
    */
   n = 0;

   while (!empty_set(saco_lnx))
   {
      clone = deleta_set(saco_lnx);
      lista[n++] = *((sDE*)clone); 
      dealloc_sd(clone);

   }
   while (!empty_set(saco_win)) 
   {
      clone = deleta_set(saco_win);
      lista[n++] = *((sDE*)clone); 
      dealloc_sd(clone);
   }

   // Despejando na array ambos conteúdos dos conjuntos...
   destroi_set(saco_win);
   destroi_set(saco_lnx);
}

static ListaDirEnt entradas_do_repositorio_linques(void)
{
/* É uma função que faz um clone da iteração, e entrega apenas uma lista
 * dos 'dirent' com o seu tamanho. Fica a cargo do chamador desalocar a
 * alocação realizada. */
   const char* caminho = repositorio();

   if (caminho == NULL) {
      perror("Variável não definida!");
      abort();
   }

   DIR* iteracao = opendir(caminho);
   struct dirent* atual = NULL;
   /* Ainda sem alguma lista alocada que foi apontada, o valor começar em
    * zero, assim já utiliza tal variável como contagem. A ordem inicial é
    * a que foi iterado, 'nenhuma' no caso representa isso. */
   ListaDirEnt output = { NULL, 0, Nenhuma };
   const int size = sizeof(struct dirent);
   int tamanho, p = 0;

   // Contando primeiramente, depois aloca o necessário baseado nisso, então
   // começar a preencher a lista.
   for (atual=readdir(iteracao); atual != NULL; atual=readdir(iteracao))
   {
      if (pular_entradas_desnecessarias(atual))
         continue;

      output.quantia += 1;
   }

   if (closedir(iteracao) == Failed) {
      perror(strerror(errno));
      exit(EXIT_FAILURE);
   }

   tamanho = size * output.quantia;
   output.lista = (struct dirent*)malloc(tamanho);
   iteracao = opendir(caminho);

   for (atual=readdir(iteracao); atual != NULL; atual=readdir(iteracao))
   {
      if (pular_entradas_desnecessarias(atual))
         continue;

      output.lista[p++] = *atual;
   }

   if (closedir(iteracao) == Failed) {
      perror(strerror(errno));
      exit(EXIT_FAILURE);
   }

   return output;
}

static void free_lde(ListaDirEnt obj)
// Desaloca a lista do tipo 'ListaDirEnt' que é clonada da iteração original.
   { free(obj.lista); }

static bool um_caminho_no_windows(const char* caminho)
{
/* Verifica se o caminho do linque passado rotea para uma aplicação armazenada
 * no sistema do Windows. */
   const char* const MATCH = "/mnt/c/";
   int comprimento = strlen(caminho);
   char buffer[comprimento];

   readlink(caminho, buffer, comprimento);
   return (strstr(buffer, MATCH) != NULL);
}

#if defined(__linux__) && defined(__unit_tests__)
static void struct_dirent_debug(struct dirent* obj) {
   char* nome = (*obj).d_name;
   uint32_t tipo = (*obj).d_type;
   int fd = (*obj).d_fileno;
   uint16_t tamanho = (*obj).d_reclen;

   printf(
      "[%-9s | %7d | %d bytes] %s\n",
      dtype_str(tipo), fd, tamanho, nome
   );
}

static const char* dtype_str(uint32_t code) {
   if (code == DT_BLK)
      return "partição";
   else if (code == DT_CHR)
      return "caractére";
   else if (code == DT_DIR)
      return "diretório";
   else if (code == DT_FIFO)
      return "named pipe";
   else if (code == DT_REG)
      return "arquivo";
   else if (code == DT_LNK)
      return "linque";
   else if (code == DT_SOCK)
      return "socket";
   else
      return "desconhecido";
   // Nota: por algum motivo, tal comparação não funciona com 'switch'.
}
#endif

static bool lincado_a_algo(char* caminho) {
/* Existe uma suposição de que, realmente o caminho passado, se refere
 * realmente a um linque. E o arquivo/diretório referenciado, se ele existe.
 */
   const int MAX = 2 * UCHAR_MAX + 1;
   char conteudo[MAX];
   struct stat info;
   ssize_t result;

   // Preenchendo com caractéres nulos para anular qualquer ruído.
   memset(conteudo, 0x00, MAX);
   result = readlink(caminho, conteudo, MAX);

   if (result > 0 && result < MAX) {
      /* Agora verifica se o real 'inode' que o linque liga, é realmente
       * válido, ou seja, está lá(existe). */
      if (stat(conteudo, &info) == 0)
         return true;
      else {
         if (errno == ENOENT)
            return false;
         else{
            perror(strerror(errno));
            abort();
         }
      }
   } else if (result == MAX) {
      perror("O caminho foi truncado, é preciso aumentar a capacidade.");
      abort();
   } else {
      perror(strerror(errno));
      abort();
   }
}

static char* junta_caminhos(char* path_a, char* path_b) {
/* Concatena dois caminhos, na ordem dos parâmetros listados. Ele aloca
 * memória na heap, portanto é preciso que o 'caller' libere posteriormente.
 */
   const int GARANTIA = 5;
   /* Coloco tal garantia, pois às vezes há um problem com buffer overflow.
    * Mais tarde, eu reviso o problema, então computo o acrescimo faltante
    * certo. */
   int length = strlen(path_b) + strlen(path_a) + GARANTIA;
   const int sz = sizeof(char);
   char* output = calloc(length, sz);

   strcpy(output, path_a);
   strcat(output, "/");
   strcat(output, path_b);

   return output;
}

static bool pular_entradas_desnecessarias(struct dirent* obj) {
// Atalhos do atual diretório, e diretório anterior(pai), são desnecessários.
   const char* const CWD = ".";
   const char* const PARENT = "..";
   char* const nome = obj->d_name;

   return (strcmp(nome, CWD) == 0 || strcmp(nome, PARENT) == 0);
}

static void status_dos_linques_avaliados(int validos, int invalidos) {
   int total = validos + invalidos;
   const int N = 150;
   char validos_str[N], invalidos_str[N];
   char* ptr_a = validos_str, *ptr_b = invalidos_str;
   char total_str[N];

   memset(validos_str, '\0', N);
   memset(invalidos_str, '\0', N);
   sprintf(validos_str, "%2d", validos);
   sprintf(invalidos_str, "%2d", invalidos);
   sprintf(total_str, "%d", total);

   ptr_a = colori_string_ii(ptr_a, Verde);
   ptr_b = colori_string_ii(ptr_b, Vermelho);
   aplica_formatacao_ii(total_str, AzulMarinho, Sublinhado);

   printf(
      "\nForam contados %s linques; Balança geral: %s |%s.\n\n",
      total_str, validos_str, invalidos_str
   );
}

static void status_dos_linques_avaliados_i
  (int validos, int invalidos, OrdemDirEnt ordem)
{
   int total = validos + invalidos;
   const int N = 150;
   char validos_str[N], invalidos_str[N], *ordem_str = NULL;
   char total_str[N], *ptr_a = validos_str, *ptr_b = invalidos_str;
   const char* ordemstr = ordemdirent_to_str(ordem);

   memset(validos_str, '\0', N);
   memset(invalidos_str, '\0', N);
   sprintf(validos_str, "%2d", validos);
   sprintf(invalidos_str, "%2d", invalidos);
   sprintf(total_str, "%d", total);

   ptr_a = colori_string_ii(ptr_a, Verde);
   ptr_b = colori_string_ii(ptr_b, Vermelho);
   aplica_formatacao_ii(total_str, AzulMarinho, Sublinhado);
   ordem_str = colori_string((char*)ordemstr, Amarelo);

   printf(
      "\nForam contados %s linques; Balança geral: %s |%s; Ordenação[%s].\n\n",
      total_str, validos_str, invalidos_str, ordem_str
   );
   free(ordem_str);
}

static SelecaoAlg selecao_do_algoritmo(OrdemDirEnt tipo_de_ordem)
{
   // Seleção baseado na ordem, então a aplicação do algoritmo.
   switch (tipo_de_ordem)
   {
      case Aleatoria:
         return desordena_entradas;
      case Nenhuma:
         return realiza_nada;
      case Sistema:
         return agrupa_entradas;
      case Acesso:
         return ordena_entradas;
      case Alfabetica:
      case Criacao:
         return ordena_entradas;
      default:
         perror("Não existe tal opção na codificação.");
         abort();
   };
}

static OrdemDirEnt carrega_ode(void)
{
   OrdemDirEnt buffer;
   const int size = sizeof(buffer);
   FILE* database = NULL;
   int result;

   #ifdef __debug__
   printf("OrdemDirEnt %d bytes\n", size);
   #endif
   database = fopen(PATH_ORDEMDE, "rb");

   if (database == NULL) {
      #ifdef __debug__
      perror(strerror(errno));
      #endif
      return Nenhuma;
   }

   result = fread(&buffer, size, 1, database);

   if (result * size == size)
      #ifdef __debug__
      puts("Leu tudo perfeitamente.");
      #else
      pass();
      #endif
   else
      return Nenhuma;
   return buffer;
}

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                       Testes Unitários
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
#if defined(__linux__) && defined(__unit_tests__)
#include "teste.h"
#include <locale.h>
#include <errno.h>
#include <time.h>
#include <math.h>

OrdemDirEnt random_ode(void);
TESTE nova_visualizacao_com_ordenacao(void);
TESTE simples_listagem_dos_linques(void);
TESTE funcao_que_clona_iteracao(void);
static void visualizacao_sd(int p, struct dirent* X);
TESTE visualizacao_de_todas_ordenacoes(void);
TESTE algoritmo_de_agrupamento(void);
TESTE byte_aleatorio(void);
TESTE sorteio_de_ordenacoes(void);
TESTE sorteio_e_registro_de_ordenacoes(void);
TESTE escolha_da_funcao_certa_pela_ordem(void);


int main(void) {
   setlocale(LC_ALL, "en_US.UTF-8");

   executa_testes_b(
      true, 10,
         Unit(escolha_da_funcao_certa_pela_ordem, false),
         Unit(sorteio_e_registro_de_ordenacoes, false),
         Unit(sorteio_de_ordenacoes, false),
         Unit(info_sobre_repositorio_de_linques, false),
         Unit(simples_listagem_dos_linques, false),
         Unit(funcao_que_clona_iteracao, false),
         Unit(visualizacao_de_todas_ordenacoes, false),
         Unit(algoritmo_de_agrupamento, true),
         Unit(byte_aleatorio, false),
         // Desativado, já que o 'output' é muito extenso.
         Unit(nova_visualizacao_com_ordenacao, false)
   );
   return EXIT_SUCCESS;
}

TESTE nova_visualizacao_com_ordenacao(void)
{
   salva_ode(Nenhuma);
   info_sobre_repositorio_de_linques_ordenada();

   salva_ode(Alfabetica);
   info_sobre_repositorio_de_linques_ordenada();

   salva_ode(Acesso);
   info_sobre_repositorio_de_linques_ordenada();

   salva_ode(Criacao);
   info_sobre_repositorio_de_linques_ordenada();

   salva_ode(Aleatoria);
   info_sobre_repositorio_de_linques_ordenada();

   salva_ode(Sistema);
   info_sobre_repositorio_de_linques_ordenada();
}


TESTE sorteio_e_registro_de_ordenacoes(void) {
   OrdemDirEnt novo, antigo;
   int repeticoes = 23;

   while (repeticoes-- > 0)
   {
      novo = random_ode();
      antigo = carrega_ode();
      salva_ode(novo);

      printf(
         "Novo[%d]: %s\tAntigo[%d]: %s\n",
         novo, ordemdirent_to_str(novo),
         antigo, ordemdirent_to_str(antigo)
      );
   }
}

TESTE escolha_da_funcao_certa_pela_ordem(void)
{
   OrdemDirEnt estado = carrega_ode();
   SelecaoAlg MATCH = ordena_entradas;
   SelecaoAlg escolha = selecao_do_algoritmo(estado);

   printf("Ordem Entradas: %p\n", (void*)(uintptr_t)MATCH);
   printf("Escolha: %p\n", (void*)(uintptr_t)escolha);

   for (int i = 1; i <= 5; i++)
   {
      long X = powl(2, i);
      const char* Y = ordemdirent_to_str(X);
      SelecaoAlg W = selecao_do_algoritmo(X);
      uintptr_t Z = (uintptr_t)W;
      printf("\t%-15s ===> %10p\n", Y, (void*)Z);
   }
}

TESTE sorteio_de_ordenacoes(void)
{
   for (int n = 1; n <= 45; n++) {
      if (n % 5 == 0)
         putchar('\n');
      printf("%-15s", ordemdirent_to_str(random_ode()));
   }
   putchar('\n');
}

OrdemDirEnt random_ode(void)
{
   static bool SEED_RANDOM_ODE = false;

   if (SEED_RANDOM_ODE)
      { SEED_RANDOM_ODE = true; srand(time(NULL)); }

   int X = (rand() % 5) + 1;

   return (OrdemDirEnt)powl(2, X);
}

TESTE simples_listagem_dos_linques(void)
{
   const char* caminho = repositorio();

   if (caminho == NULL) {
      perror("Variável não definida!");
      abort();
   }

   DIR* iteracao = opendir(caminho);
   struct dirent* atual = NULL;

   do {
      atual = readdir(iteracao);

      if (atual == NULL) {
         int code = errno;
         char* msg_erro = strerror(code);

         perror(msg_erro);
         break;
      }
      struct_dirent_debug(atual);

   } while (true);
}

TESTE funcao_que_clona_iteracao(void)
{
   ListaDirEnt out = entradas_do_repositorio_linques();
   int posicao;

   for (posicao = 0; posicao < out.quantia; posicao++)
   {
      if ((posicao + 1) % 4 == 0)
         putchar('\n');
      else
         printf("%dº) %s\t", posicao + 1, out.lista[posicao].d_name);
   }
   putchar('\n');
   free_lde(out);
}

void visualizacao_sd(int p, struct dirent* X)
{
   struct dirent Y = X[p];
   char* caminho = junta_caminhos(repositorio(), Y.d_name);

   if (um_caminho_no_windows(caminho))
      printf("%2dº) %-30s(WINDOWS)\n", p + 1, Y.d_name);
   else
      printf("%2dº) %s\n", p + 1, Y.d_name);
   free(caminho);
}

TESTE visualizacao_de_todas_ordenacoes(void)
{
   ListaDirEnt out = entradas_do_repositorio_linques();
   int posicao;

   out.ordem = Alfabetica;
   puts("\nOrdenação: 'Alfabética'");
   ordena_entradas(out);

   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   out.ordem = Criacao;
   puts("\nOrdenação: 'Tempo de Criação'");
   ordena_entradas(out);

   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   out.ordem = Acesso;
   puts("\nOrdenação: 'Último Acesso'");
   ordena_entradas(out);

   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   out.ordem = Aleatoria;
   puts("\nOrdenação: 'Aleatória'");
   desordena_entradas(out);

   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   out.ordem = Sistema;
   puts("\nOrdenação: 'Sistema'");
   desordena_entradas(out);

   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   putchar('\n');
   free_lde(out);
}

TESTE algoritmo_de_agrupamento(void)
{
   ListaDirEnt out = entradas_do_repositorio_linques();
   int posicao;

   puts("\nOrdenação: 'Sistema'");
   puts("\tAntes da ordenação...");
   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);
   // Algoritmo de agrupamento aplicado.
   agrupa_entradas(out);
   puts("\tDepois de ordenado.");
   for (posicao = 0; posicao < out.quantia; posicao++)
      visualizacao_sd(posicao, out.lista);

   putchar('\n');
   free_lde(out);
}

TESTE byte_aleatorio(void){
   const char* const CAMINHO = "/dev/urandom";
   int device = open(CAMINHO, O_NONBLOCK);
   uint32_t buffer = 0x00000000;

   if (device == Failed)
      perror(strerror(errno));
   else
      puts("Aberto pipe com sucesso.");

   if (read(device, &buffer, sizeof(uint32_t)) == sizeof(uint32_t))
      puts("Lido com sucesso.");
   else
      perror(strerror(errno));

   printf("Seleção: %d\n", buffer);
}
#endif
