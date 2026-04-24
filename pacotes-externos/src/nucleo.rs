/*!
 *   Ficam os algoritmos referentes a filtragem, como dados estruturados que
 * ele talvez use, apelidos criados que são compartilhados ao longo do código,
 * constantes, testes unitários e etc. Os marcadores de comentários também
 * serão replicados prá cá. Eles são bons delimitadores de grandes porções de
 * códigos.
 */
use std::collections::HashMap;
use std::io::{ErrorKind, Result as ResultadoIO};
use std::path::{Path, PathBuf};
use std::fs::{read_dir};
use std::env::{var};

// Apelidos que facilitam na leitura.
pub type Pacote    = HashMap<String, Vec<String>>;
type Par       = (String, Vec<String>);
type Pares     = Vec<Par>;
type Caminhos  = Vec<PathBuf>;

#[cfg(target_os="linux")]
pub const RAIZ: &'static str = concat!(
   env!("HOME"),
   "/.cargo/registry/src"
);

#[cfg(target_os="windows")]
pub const RAIZ: &'static str = concat!(
   env!("HOMEPATH"),
   "/.cargo/registry/src"
);

/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Pública
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
pub fn organiza_fontes_e_suas_versoes(fontes: Caminhos) -> Option<Pacote>
{
   /* agloremando códigos-fontes iguais, porém com versões diferentes
    * deles. O retorno será um dicionário, onde a chave será o nome
    * da fonte, já os valores serão uma array contendo todas versões
    * disponíveis no sistema. Os resultado pode ser válido ou não,
    * dependendo se há algo no sistema.
    */
   let mut tudo = fontes;

   /* se estiver vázio, ou seja, não há nenhuma biblioteca de terceiros no
   computador, apenas retorna sem dados(none). */
   if tudo.is_empty() { return None; }

   /* dicionário contendo nome do código, e suas versões disponíveis. */
   // let mut compilado: HashMap<String, Vec<String>>;
   let mut compilado = Pacote::with_capacity(tudo.len());

   // inserindo organizadamente(não quer dizer ordenado) no dicionário ...
   for caminho in tudo.drain(..) {
      let (nome, versao) = identificando_fonte(&caminho);
      let versao_str = versao.to_string();

      if compilado.contains_key(nome) {
         let entrada = compilado.get_mut(nome).unwrap();
         /* evitando a mesma versão de ser adicionada novamente, por algum
          * motivo tal redundância está acontencendo. Algum erro bobo com
          * a listagem acima, porém está sendo resolvido aqui na inserção
          * do mapa. */
         if !entrada[..].contains(&versao_str)
            { entrada.push(versao.to_string()); }

      } else {
         compilado.insert(
            nome.to_string(),
            vec![versao.to_string()]
         );
      }
   }

   /* retornando o mapa contendo chaves(códigos-fontes),
    * e suas versões. */
   Some(compilado)
}

pub fn todos_diretorios_fontes() -> ResultadoIO<Caminhos> {
   // bolsa para coletar caminhos de códigos-fontes.
   let mut coletador = Vec::<PathBuf>::new();

   /* iterando os iniciais 2 diretórios, mais de uma forma
    * que avance dentro deles, vasculhando os diretórios
    * fontes de terceiros. */
   for pth in read_dir(RAIZ)? {
      let caminho = pth?.path();
      /* se for algum diretório, entrá nele, e finalizar o
       * trabalho(colocar um código-fonte na lista). */
      if caminho.as_path().is_dir() {
         for pth_inner in read_dir(caminho.as_path())? {
            let fonte = pth_inner?.path();
            if fonte.as_path().is_dir()
               { coletador.push(fonte); }
         }
      }
   }

   /* enviando de volta lista com possíveis códigos-fontes
    * coletados nele.*/
   Ok(coletador)
}

pub fn todos_diretorios_fontes_no_windows() -> ResultadoIO<Caminhos>
{
/* O mesmo algoritmo co que o 'todos_diretorios_fontes', entretanto à partir
 * de outro caminho, em outra partição. */
   let mut coletador = Vec::<PathBuf>::new();
   let raiz = raiz_no_windows();

   /* Se não houver nenhuma variável, não é possível fazer a buscar. Então,
    * apenas retorna a lista vázia. */
   if raiz.is_none()
      { return Ok(coletador); }

   for pth in read_dir(raiz.unwrap())? {
      let caminho = pth?.path();

      if caminho.as_path().is_dir() {
         for pth_inner in read_dir(caminho.as_path())? {
            let fonte = pth_inner?.path();
            if fonte.as_path().is_dir()
               { coletador.push(fonte); }
         }
      }
   }

   Ok(coletador)
}

pub fn ordena_repositorio(mut repositorio: Pacote) -> Pares {
   /* Ordena uma lista de strings de a até z, baseado na chave do mapa. */
   let mut array = repositorio.drain().collect::<Pares>();

   // algoritmo de ordenação bubblesort.
   for i in 0..array.len() {
      for j in (i + 1)..array.len() {
         if array[j].0 < array[i].0 {
            // swap clonando dados, faz possível, porém bastante pesado.
            let ptr: *mut Par = array.as_mut_ptr();
            unsafe {
               ptr.add(j).swap(ptr.add(i));
            }
         }
      }
      // ordenando suas versões, já que aqui itera todos.
   }

   /* array contendo tuplas do nome mais array com versões, ordenado
    * baseado no seu nome, ou seja, primeiro elemento da tupla. */
   array
}

/** Separa o nome da versão do código-fonte passado.*/
pub fn identificando_fonte<'a>(codigo_fonte: &'a Path)
  -> (&'a str, &'a str)
{
   // Nome do diretório em sí.
   let diretorio_nome = codigo_fonte.file_name().unwrap();


   /* divindo-o através do traço que separa o nome da versão. */
   match diretorio_nome.to_str().unwrap().rsplit_once('-')
   {
      Some(tupla) => (tupla.0, tupla.1),
      None =>
         { panic!("Nome: {diretorio_nome:?}"); }
   }
}

#[allow(dead_code)]
pub fn organizando_fontes_e_suas_versoes() -> Option<Pacote> {
   /* agloremando códigos-fontes iguais, porém com versões diferentes
    * deles. O retorno será um dicionário, onde a chave será o nome
    * da fonte, já os valores serão uma array contendo todas versões
    * disponíveis no sistema. Os resultado pode ser válido ou não,
    * dependendo se há algo no sistema.
    */
   let mut tudo = match todos_diretorios_fontes() {
      Ok(lista) => lista,
      Err(erro) => {
         match erro.kind() {
            ErrorKind::NotFound =>
            // Um problema de não haver tal diretório é aceitável.
               { return None; }
            _=>
               { panic!("[ERRO] {:?}", erro); }
         }
      }
   };

   /* se estiver vázio, ou seja, não há nenhuma biblioteca de terceiros no
   computador, apenas retorna sem dados(none). */
   if tudo.is_empty() { return None; }

   /* dicionário contendo nome do código, e suas versões disponíveis. */
   // let mut compilado: HashMap<String, Vec<String>>;
   let mut compilado = Pacote::with_capacity(tudo.len());

   // inserindo organizadamente(não quer dizer ordenado) no dicionário ...
   for caminho in tudo.drain(..) {
      let (nome, versao) = identificando_fonte(&caminho);
      let versao_str = versao.to_string();

      if compilado.contains_key(nome) {
         let entrada = compilado.get_mut(nome).unwrap();
         /* evitando a mesma versão de ser adicionada novamente, por algum
          * motivo tal redundância está acontencendo. Algum erro bobo com
          * a listagem acima, porém está sendo resolvido aqui na inserção
          * do mapa. */
         if !entrada[..].contains(&versao_str)
            { entrada.push(versao.to_string()); }

      } else {
         compilado.insert(
            nome.to_string(),
            vec![versao.to_string()]
         );
      }
   }

   /* retornando o mapa contendo chaves(códigos-fontes),
    * e suas versões. */
   Some(compilado)
}
/* -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- --- ---
 *                         Interface Privada
 * -- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---- -- -- -- */
/// Retorna a raíz do cache de repositórios baixados, porém na máquina Windows.
fn raiz_no_windows() -> Option<PathBuf> {
   // Supõe que a variável 'homepath', para o diretório 'home' fora do
   // Linux está definido.
   match var("HOMEPATH") {
      Ok(valor) => {
         let conteudo = Path::new(&valor);
         let complemento = ".cargo/registry/src";
         let caminho = conteudo.join(complemento);

         Some(caminho.to_path_buf())

      } Err(erro) => {
            if cfg!(debug_assertions)
               { println!("[ERRO] {erro}"); }
            None
         }
   }
}



#[cfg(test)]
#[allow(non_snake_case)]
mod tests {
   extern crate os_info;

   use super::*;
   use std::env::var;
   use std::process::{Command};


   #[test]
   fn verifica_se_esta_no_wsl_windows() -> std::io::Result<()>
   {
   /* Testando um prototipo de função que me da o kernel sendo utilizado.
    * Como não quero usar bibliotecas terceiras, apenas uso um comando
    * tanto comum no Linux(puro) como o Linux(WSL). */
      let result = Command::new("uname").arg("-r").output()?;
      let codigo = result.status;
      let dados = result.stdout;
      let conteudo = String::from_utf8_lossy(&dados);
      let contem_nome_a = conteudo.contains(&"microsoft");
      let contem_nome_b = conteudo.contains(&"WSL2");
      let provavelmente_um_wsl = contem_nome_a && contem_nome_b;

      println!("Exit code: {}\nSaída: '{}'\n", codigo, conteudo);
      println!("É um Linux(WSL)? {}", provavelmente_um_wsl);
      Ok(())
   }

   #[test]
   fn verificando_se_filtra_fontes_apenas() {
      let possiveis_sources = todos_diretorios_fontes();

      for path in possiveis_sources.unwrap() {
         let caminho = path.join("./Cargo.toml");

         // visualizando caminho.
         println!("{}", caminho.display());

         /* se todos tiverem um arquivo 'Cargo.toml', estão a meio
          * caminho de se confirmarem com um verdadeiro código-fonte
          * de Rust.*/
         assert!(caminho.exists());
      }
   }

   #[test]
   fn separacao_perfeita_das_fontes() {
      let possiveis_sources = todos_diretorios_fontes();

      for path in possiveis_sources.unwrap() {
         let (nome, versao) = identificando_fonte(path.as_path());
         // visualizando caminho.
         println!(
            "'{}'\nEntão após aplicado a função =>
            \r\tnome: {nome}
            \r\tversão: {versao}
            ", path.display(),
         );

      }

      // avaliação manual.
      assert!(true);
   }


   #[test]
   fn comparacao_strings() {
      assert!("abacate" < "abacaxi");
   }

   #[test]
   #[ignore="apenas um teste da biblioteca"]
   fn que_tipo_de_informacao_fornece() {
      let X = os_info::get();
      const Y: &str = " --- ";

      println!(
         "Informações sobre a máquina:\n\tArquitetura: {}\n\t \
         Sistema do Tipo:{:?}\n\tCodename do OS: {}\n\tEdição: '{}'\n\t \
         Tipo de OS: {:?}\n\tVersionamento: {:?}",
         X.architecture().unwrap_or(Y), X.bitness(),
         X.codename().unwrap_or(Y), X.edition().unwrap_or(Y),
         X.os_type(), X.version()
      );
   }

   #[test]
   #[ignore="simples verificação de ambiente"]
   fn variavel_de_ambiente_existente() {
      match var("LINKS") {
         Ok(data) =>
            { println!("LINKS: {data:}"); }
         Err(erro) => {
            println!("Está é a mensagem de erro: '{erro:?}'");
            assert!(false);
         }
      }
   }
}
