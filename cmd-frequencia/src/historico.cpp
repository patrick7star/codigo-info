
/* Salva o arquivo '.bash_history', pois ele tem um limite de comandos 
 * guardados, portanto, toda nova sessão de terminal acaba, ele grava os 
 * comandos usados nela no tal arquivo, sobreescrevendo os bem antigos.
 * Então temos que dá um jeito de pegar os comandos perdidos, e colocar-los
 * num grande 'banco de comandos', digo todos já executados.
 */

#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <cmath>

// Apelidos e renomeações:
namespace fs = std::filesystem;
typedef std::vector<std::string> HistoricoCMD;
const int16_t NAO_ENCONTRADO = -1;

static HistoricoCMD lista_sequencial_do_historico(fs::path caminho) {
   std::fstream arquivo(caminho, std::ios::in);
   HistoricoCMD sequencia;

   #ifdef __debug__
   size_t _count = 1;
   #endif

   do {
      std::string linha; linha.reserve(500);
      std::getline(arquivo, linha);
      if (!linha.empty())
         sequencia.push_back(linha);

      #ifdef __debug__
      if (_count < 20 || _count > 1980) 
         std::cout << _count << "º comando: " << linha << std::endl;
      _count++;
      #endif
   } while(!arquivo.eof());

   arquivo.close();
   return sequencia;
}

static auto correspodencia_de_n(HistoricoCMD& a, HistoricoCMD& b, 
  uint16_t i, uint16_t n) -> bool
{
/* Verifica se à partir de dado ponto, todos 'n' itens, de ambas arrays,
 * correspodem uma ao outro. O retorno será lógico. */
   for (uint16_t p = i; i <= n; p++) {
      // Se um único não corresponder, então é falso.
      if (a[p] != b[p])
         return false;
   }
   // Se chegar até o fim, então simplesmente todos correspodem.
   return true;
}

static auto posicao_intersecao_dos_historicos(HistoricoCMD& antigo, 
  HistoricoCMD& novo)  -> std::int16_t
{
   const int16_t BLOCO_LEITOR = 5;
   int16_t fim = antigo.size() - BLOCO_LEITOR;
   int16_t posicao = NAO_ENCONTRADO;

   #ifdef __debug__
   float P[] = { 0.05, 0.10, 0.15 };
   size_t t = antigo.size();

   std::cout << "Cinco, dez e quinze porcentos do antigo: " 
      << round(P[0]*t) << ", " << round(P[1]*t) 
      << " e " << round(P[2]*t) << std::endl;
   #endif

   for (uint16_t i = 1; i <= fim; i++) {
      // Todos os 'n' batem, à partir de tal inicio.
      if (correspodencia_de_n(antigo, novo, i, BLOCO_LEITOR)) {
         posicao = i;
         break;
       }
   }
   return posicao;
}

#ifdef __debug__
// Biblioteca padrão do C:
#include <cassert>
#include <cerrno>
#include <cstring>
// Glib: 
#include <sys/stat.h>
// Biblioteca externa:
extern "C" { 
#include "dados_testes.h"
}

void funcao_listagem_do_historico(void) {
   fs::path caminho{getenv("HOME")};
   caminho /= ".bash_history";
   std::cout << "Caminho procurado: '" << caminho << "'\n";
   auto result = lista_sequencial_do_historico(caminho);
   std::cout << "Total filtrado: " << result.size() << std::endl;
   assert (result.size() <= 2000);
}

static auto e_permitido_fazer(std::string caminho) {
   struct stat info;
   int result = stat(caminho.c_str(), &info);

   if (result == -1 && errno == ENOENT)
      return true;
   else 
      return info.st_size == 0;
   return false;
}

static void criando_arquivos_testes(void) {
   int result;
   char* erro_msg;

   // Criando diretórios necessários para fazer arquivos de testes.
   result = mkdir("data", S_IRWXU);
   if (result != 0) {
      erro_msg = std::strerror(errno);
      std::cout << "[error(data)] " << erro_msg << std::endl;
   }
   result = mkdir("./data/testes", S_IRWXU);
   if (errno == EEXIST)  {
      erro_msg = std::strerror(errno);
      std::cout << "[error(data/testes)] " << erro_msg << std::endl;
   } else {
      std::cout << "Diretório 'data' criado com sucesso.\n";
      char* erro_msg = std::strerror(errno);
      std::cout << erro_msg << std::endl;
      assert (result == 0);
   }

   const std::string OLD_FILE {"data/testes/antigo.txt"};
   const std::string NEW_FILE {"data/testes/novo.txt"};
   std::ofstream arquivo_antigo {OLD_FILE, std::ios::out};
   std::ofstream arquivo_novo {NEW_FILE, std::ios::out};

   if (e_permitido_fazer(OLD_FILE)) {
      std::cout << "Escrevendo em " << OLD_FILE << std::endl;
      // Velho arquivo com alguns nomes:
      for (size_t k = 1; k <= GIRLS_NAMES; k++)
         arquivo_antigo << girls_names[k - 1] << '\n';
      for (size_t k = 1; k <= NOMES_DE_MENINAS; k++)
         arquivo_antigo << nomes_de_meninas[k - 1] << '\n';
   } else 
      std::cout << "O arquivo '" << OLD_FILE << "' já foi escrito\n";
   arquivo_antigo.close();


   if (e_permitido_fazer(NEW_FILE)) {
      std::cout << "Escrevendo em " << NEW_FILE << std::endl;
      // O novo arquivo com nomes acima, e mais nomes inseridos.
      for (size_t k = 1; k <= BOYS_NAMES; k++)
         arquivo_novo << boys_names[k - 1] << '\n';
      for (size_t k = 1; k <= GIRLS_NAMES; k++)
         arquivo_novo << girls_names[k - 1] << '\n';
      for (size_t k = 1; k <= NOMES_DE_MENINAS; k++)
         arquivo_novo << nomes_de_meninas[k - 1] << '\n';
   } else 
      std::cout << "O arquivo '" << NEW_FILE << "' já foi escrito\n";
   arquivo_novo.close();
}

void localizacao_onde_comecao_antigo_historico(void) {
   criando_arquivos_testes(); 
   HistoricoCMD old, new_;

   old = lista_sequencial_do_historico("data/testes/antigo.txt");
   new_ = lista_sequencial_do_historico("data/testes/novo.txt");
   int16_t posicao_linha = posicao_intersecao_dos_historicos(old, new_);

   std::cout << "Posição que do antigo: " << posicao_linha << std::endl;
}

int main(void) {
  // funcao_listagem_do_historico(); 
  localizacao_onde_comecao_antigo_historico();
}
#endif
