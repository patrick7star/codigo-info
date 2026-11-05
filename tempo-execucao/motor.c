#include "motor.h"
// Biblioteca padrão do C:
#include <stdio.h>
// API do sistema Windows:
#include <windows.h>
#include <winbase.h>

const int MAX = 4 * CHAR_MAX;

void debug_uc(UptimeConteudo* obj) 
{ 
   printf(
      "UptimeConteudo(%0.2lf, %0.2lf)\n", 
      (*obj).uptime, (*obj).idle
   ); 
}

static char* caminho_uptime_no_wsl_server(void) {
   const char sz = sizeof(char);
   char* caminho = malloc(MAX * sz);

   GetEnvironmentVariable("FedoraWSL", caminho, MAX);
   sprintf(caminho, "%s/proc/uptime", caminho);

   return caminho;
}

UptimeConteudo tempo_wsl_ativado(void) {
   FILE* arquivo = NULL;
   const char sz = sizeof(char);
   char* caminho = caminho_uptime_no_wsl_server();
   UptimeConteudo output;
   char conteudo[MAX], auxiliar[MAX];
   char* ptr; ptrdiff_t length;
   
   // Lendo conteúdo do conteúdo se possível.
   fopen_s(&arquivo, caminho, "rt");
   fread(conteudo, sz, MAX, arquivo);
   fclose(arquivo);
   free(caminho);

   // A segunda parte(idle time), é mais fácil.
   ptr = strchr(conteudo, ' ');
   strcpy_s(auxiliar, MAX, ptr);
   output.idle = atof(auxiliar);   
   // Retira a primeira parte(uptime), então converte.
   length = ptr - conteudo;
   strncpy_s(auxiliar, MAX, conteudo, length);
   auxiliar[length] = '\0';
   output.uptime = atof(auxiliar);

   return output;
}

static double tempo_windows_ativado(void) 
   { return GetTickCount64() / 1000.0; }

void formata_um_horario(char* Output) {
   SYSTEMTIME Out;

   GetLocalTime(&Out);
   sprintf(
      Output, "\t\b\b%02d:%02d:%02d\n", 
      Out.wHour, Out.wMinute, Out.wSecond
   );
}

#ifdef _WIN32
 #ifdef __unit_tests__
#define TESTE static void
#include <processthreadsapi.h>
#include <locale.h>
#include <stdbool.h>

#define Okay   0
#define Fail   -1
#define SIZE_FMT 30

static char FMT[SIZE_FMT];

size_t filetime_to_u64(FILETIME x)
{ 
   ULARGE_INTEGER out = {
      .u = {
         x.dwLowDateTime, 
         x.dwHighDateTime 
      } 
   };

   return out.QuadPart;
}

const char* debug_filetime(FILETIME obj)
{
   size_t nanos = filetime_to_u64(obj);

   memset(FMT, 0x00, SIZE_FMT);
   sprintf(FMT, "(FILETIME) %zuns", nanos);

   return FMT;
}

TESTE estudando_funcao_get_system_time(void)
{
   FILETIME idle, kernel, user;
   
   if (GetSystemTimes(&idle, &kernel, &user))
      puts("Função chamada com sucesso.");
   else
      puts("A função falhou.");

   printf("idle: %s\n", debug_filetime(idle));
   printf("kernel: %s\n", debug_filetime(kernel));
   printf("user: %s\n", debug_filetime(user));

   size_t total_ns = {
      filetime_to_u64(idle) + 
      filetime_to_u64(user) + 
      filetime_to_u64(kernel) 
   };
   printf("Nanoseg: %zu\n", total_ns);
}

int main(int total, char* args[], char* vars[]) 
{
   setlocale (LC_CTYPE, "en_US.UTF8");

   estudando_funcao_get_system_time();
   return EXIT_SUCCESS;
}
 #endif
#endif
