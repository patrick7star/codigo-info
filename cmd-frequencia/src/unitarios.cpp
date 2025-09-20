
#include <set>
#include <new>
#include <algorithm>
#include <stdint.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>
#define E_SUBPROCESSO 0

extern "C" void executa_testes (uint8_t total, ...);

void visualizacao_de_debug(void) {
   ifstream arquivo(CAMINHO, ios::in);
   auto frequencias = computa_frequencia(arquivo); 
   arquivo.close();
   visualizaca_mapa(frequencias);
}


void visualiza_trechos_da_saida_gigante(string& obj) {
   size_t Q = obj.size();
   cout << "Total de caractéres: " << Q << "\tCapacidade: "
      << obj.capacity() << endl;
}


static string executa_comando_output(const char* cmd) {
   FILE* comando_output = popen(cmd, "r");
   const size_t T = 50, size = sizeof(char);
   string conteudo;
   char buffer[T], *ptr_buffer;
   size_t lido = 0;


   assert (comando_output != nullptr);
   fstream saida(comando_output, ios::in);
   ptr_buffer = buffer;
   for (size_t i = 1; i <= 100; i++) {
      saida.getline(ptr_buffer, T);
      lido += 50;
      conteudo.append(buffer);
   }
   saida.close();
   pclose(comando_output);
   
   #ifdef _CARREGA_CMDS
   cout << "Tem que ser lido/Realmente lido: " << T * size <<
      '/' << lido << endl;
   #endif
   assert (lido == 100 * size);

   return conteudo;
}

static set<string> carrega_todos_comandos(void) {
   set<string> TODOS_CMDS{};

   if (TODOS_CMDS.size() > 0)
      { return TODOS_CMDS; }

   const char comando[] = "/bin/bash -c 'compgen -c | sort | head -n 50'";
   auto content = executa_comando_output(comando);
   visualiza_trechos_da_saida_gigante(content);

   return TODOS_CMDS;
}

void e_um_comando(string& cmd) {
   sleep(2);
   cout << "O comando '" << cmd << "' foi executado." << endl;
   exit(0);
}

void verificacao_da_validade_dos_comandos(void) {
   ifstream arquivo(CAMINHO, ios::in);
   auto frequencias = computa_frequencia(arquivo); 
   arquivo.close();

   auto iterador = frequencias.begin();
   do {
      if (fork() == E_SUBPROCESSO) {
         string cmd = iterador->first;
         e_um_comando(cmd);
      }

      sleep(1); // Pausa na interaçã de cada.
      iterador++;
   } while(iterador != frequencias.end());
}

void processo_de_carregamento_de_cmds(void) {
   set<string> resultado = carrega_todos_comandos();
   assert (resultado.empty());
}

// -- -- --  Testes rápidos e simples de alguns funcionalidades -- -- -- -- 
void testando_comando_forque(void) {
   pid_t subprocesso = fork();

   for (size_t i = 1; i <= 2; i++) {
      if (subprocesso == E_SUBPROCESSO) { 
         sleep(5);
         cout << "[Child] o processo foi sucedido.\n"; 
      } else { 
         cout << "[Parent] o processo foi sucedido.\n"; 
      }
   }
}

void imprime_mensagem(string& msg) {
   sleep(2); // Pausa para exibição.
   cout << "Sua mensagem:\n\t'" << msg << "'\n" << endl;
   exit(0);
}

void teste_de_chamada_de_forque_externa_ao_escopo(void) {
   vector<string> mensagens {
      "Um dia lindo", "Cão que ladra, não morde",
      "Bom dia flor do dia"
   };

   for (string& msg: mensagens) { 
      if (fork() == E_SUBPROCESSO)
         imprime_mensagem(msg); 
      else
         cout << "Foi paralelizado com sucesso." << endl;
   }
}

void simples_teste_do_popen_systemcall(void) {
   FILE* _stream = popen("/bin/ls -1 /", "r");
   char buffer[1000];

   fread(buffer, sizeof(char), 1000, _stream);
   cout << buffer << endl;
}
//-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  -- -- -- -- 


int main (void) {
   executa_testes (
      8, verificando_extracao_de_caminhos, false,
         visualizacao_de_debug, false,
         testes_da_reparticao, false,
         processo_de_carregamento_de_cmds, true,
         // [OFF] pois consumem muito tempo:
         verificacao_da_validade_dos_comandos, false,
         // Verificando com funciona a funcionalidade:
         testando_comando_forque, false,
         teste_de_chamada_de_forque_externa_ao_escopo, false,
         simples_teste_do_popen_systemcall, false
   );
}
