#include "extracao_comando.hpp"
#include <deque>
#include <string>

void reparte_atravez_do_token (deque<string>& fila, string& cmd, 
  char token
) {
	// quebra string na ordem, e vai concatenando partes na 'fila'...
	size_t p = cmd.find (token);
	size_t t = cmd.length();
	string comando = cmd;

	if (p == string::npos)
		fila.push_back (comando);
	else {
		while (p != string::npos) {
			string primeiro = comando.substr (0, p);
			string segundo = comando.substr (p + 1, t);

			fila.push_back (primeiro);
			fila.push_back (segundo);

			// refazendo para a segunda parte(string).
			comando = fila.back();
			p = comando.find (token);
			if (p != string::npos) 
				fila.pop_back();
		}
	}
}

deque<string> reparte_comando (string cmd) {
	/* Reparte o programa em várias partes(outros comandos). Se não for
	 * possível de fazer-lô, então apenas retorna uma parte, que é o 
	 * próprio comando na fila.
	 */
	deque<string> colecao;
	reparte_atravez_do_token (colecao, cmd, '|');

	// reprocessando cada um dos comandos extráidos para outros tokens...
	size_t q = colecao.size();
	while (q-- > 0) {
		auto novo_cmd = colecao.front();
		colecao.pop_front();
		reparte_atravez_do_token (colecao, novo_cmd, '&');
	}
	
	q = colecao.size();
	while (q-- > 0) {
		auto novo_cmd = colecao.front();
		colecao.pop_front();
		reparte_atravez_do_token (colecao, novo_cmd, ';');
	}

	// retirando comandos em branco...
	size_t Q = colecao.size();
	while (Q-- > 0) {
		auto _cmd = colecao.front();
		colecao.pop_front();
		if (!_cmd.empty())
			colecao.push_back (_cmd);
	}
	
	return colecao;
}

string retira_caminho (string cmd) {
   size_t comeco = cmd.rfind('/');

   // verificando se há uma 'slash' que representa caminho.
   if (comeco != string::npos)
      return cmd.substr (comeco + 1, cmd.length());

   // retorna uma copia da string.
   return cmd;
}

#ifdef __unit_tests__
void visualiza_deque (deque<string> fila) {
	size_t total = fila.size();
	size_t contagem = 1;

	cout << "fila listagem: " << endl;
	while (total-- > 0) {
		auto item = fila.front();
		cout << '\t' << contagem++ << "º " << item << endl;
		fila.pop_front();
		fila.push_back (item);
	}	
}

void testes_da_reparticao (void) {
	string entradas[] = {
		"you only live once",
		"lonely boy",
		"ls -A some/directory/here | sort",
		"meu_comando_1 && meu_comando_2 | meu_comando_3",
		"libera_retorno | pega_o_retorno | reprocessa_ele | processo_final",
		"um_comando; depois_outro;mais_outro_ai"
	};

	for (string& entrada: entradas)
		visualiza_deque (reparte_comando(entrada));
}

void verificando_extracao_de_caminhos (void) {
	string entradas [] = {
		"./meu_comando_agora",
		"apenas_um_comando_so",
		"ls -A some/directory/here | sort",
		"meu_comando_1 && meu_comando_2 | meu_comando_3",
      "/pasta/outra_pasta/pasta_dentro_da_pasta/programaI",
		"./meu_programa/executa_aqui"
	};

	for (string& E: entradas) {
      cout << "antes: " << E << endl 
      << "depois: " << retira_caminho (E) 
      // dupla linha para separa bem as saídas.
      << endl << endl;
   }
}
#endif
