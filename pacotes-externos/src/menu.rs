/** Jogando o menu para este módulo posso colocar o máximo de funcionalidades
 * o  possível, sem necessáriamente bagunçar o código do menu principal.
 */

use std::env::{args};

pub type OpcoesSelecionadas = Vec<Opcoes>;

#[derive(Debug)]
pub enum Opcoes {
   RealizaResgistro,
   QtdDeRegistros,
   Ajuda,
   Listagem,
   Nenhuma,
   Invalido
}

pub fn opcoes_selecionadas() -> OpcoesSelecionadas {
   let mut output = Vec::<Opcoes>::new(); 
   let contagem = args().count();

   if cfg!(debug_assertions)
      { println!("Quantia de argumentos: {}", contagem); }

   for arg in args() {
      if arg == "--ajuda" || arg == "--help" || arg == "-h"
         { output.push(Opcoes::Ajuda); }

      if arg == "--registrar" || arg == "-R"
         { output.push(Opcoes::RealizaResgistro); }

      if arg == "--quantia-realizados" || arg == "-q"
         { output.push(Opcoes::QtdDeRegistros); }

      if arg == "--listagem" || arg == "-l"
         { output.push(Opcoes::Listagem); }
   }

   if contagem > 1
      { output.push(Opcoes::Invalido); }
   else
      { output.push(Opcoes::Nenhuma); }

   output
}

pub fn manual_de_ajuda() {
   println!("
      \rPROGRAMA\n\tpacotes-exeternos <opções> ...
      \n\rOPÇÕES
      \r\t--listagem, -l
      \r\t\t A função principal do programa, que é mostrar os arquivos que 
      \r\t\t sobraram no cache quando foram baixados, seja isso em qualquer
      \r\t\t programa Rust que usou de terceiros, e puxo ele do repositório
      \r\t\t geral, ou outros.
      \r\t--registrar, -R
      \r\t\t Não realiza um registro após o final da listagem exigida.
      \r\t--quantia-realizados, -q
      \r\t\t Mostra a quantidade de registros realizados até o momento. 
      \r\t\t É uma função muito importante, já que, quanto mais registros
      \r\t\t mais tempo leva para operação de carregar ser computada. Pelo
      \r\t\t menos a mais básica.
      \r\t--ajuda, --help, -h
      \r\t\t Mostra este manual ao usuário.
   ");
}

