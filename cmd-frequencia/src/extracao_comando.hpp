#pragma once
#include <deque>
#include <string>

using std::string;
using std::deque;

void reparte_atravez_do_token
  (deque<string>& fila, string& cmd, char token);
deque<string> reparte_comando (string cmd); 
string retira_caminho (string cmd);
