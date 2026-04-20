// Cabeçalho do módulo abaixo:
#include "lincagem.hpp"
// Bibliotecas do C++:
#include <iostream>
#include <exception>
// Biblioteca externa do C:
#include <cstdlib>
#include <cassert>
#include <climits>
// Biblioteca da plataforma(Unix):
#include <unistd.h>
#include "macros.h"

// const int Okay = 0;
enum resultados_da_syscalls { Failed = -1, Okay };
[[gnu::unused]]
constexpr const char* DIRETORIO_DO_PROGRAMA = "comandos-frequencia";
constexpr const char* LINQUE_NOME = "cmd-frequencia";


auto lincagem::caminho_do_executavel(void) -> fs::path
{
   pid_t id = getpid();
   string processo_str = std::to_string(id);
   fs::path link_do_executavel("/proc");
   fs::path output;
   const int MAX = UCHAR_MAX * 3;
   char buffer[MAX];

   link_do_executavel /= fs::path(processo_str);
   link_do_executavel /= fs::path("exe");
   readlink(link_do_executavel.c_str(), buffer, MAX);

   return fs::path(buffer);
}

auto lincagem::cria_linque_no_devido_respositorio (const string nome) 
  -> optional<fs::path> 
{
/* O destino do linque será tanto local como no repositório de linques do
 * usuário(LINKS). Se não houver tal caminho válido, o programa será 
 * interrompido. */
   char* caminho_link = getenv("LINKS");
   fs::path binario = caminho_do_executavel();
   fs::path linque_path{caminho_link};

   if (caminho_link == nullptr) {
      std::cout << "O linque \"$LINKS\" não existe.\n";
      return std::nullopt;
   }
   // Anexando o nome do executável ao caminho.
   linque_path /= fs::path(nome);

   try {
      fs::create_symlink(binario, linque_path);
      return std::make_optional(linque_path);
   } catch (const std::exception& erro) {
      return std::nullopt;
   }
} 

auto lincagem::diretorio_do_programa(void) -> optional<fs::path> {
/* Retorna o caminho ao diretório que este programa está armazenado no 
 * momento da execução. Como faz isso? Bem simples, ele somente pega o
 * caminho do executável, então apara os diretórios até que se chegue no
 * que bate com o nome do diretório. */
   const fs::path parada{DIRETORIO_DO_PROGRAMA};
   const fs::path sem_caminho{""};
   fs::path output = caminho_do_executavel();

   do {
      fs::path nome = output.filename();

      if (nome == parada)
      /* Neste caso, se chegou a base, loga abandona o loop para que o
       * caminho composto seja retornado. */
         break;
      else if (nome == sem_caminho)
      /* Se chegar num "caminho em branco", isso implica que o caminho 
       * passado não é válido, ou seja, não tem a base que para. O retorno
       * disso é um valor inválido, que não pode ser trabalhado. */
         return std::nullopt;

      output = output.parent_path();

   } while (true);

   return std::make_optional(output);
}

void lincagem::tenta_criar_linque_ao_iniciar_programa(void)
{
   fs::path executavel_path = caminho_do_executavel();
   string nome_do_linque{LINQUE_NOME};
   std::optional<fs::path> tentativa_de_criacao;
   // auto chamada = lincagem::cria_linque_no_devido_respositorio;
   optional<fs::path> (*chamada)(string);

   chamada = &lincagem::cria_linque_no_devido_respositorio;
   tentativa_de_criacao = chamada(nome_do_linque);

   if (!tentativa_de_criacao.has_value()) 
      std::cout << "[Falha]Provavelmente o link já existe." 
         << std::endl << std::endl;
   else {
      std::cout << "Linque criado com sucesso.\n\n";
      assert(fs::exists(*tentativa_de_criacao));
   }
}

#ifdef __unit_tests__
/* === === === === === === === === === === === === === === === === === ==
 *                      Testes Unitários
 * === === === === === === === === === === === === === === === === === ==*/
#include <array>
extern "C" {
  #include "teste.h"
   
  void executa_testes_a  (bool ativado, int total, ...);
}

// Usando alguns apelidos pra namespaces:
using std::array;
using lincagem::string;
namespace fs = lincagem::fs;

void criacao_do_linque_no_respositorio_links(void) {
   using lincagem::cria_linque_no_devido_respositorio;

   const string name{"uTlIcAgEm"};
   auto pth = cria_linque_no_devido_respositorio(name);

   if (pth.has_value()) {
      assert(fs::exists(pth.value()));
      assert(fs::remove(*pth));
      assert(!fs::exists(pth.value()));
   }
}

void tentativa_de_criar_linque_para_executavel(void)
{
   lincagem::tenta_criar_linque_ao_iniciar_programa();
}

int main(int total, char** args, char* envs[]) 
{
   executa_testes_a(
     true, 2,
      criacao_do_linque_no_respositorio_links, true,
      tentativa_de_criar_linque_para_executavel, true
   );
}
#endif
