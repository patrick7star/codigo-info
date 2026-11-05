// Biblioteca padrão do C:
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <limits.h>
// Módulos internos:
#include "motor.h"
// Módulos externos:
#include "legivel.h"

typedef const char* const LiteralStr;
LiteralStr TAB     = "   ";


void impressao_preto_e_branco(void) {
   char horario[CHAR_MAX];

   formata_um_horario(horario);
   printf(
      "\n%sWindows:%26s\n%sLinux(WSL):%24s\n%sHorário:%30s\n",
      TAB, tempo_legivel(tempo_windows_ativado()),
      TAB, tempo_legivel(tempo_wsl_ativado().uptime),
      TAB, horario
   );
}

void impressao_colorida(void) 
{
   LiteralStr AZUL    = "\033[0;96m";
   LiteralStr VERDE   = "\033[0;92m";
   LiteralStr AMARELO = "\033[0;93m";
   LiteralStr OFF     = "\033[0m";
   char horario[CHAR_MAX];

   formata_um_horario(horario);
   printf(
      "\n%s%sWindows:%s %26s"
      "\n%s%sLinux(WSL):%s %24s"
      "\n%s%sHorário:%s %30s\n",
      TAB, AZUL, OFF, tempo_legivel(tempo_windows_ativado()), 
      TAB, VERDE, OFF, tempo_legivel(tempo_wsl_ativado().uptime),
      TAB, AMARELO, OFF, horario
   );
}

bool sem_cor(int qtd, char* lista[])
{
/* Verifica se um argumento que pede a impressão sem cor foi dado. Caso 
 * sim, o retorno é o valor lógico 'verdadeiro', caso contrário, será o
 * oposto('falso'). */
   LiteralStr MATCH = "--sem-cor";

   for (int n = 1; n <= qtd; n++)
   {
      if (strstr(lista[n - 1], MATCH) != NULL)
         return true;
   }
   return false;
}

int main(int total, char* args[], char* vars[]) 
{
   // ativando Unicode no programa ...
   setlocale (LC_CTYPE, "en_US.UTF8");

   if (sem_cor(total, args))
      impressao_preto_e_branco();
   else
      impressao_colorida(); 

   return EXIT_SUCCESS;
}
