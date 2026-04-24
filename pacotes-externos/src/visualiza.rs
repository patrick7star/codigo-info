/*! Parte do código que ficará toda parte referente a visualização do código.
 *  Tantos as constantes, funções públicas e privadas(obviamente), sem deixar
 * mencionar dos testes unitários refentes, apelidos e etc. Isso tem como 
 * objetivo esvaziar um pouco o 'main'. Fica muito bagunçado e difícil de 
 * encontrar o que é importante. Também, ficar melhor em detectar parte do 
 * código relacionado a isso lá. 
 */
use std::process::{Command};
use crate::nucleo::{Pacote, ordena_repositorio};

/// Desenha a barra no fim da visualização.
pub fn desenha_barra_delimitadora_da_listagem()
   { println!("\n{}\n", &"-".repeat(60)); }


/// Verifica se o programa está no Linux(WSL) no Windows.
pub fn ambiente_e_wsl_no_windows() -> bool{
      const PROGRAMA: &str = "/usr/bin/uname";
      match Command::new(PROGRAMA).arg("-r").output() {
         Ok(result) => {
            let dados = result.stdout;
            let conteudo = String::from_utf8_lossy(&dados);
            // Proposições.
            let feito_pela_microsoft = conteudo.contains(&"microsoft");
            let uma_versao_linux_emulada = conteudo.contains(&"WSL2");

            // Ambas proposições tem que se verdadeiras.
            feito_pela_microsoft && uma_versao_linux_emulada
         } Err(_) => false
      }
}

pub fn listagem_das_fontes(repositorio: Pacote) {
   /* cuidando especialmente da função de visualização de todos estes
    * dados, formando a saída para o modo mais legível possível.
    */
   if repositorio.is_empty()
      { println!("\nnão há nada aqui!\n"); return (); }

   // baseando a formatação no comprimento da maior string.
   let maior_comprimento = {
      repositorio.keys()
      .map(|s| s.len())
      .max().unwrap() + 4
   };

   /* boa listagem de das versões, que se preocupa em arrancar a última
    * vírgula. */
   fn listagem_versoes(mut l: Vec<String>) {
      let total = l.len();

      for (i, numero) in l.drain(..).enumerate() {
         if i == (total - 1)
            { print!("v{}", numero); }
         else
            { print!("v{} | ", numero); }
      }
   }

   for (nome, versoes) in ordena_repositorio(repositorio).drain(..) {
      print!("\t{0:.<maior_comprimento$} \u{27e8}", nome);
      listagem_versoes(versoes);
      println!("\u{27e9}");
   }
}

#[cfg(test)]
mod tests {
   use super::{listagem_das_fontes, };
   use crate::nucleo::{organizando_fontes_e_suas_versoes, Pacote, RAIZ};

   #[test]
   fn informacao_compilada_nomes_e_versoes() {
      for entrada in organizando_fontes_e_suas_versoes().unwrap()
         { println!("{:?}", entrada);  }

      // avaliação manual.
      assert!(true);
   }

   #[test]
   fn novo_tipo_de_visualizacao() {
      let funcao: fn() -> Option<Pacote>;
      funcao = organizando_fontes_e_suas_versoes;
      let todo_repositoiro = funcao().unwrap();
      listagem_das_fontes(todo_repositoiro);
      // uma avalização manual?
      assert!(true);
   }

   #[test]
   fn variavel_ambiente_home_no_wsl() {
      println!("Home: '{}'", env!("HOME"));
      println!("HomePath: '{}'", env!("HOMEPATH"));
      // Variável obtém que valor:
      println!("{RAIZ}");
   }

}

