#pragma once

#include <filesystem>
#include <string>
#include <optional>


namespace lincagem {
  using std::optional;
  using std::string;
  namespace fs = std::filesystem;


  // Retorno o caminho do atual executável do sistema.
  auto caminho_do_executavel(void) -> fs::path;
  /* Retorno o caminho do diretório onde todo este programa está armazenado.
   * Ele parte do princípio que o executável que chama tal programa está 
   * neste mesmo diretório. */
  auto diretorio_do_programa(void) -> optional<fs::path>; 

  /* Cria um linque para um 'executável' com um 'nome' dado. */
  auto cria_linque_no_devido_respositorio
    (const string nome) -> optional<fs::path>;

  void tenta_criar_linque_ao_iniciar_programa(void);
}


/* Estreando o mod debug. É muito melhor do que ficar escrevendo o
 * pré-processador toda vez, apenas coloca o código num bloco 'if' como os
 * nomes abaixados serão evaluados automaticamente. Para aciona tal é 
 * preciso passar um macro prédefinido na compilação do programa. */
#ifdef __debug__
#define __modo_debug__ true
#else
#define __modo_debug__ false
#endif
// Nomes alternativos que também funcionam.
#define __MODO_DEBUG__        __modo_debug__
#define __debug_assertions__  __modo_debug__
#define   MODO_DEBUG          __modo_debug__
#define   debug_assertions    __modo_debug__
#define   DebugAssertions     __modo_debug__
