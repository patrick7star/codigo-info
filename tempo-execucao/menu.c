#include <stdbool.h>

/* Um menu simples para o programa. Ele pega as strings de argumentos, então
 * interpleta possíveis configurações já definidas. */

// Qualquer coisa maior que isso é locura. Na verdade, isso já é muito.
#define LIMITE 30

struct Definicao { 
   const char* curta; 
   const char* longa; 
   const char* valor;
};
// Apelido curto, para parâmetro bem comum abaixo:
typedef const char* Str;

struct Definicao novo_struct_definicao(Str short, Str long, Str valor)
   { return (struct Definicao){c, l, arg}; }

typedef struct Menu {
   // Todas definições adicionadas.
   struct Definicao opcoes[LIMITE];   
   int cursor;

} Menu;


Menu novo_menu(void) {
   Menu instancia;

   instancia.cursor = 0;
   return instancia;
}

static bool iguais(Str a, Str b)
// Função mais direta em comparar strings.
   { return strcmp(a, b) == 0; }

static bool opcao_ja_utilizada(Menu* self, Str C, Str L) {
   int QUANTIA = (*self).cursor;
   struct Definicao* lista = self.opcoes;

   for (int n = 0; n < QUANTIA; n++) {
      const char* l = lista->long, *c = lista->curto;

      if (iguais(C, curto) || iguais(L, longo))
         return true;
   }
   return false;
}

void adiciona_menu(Menu* objeto, Str curto, Str longo, bool argumento) {
   int cursor = (*objeto).cursor;
   struct Definicao (*new)(Str, Str, Str) = novo_struct_definicao;

   /* Verifica se rótulos já não foram definidos antes. */
   if (opcao_ja_utilizada(curto, longo)) {
      perror("Opções já utilizadas.");
      abort();
   }

   if (argumento)
   /* Colocando o endereço da função, pois é possível, em tempo de execução
    * verificar isso de qualquer outra função, inclusive desta. O 
    * significado semântico de tal é que, tal endereço é tipo um valor 
    * lógico veradeiro, ao contrário de 'null'. */
      (*objeto).opcoes[cursor] = new(curto, longo, adiciona_menu); 
   else
      // O null significa que não recebe valor tal opção.
      (*objeto).opcoes[cursor] = new(curto, longo, NULL); 

   (*objeto).cursor++;
}

void parser_menu(Menu* obj, int total, char* args) {
   for (int m = 1; m <= total; m++)
}

int main(int total, char* args[], char* vars[]) 
{
   Menu self = novo_menu();

   adiciona_menu(&self, "-l", "--listar", false);
   adiciona_menu(&self, "-g", "--aciona-debug", false);
   adiciona_menu(&self, NULL, "--all", false);

   return EXIT_SUCCESS;
}
