/**
   Este programa tem como objetivo substituir a listagem de libs externas do
 Rust, estas que foram baixados por outras bibliotecas na hora de
 compilação, ou adicionadas por mim mesma de modo manual. Claro que será uma
 coisa bem mais organizada do que o simples comando faz hoje, usando de
 sintaxe colorida e organizando versões numa simples linha.
*/

mod historico;
mod banco;
mod menu;
mod visualiza;
mod nucleo;

// Biblioteca padrão do Rust:
use std::process::{exit};
use std::collections::{HashMap};
// Ferramental dos módulos criados:
use historico::{Historico};
use banco::{registra_um_historico, carrega_historicos};
use menu::{Opcoes, opcoes_selecionadas, manual_de_ajuda};
use visualiza::{
   listagem_das_fontes, ambiente_e_wsl_no_windows,
   desenha_barra_delimitadora_da_listagem
   }; 
use nucleo::{
   todos_diretorios_fontes, 
   todos_diretorios_fontes_no_windows,
   organiza_fontes_e_suas_versoes
   }; 

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
fn main() {
   /* O menu de todas opções personalizaveis que este programa pode promover
    * até o momento. Futuramente, obviamente haverá bem mais. Por enquanto
    * é apenas isso. Veja também, que a listagem, que é a funcionalidade
    * principal do programa. Se transformou numa opção obrigatoria prá
    * executar. É claro que, pode ser que, seja ativada quando não houver
    * opção, ou talvez a 'ajuda' seja a padrão. Ainda não tem certezea de
    * qual usar. */
   for opcao in opcoes_selecionadas() {
      match opcao {
         Opcoes::Nenhuma => 
            { mostra_listagem_dos_pacotes_baixados(); }

         Opcoes::Listagem => {
            mostra_listagem_dos_pacotes_baixados();
            realiza_um_novo_registro_de_historico_ao_sair();
         }
         Opcoes::QtdDeRegistros => {
            if let Ok(fila) = carrega_historicos() {
               println!(
                  "Foram realizados {} registros até momento.",
                  fila.len()
               );
            } else
               { println!("Não foi possível obter um histórico."); }
         } 
         Opcoes::Ajuda =>
            { manual_de_ajuda(); }
         Opcoes::Invalido =>
            { println!("Opção passada, não existe."); }
      }
   }
}
/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
/** Realiza um novo registro de histórico ao sair do programa toda vez. Por
 * enquanto, não discrimina se não há tanta variação de um 'snap' para outro,
 * apenas vai gravando algo novo. */
fn realiza_um_novo_registro_de_historico_ao_sair()
{
   let snap = Historico::gera();

   if let Ok(size) = registra_um_historico(snap)
      { println!("Foi gravado um registro de {} bytes.", size); }
    else
     { println!("Nenhum registro no BD foi realizada."); }
}

fn mostra_listagem_dos_pacotes_baixados() {
   /* preciso apenas chamar a função 'organizando_fontes_e_suas_versoes',
    * para obter o mapa com os dados, e organizar-lô, de maneira bem
    * mais bonita, do que sua versão alinhada de 'debug'.
    */
   // Resultado das varreduras por pacotes em ambos sistemas.
   let varredura_linux = todos_diretorios_fontes().unwrap();
   let varredura_win = todos_diretorios_fontes_no_windows().unwrap();
   // Organiza as listas obtidas agregando pacotes iguais em versões.
   let repositorio_linux = organiza_fontes_e_suas_versoes(varredura_linux);
   let repositorio_win = organiza_fontes_e_suas_versoes(varredura_win);
   // Referências aos dicionários acima.
   let repo_linux = &repositorio_linux.as_ref().unwrap();
   /* Para não ocorrer um erro no Linux puro, ele retorna um mapa vázio, pelo
    * erro 'levantado' no unwrap da lista de repositórios no WLS. */
   let vazio = HashMap::<String, Vec<String>>::new();
   let repo_win = &repositorio_win.as_ref().unwrap_or(&vazio);

   if repositorio_linux.is_none()
      { println!("Não há repositórios neste sistema."); exit(0); }

   // Soma de todas versões, é o total de pacotes baixados.
   let total_de_pacotes_nominal = repo_win.len() + repo_linux.len();
   let total_de_pacotes_real = {
      repo_linux.iter()
      .map(|(_, array)| array.len())
      .sum::<usize>() 
               + 
      repo_win.iter()
      .map(|(_, array)| array.len())
      .sum::<usize>()
   };
   let recuo = &" ".repeat(4);

   println!(
      "\nDepedências baixadas no computador({} \
      pacotes distintos, {} no total):",
      total_de_pacotes_real,
      total_de_pacotes_nominal
   );

   if ambiente_e_wsl_no_windows()
   {
      println!("\n{recuo}Pacotes referentes ao pacotes Linux(WSL 2):");
      listagem_das_fontes(repositorio_linux.unwrap());

      println!("\n{recuo}Pacotes referentes ao pacotes Windows:");
      if repo_win.len() > 0
         { listagem_das_fontes(repositorio_win.unwrap()); }
      else
         { println!("{recuo}Nenhum pacote foi detectado no sistema."); }
   } else {
   /* Se a execução for primariamente numa distribuição Linux. Sem esta está
    * sendo emulada dentro do Windows pelo WSL2, então a visualização clássica
    * abaixo é o suficiente. */
      listagem_das_fontes(repositorio_linux.unwrap());
   }
   desenha_barra_delimitadora_da_listagem();
}
