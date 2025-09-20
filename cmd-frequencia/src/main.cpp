
// Biblioteca padrão do C++:
#include <iostream>
#include <filesystem>
#include <map>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cstdio>
#include <fstream>
// Outros módulos de programa:
#include "extracao_comando.hpp"

namespace fs = std::filesystem;
using namespace std;
using std::deque;
using std::string;

const string NOME_ARQUIVO = ".bash_history";
auto CAMINHO = fs::path(getenv("HOME")) / NOME_ARQUIVO;
// símbolos usados na visualização abaixo.
const char BARRA = '#';
const char VAZIO = ' ';

typedef map<string, uint16_t> GrupoFrequencias;
typedef tuple<string, uint16_t> frequencia_t;
typedef vector<frequencia_t> TodasFrequencias;


static void visualizaca_mapa(GrupoFrequencias& m) {
   /* mostra cada chave do dicionário e seu valor respectivo. */
   for (auto item = m.begin(); item != m.end(); ++item) {
      auto chave = item->first;
      auto valor = item->second;
      cout << '\'' << chave << '\''<< ':' << valor << endl;
   }
}

static void adiciona_comando (GrupoFrequencias& dicio, string& cmd) {
   string& chave = cmd;

   // conta uma chave(comando) nova, e incrementa uma já existente.
   if (dicio.count(cmd) == 0)
      dicio[chave] = 1;
   else
      dicio[chave] += 1;
}

static string extrai_programa (string cmdstr) {
   size_t fim = cmdstr.find(' ');
   return cmdstr.substr(0, fim);
}

auto computa_frequencia(ifstream& arquivo) -> GrupoFrequencias {
/* Toma o arquivo itera cada linha dele, corta o trecho até o primeiro 
 * espaço vázio, este é o comando. Insere no dicionário, com a contagem
 * de um, se falhar, tal comando já pode está lá, logo incrementa a atual
 * contagem apenas. 
 */
   GrupoFrequencias compilado;
   string linha;

   if (arquivo.is_open()) {
      // iterando cada linha até o fim do arquivo.
      do {
         // pegando tal comando...
         getline(arquivo, linha);

         #ifdef _DEBUG_COMPUTA_FREQUENCIA
         cout << "linha: \"" << linha << '\"' << endl;
         #endif // DEBUG COMPUTA FREQUENCIA

         // partes comandos concatenados ou em pipelines.
         for (auto cmd: reparte_comando (linha)) {
            // pega apenas o comando...
            auto novo_cmd = extrai_programa(cmd);
            // agora tira possível caminho com reprocessamento...
            novo_cmd = retira_caminho (novo_cmd);
            // finalmente coloca o comando puro(programa) na 'lista'.
            adiciona_comando (compilado, novo_cmd);
         }

      } while (!arquivo.eof());
   }

   // removendo a chave em branco...
   compilado.extract("");
   return compilado;
}

static void ordenacao_decrescente(vector<frequencia_t>& array) {
/* Ordena todas 'frequências' aqui na ordem descrescente, onde quanto
 * maior, mais à esquerda. Usando o 'bubble sort', algoritmo fácil de
 * implementar. */
   size_t contagem = 0;
   size_t t = array.size();

   for (size_t p = 0; p < t; p++) {
      for (size_t k = p + 1; k < t; k++) {
         uint16_t valor_a, valor_b; string s;
         // valores de comparação.
         tie(s, valor_a) = array[k];
         tie(s, valor_b) = array[p];

         if (valor_a > valor_b) {
            // aplica o swap.
            auto aux = array[k];
            array[k] = array[p];
            array[p] = aux;
            // contabilizando o total de trocas realizadas.
            contagem++;
         }
      }
   }
   #ifdef _DEBUG_ORDENACAO_DECRESCENTE
   cout << "foram realizados " << contagem << " swaps\n";
   #endif
}

vector<frequencia_t> amostra_de_mais_frequentes(GrupoFrequencias& m) 
{
/* Pega amostra aleatória de frequências crescentes, mais não ainda os
 * mais frequentes. Como será feito isso é o seguinte. Pegamos */
   vector<frequencia_t> colecao;
   const uint16_t LIMITE = 20;

   for (auto item = m.begin(); item != m.end(); ++item) 
   {
      auto valor = item->second;
      auto chave = item->first;

      colecao.push_back(make_tuple(chave, valor));
   }   

   /* neste caso é necessário a ordenação para remoção das pontas menos
    * frequentes(à direita). */
   ordenacao_decrescente(colecao);

   while (colecao.size() > LIMITE) 
      colecao.pop_back();
   
   return colecao;
}

static void visualizacao_de_frequencia(frequencia_t n, uint16_t t) {
   // valores da tupla passada.
   string chave; uint16_t qtd;
   tie(chave, qtd) = n;
   // computando quantidade pausinhos(nível) a escrever.
   float p = (float)qtd / (float)t;
   uint16_t m = p * 10;


   // ínicio da visualização frequência contabilizada entre barras.
   printf("[");
   // escreve o total de pausinhos necessários.
   for (size_t k = 1; k <= 10; k++) {
      if (k <= m) printf("%c", BARRA);
      else printf("%c", VAZIO);
   }
   // mostrando o comando em sí, e sua frequência de utilização.
   printf("] %4d\t%s\n", qtd, chave.c_str());
}

static void visualizacao(vector<frequencia_t>& lista)
{
   /* visualiza uma amostra de frequências(nome do comando se sua contagem
    * ). Apenas itera a lista e manda visualizar cada 'frequência nela' na
    * ordem de iteração dada. */
   for (auto tupla: lista) 
      visualizacao_de_frequencia(tupla, 300);

   cout << "\ntotal de comandos filtrados: " << lista.size() << endl;
}

static vector<frequencia_t> todas_disponiveis(GrupoFrequencias& m) {
   vector<frequencia_t> colecao;

   for (auto item = m.begin(); item != m.end(); item++) 
      colecao.push_back(make_tuple(item->first, item->second));

   #ifdef _DEBUG_ORDENACAO_DECRESCENTE
   ordenacao_decrescente(colecao);
   #endif
   return colecao;
}


#ifdef __unit_tests__
/* Se a diretriz de testes unitários for acionada, compila apenas esta parte
 * do código, onde estão localizados os testes unitários. deste módulo em
 * específico. No futuro, até existe planos de criar um módulo apenas para
 * estas ferramentas, mas enquanto compartilham o arquivo 'main' este é um
 * modo válido de deixar o arquivo bem mais legível. */
#include "unitarios.cpp"

#else
/* Projeto principal, este que recebe os argumentos e opera o software de 
 * acordo com o que foi demandado. */
#include <optional>
#include <unistd.h>
#include <cassert>
#include "lincagem.hpp"
#define SEM_MAIS_ARGUMENTOS -1

void gerencia_opcoes_impostas(
  int qtd, char** argumentos, bool& havera_visualizacao, 
  vector<frequencia_t>& exemplos, GrupoFrequencias& frequencias
){
   int opcao = getopt(qtd, argumentos, "Mht");

   if (opcao != SEM_MAIS_ARGUMENTOS) {
      switch (opcao) {
         case 't':
            exemplos = todas_disponiveis(frequencias);
            havera_visualizacao = true;
            puts("Você optou por mostrar todas frequências disponíveis.");
            break;
         case 'h':
            puts(
               "\nEste é o leque de opções:\
               \n\t -t --tudo lista todos comandos.\
               \n\t -M --mais-frequentes comandos mais frequentes.\n"
            );
            break;
         case 'M':
            puts("Mostrando apenas os comandos mais frequentes.");
            havera_visualizacao = true;
            exemplos = amostra_de_mais_frequentes(frequencias);
            break;
         default:
            puts(
               "Não escreva opções inválidas, use '-h' para visualizar"
               " todas opções disponíveis."
            );
            abort();
      }
   } else 
      puts("nenhuma opção foi selecionada.");
}

int main(int qtd, char* argumentos[], char* variaveis[]) 
{
   lincagem::tenta_criar_linque_ao_iniciar_programa();

   // processando todos comandos e contando sua frequência...
   ifstream arquivo(CAMINHO, ios::in);
   auto frequencias = computa_frequencia(arquivo); 
   arquivo.close();

   // amostra de como será a visualização ...
   vector<frequencia_t> exemplos;
   bool havera_visualizacao = false;

   gerencia_opcoes_impostas
     (qtd, argumentos, havera_visualizacao, exemplos, frequencias);

   if (havera_visualizacao) {
      cout << endl; 
      visualizacao(exemplos);
      cout << endl;
   }
}
#endif
