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
// Bibliiotecas externas:
#include "impressao.h"

// Caractéres Unicode de representão itens a serem listados.
const wchar_t VALIDO   = L'\U0001f7e2';
const wchar_t INVALIDO = L'\u2b55';


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
      "\nForam contados %s linques; Balança geral: %s |%s.\n",
      total_str, validos_str, invalidos_str
   );
}

void info_sobre_repositorio_de_linques(void) {
/* Lista todas entradas do diretório que representa o repositório de linques,
 * se houver algum, ou a variável que direciona ela estiver bem definida.
 */
   const char* caminho = getenv("LINKS");

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

      printf("\t\b\b%lc %s\n", status, nome);
      free(aux);
   }
   status_dos_linques_avaliados(validos, invalidos);
}

#if defined(__linux__) && defined(__unit_tests__)
#include <locale.h>


void simples_listagem_dos_linques(void)
{
   const char* caminho = getenv("LINKS");

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

int main(void) {
   setlocale(LC_ALL, "en_US.UTF-8");

   info_sobre_repositorio_de_linques(); 
   return EXIT_SUCCESS;
}
#endif
