/*! O que gravar como histórico: Todos os pacotes que fazem parte, mais um 
 * selo de tempo? Vejo isso promissor, entretanto, cada nova chamada onde 
 * altera-se apenas o selo de tempo, pois não houve mais nova entradas e 
 * saídas de pacotes antigos. Bem, este é o problema que vejo com tal
 * abordagem.
 */

use std::path::{Path};
use std::fs::{File};
use std::io::{Read};
use std::fmt::{Formatter, Result as ResultFmt, Debug, Display};
use std::time::{SystemTime};
use std::collections::{VecDeque};
use crate::todos_diretorios_fontes;
use std::time::{Duration};

pub trait Serializador {
   /// Transforma o objeto numa array de bytes. 
   fn serializa(&self) -> Vec<u8>;

   /// Transforma uma array variável de bytes, no objeto novamente.
   fn deserializa(dados: Vec<u8>) -> Self;
}

/** O tipo de dado consiste em, primeiro, o nome do pacote, seguido de 
 * sua versão.*/
pub struct LibRust(String, String);

/** O conceito 'histórico' consiste na lista de LibRust atualmente, e 
 * quando isso foi registrado. A quantidade de itens é levada pelo o
 * comprimento da 'lista'. */
pub struct Historico { lista: Vec<LibRust>, selo: SystemTime }

/* == === === === === === === === === === === === === === === ==== == === ===
 *                               Histórico 
 * == === === === === === === === === === === === === === === ==== === === */
impl Historico {
   /// Método construtor do objeto.
   pub fn gera() -> Self {
      Self {
         lista: Historico::filtragem(),
         selo: SystemTime::now()
      }
   }
}

impl Display for Historico {
   fn fmt(&self, fmt:&mut Formatter<'_>) -> ResultFmt { 
      let inicio = SystemTime::UNIX_EPOCH;
      let decorrido = self.selo.duration_since(inicio).unwrap();
      let seg = decorrido.as_secs();

      write!(fmt, "Histórico({} pacotes; {}seg)", self.lista.len(), seg)
   }
}

impl PartialEq for Historico {
   fn eq(&self, outro: &Self) -> bool { 
      let a = &self.lista;
      let b = &outro.lista;

      /* Verifica se ambos tem a mesma quantia de itens, e se todos eles,
       * de cada lista, corresponde com algum da outra. */
      a.len() == b.len() && ambas_listas_com_mesmos_itens(a, b)
   }
}

impl Debug for Historico {
   fn fmt(&self, fmt:&mut Formatter<'_>) -> ResultFmt { 
      let ultimo = self.lista.len() - 2;
      let a = &self.lista[0];
      let b = &self.lista[ultimo];

      fmt.debug_list().entry(a).entry(b).entry(&self.selo).finish() 
   }
}

impl Serializador for Historico 
{
   fn serializa(&self) -> Vec<u8> {
      let mut output = Vec::<u8>::new();
      let quantia: u16 = self.lista.len() as u16; 
      let bytes = quantia.to_le_bytes();
      let partida = SystemTime::UNIX_EPOCH;
      let seg = self.selo.duration_since(partida).unwrap();
      let timestamp = seg.as_secs();

      /*   A ordem de arranjo dos bytes que a serialização produzirá é a 
       * seguinte: um inteiro sem sinal de 2 bytes, dizendo a quantidade de
       * itens; os itens em sí, estes os bytes são variável, más, com 
       * conhecimento da quantidade deles, que os bytes iniciais indicam,
       * fica fácil decodificar os demais; os últimos 8 bytes significam
       * o timestamp de quando foi feito isso. */
      output.extend(bytes.as_slice());
      self.lista.iter().for_each(|x| output.extend(x.serializa()));
      output.extend(timestamp.to_le_bytes().as_slice());
      output
   }

   fn deserializa(dados: Vec<u8>) -> Self {
      let mut fila = VecDeque::from(dados);
      let mut aux = Vec::<u8>::new();
      let quantidade;
      let mut lista: Vec::<LibRust>;
      let segundos: u64;
      let selo: SystemTime;

      /* Os dois bytes iniciais representam um inteiro sem sinal de 16-bits
       * que informa a quantidade de itens do tipo 'LibRust' na lista 
       * interna do 'Histórico'. */
      quantidade = decodifica_quantidade_de_itens_librust(&mut fila);
      lista = Vec::with_capacity(quantidade as usize);

      /* Agora parte que decodifica os itens 'LibRust', que há numa quantia
       * variável, porém, já sabemos a quantia, esta que foi obtida na 
       * primeira parte acima. */
      for _ in 1..=quantidade {
         extrai_bytes_de_um_item_librust(&mut fila, &mut aux);
         lista.push(LibRust::deserializa(aux.clone())); 
         // Limpeza precisa para os bytes de outros. 
         aux.clear();
      }

      // Já os últimos bytes, são referentes ao 'timestamp'.
      segundos = decodifica_selo_dos_bytes(&mut fila); 
      selo = descobre_system_time_dado_seu_timet(segundos);

      Historico { lista, selo }
   }
}

/* == === === === === === === === === === === === === === === ==== == === ===
 *                        Pacote Cache 
 * == === === === === === === === === === === === === === === ==== === === */
impl LibRust {
   pub fn novo(nome: String, versao: String) -> Self
      { LibRust(nome, versao) }
}

impl PartialEq for LibRust {
   fn eq(&self, outro: &LibRust) -> bool
      { self.0 == outro.0 && self.1 == outro.1 }
}

impl Debug for LibRust {
   fn fmt(&self, fmt:&mut Formatter<'_>) -> ResultFmt 
      { fmt.debug_list().entry(&self.0).entry(&self.1).finish() }
}

impl Serializador for LibRust 
{
   fn serializa(&self) -> Vec<u8> 
   {
      let mut output = Vec::<u8>::with_capacity(50 + 20);
      let (qn, qv) = (self.0.len(), self.1.len());
      let comprimento_nome: u8 = qn.try_into().unwrap();
      let mut nome = self.0.clone().into_bytes();
      let mut versao = self.1.clone().into_bytes();
      let comprimento_versao: u8 = qv.try_into().unwrap();

      output.extend(comprimento_nome.to_le_bytes().as_slice());
      output.extend(nome.drain(..));
      output.extend(comprimento_versao.to_le_bytes().as_slice());
      output.extend(versao.drain(..));
      output
   }

   fn deserializa(input: Vec<u8>) -> Self
   {
      let mut input = VecDeque::from(input);
      let comprimento_nome = input.pop_front().unwrap() as usize;
      let comprimento_versao: u8;
      let mut auxiliar = Vec::<u8>::with_capacity(comprimento_nome);
      let (nome, versao): (String, String);
      
      for _ in 1..=comprimento_nome
         { auxiliar.push(input.pop_front().unwrap()); }

      comprimento_versao = input.pop_front().unwrap();
      nome = String::from_utf8(auxiliar).unwrap();
      auxiliar = Vec::<u8>::with_capacity(comprimento_versao as usize);

      for _ in 1..=comprimento_versao
         { auxiliar.push(input.pop_front().unwrap()); }
      versao = String::from_utf8(auxiliar).unwrap();

      Self::novo(nome, versao)
   }
}

/* == === === === === === === === === === === === === === === ==== == === ===
 *                   Todas Funções Auxiliares Privadas
 * == === === === === === === === === === === === === === === ==== === === */

/** Pega toda conteúdo contido no arquivo TOML de tal pacote. Ele não 
 * precisa do caminho do TOML, apenas o caminho até o pacote. */
fn le_conteudo_do_cargo_toml(pkg: &Path) -> String {
   let caminho = pkg.join("Cargo.toml");
   let mut arquivo = File::open(caminho).unwrap();
   let mut conteudo = String::with_capacity(1000);

   arquivo.read_to_string(&mut conteudo).unwrap();
   conteudo
}

/** Reprocessa string, então tira qualquer aspas e espaços brancos que
 * tenham ficado no processo de extração de fato. */
fn retira_aspas(input: String) -> String {
   let n = input.len() + 2;
   let mut output = String::with_capacity(n);

   for unidade in input.chars() { 
      if unidade != '\"' 
         { output.push(unidade);  }
   }
   output.trim_start().to_string()
}

impl LibRust {
   /** Ler o arquivo TOML do pacote, então extrai seu Pacote, que não é nada
    * mais que seu nome e sua versão. */
   fn extrai(pkg: &Path) -> Option<LibRust> {
      let conteudo = le_conteudo_do_cargo_toml(pkg);
      let mut checado = [false, false];
      let mut nome = String::from("");
      let mut versao = String::from("");
      /* Apenas 'version' e 'name' depois de '[package]' são de fato entradas
       * válidas. Por isso, a razão desta chave. */
      let mut permissao_cedida = false; 

      for linha in conteudo.lines() 
      {
         /* Apenas começa processamento de fato, depois da tag 'package'. */
         if linha.contains("[package]")
            { permissao_cedida = true; }

         if permissao_cedida {
            if linha.contains("name") && linha.contains("=") {
               let marco = linha.find("=").unwrap(); 
               let indice = marco + 1;

               nome.push_str(linha.get(indice..).unwrap());
               nome = retira_aspas(nome);
               checado[0] = true;
            } else {}

            if linha.contains(&"version") && linha.contains("=") &&
               !linha.contains("rust-version")
            {
               let marco = linha.find("=").unwrap(); 
               let indice = marco + 1;

               versao.push_str(linha.get(indice..).unwrap());
               versao = retira_aspas(versao);
               checado[1] = true;
            } else {}

            if checado.iter().all(|arg| *arg) 
               { return Some(LibRust::novo(nome, versao)); }
         }
      }
      None
   }
}

impl Historico {
   fn filtragem() -> Vec<LibRust> {
      todos_diretorios_fontes().unwrap().into_iter()
      .map(|x| LibRust::extrai(&x))
      .filter(|a| a.is_some())
      .map(|x| x.unwrap())
      .collect::<Vec<LibRust>>()
   }
}

/// Decodifica a quantidade de itens que a array pode significar.
fn decodifica_quantidade_de_itens_librust(input: &mut VecDeque<u8>) -> u16 {
   let buffer: [u8; 2] = [
      input.pop_front().unwrap(), 
      input.pop_front().unwrap()
   ];

   u16::from_le_bytes(buffer)
}

/** Decodifica quais bytes pertecem ao um objeto do tipo 'LibRust', então 
 * extrai eles. */
fn extrai_bytes_de_um_item_librust
 (input: &mut VecDeque<u8>, output: &mut Vec<u8>) 
{
   let quantia = input.pop_front().unwrap();

   output.push(quantia);
   for _ in 1..=quantia
      { output.push(input.pop_front().unwrap()); }

   let quantia = input.pop_front().unwrap();

   output.push(quantia);
   for _ in 1..=quantia
      { output.push(input.pop_front().unwrap()); }
}

/** Decodifica uma valor inteiro de 64-bits que representam os segundos
 * registrados, por acaso eles servem prá descobrir o 'timestamp'. */
fn decodifica_selo_dos_bytes(input: &mut VecDeque<u8>) -> u64 {
   let mut buffer = [0u8; 8];
   let mut cursor = 0;

   while let Some(byte) = input.pop_front() {
      buffer[cursor] = byte;
      cursor += 1;
   }

   u64::from_le_bytes(buffer)
}

/** Tenta computar o SystemTime que foi gerado dados os segundos(time_t). */
fn descobre_system_time_dado_seu_timet(segundos: u64) -> SystemTime {
   /* O algoritmo prá achar isso é: marca um 'time_point' neste instante,
    * converter prá segundos(time_t), subtrair com o valor que foi passado
    * como argumento. Feito isso, é achado o decorrido em segundos de
    * ambos(time_t), subtrai isso do atual 'time_point', então descobre-se
    * o que foi feito na hora do registro. 
    */
   const PARTIDA: SystemTime = SystemTime::UNIX_EPOCH;
   let agora = SystemTime::now();
   let tb = Duration::from_secs(segundos);
   let ta = agora.duration_since(PARTIDA).unwrap();

   agora.checked_sub(ta - tb).unwrap()
}

/// Verifica se a lista contém somete itens únicos.
fn sem_itens_librust_duplicados(input: &Vec<LibRust>) -> bool {
   let quantia = input.len();
   let ultimo = quantia - 1;

   for p in 0..=(ultimo - 1) {
      for k in (p + 1)..=ultimo {
         if input[k] == input[p]  { 
            return false; }
      }
   }
   // Presunção inicial é retornada.
   true
}

/** Compara os itens de cada lista de forma não linear, ou seja, mesmo que
 *  ambas não tenho a mesma orden dos itens. 
 *
 * Obs.: Tal algoritmo, sempre supos que os argumentos passados tem o mesmos
 * tamanhos. Se não forem, o algoritmo fica indeterministico, que pode causar
 * erro de indexação, ou se funcionar, provavelmente dar uma resposta 
 * errada.*/
fn ambas_listas_com_mesmos_itens
  (lista_a: &Vec<LibRust>, lista_b: &Vec<LibRust>) -> bool
{
   /* Você assumir por um momento que, pode ser que eles não tenham a mesma
    * organização de itens, ou seja, que estejam na mesma ordem. Portanto,
    * minha proposta de igualdade é que tenham a mesma quantidade de itens
    * iguais. Claro que prá isso, é necessário que cada lista não tenha 
    * itens duplicados. */
   let mut contagem = 0;

   /* Nenhuma das listas pode ter elementos duplicados. */
   if !sem_itens_librust_duplicados(&lista_a) ||
      !sem_itens_librust_duplicados(&lista_b)
         { return false; }

   for a in lista_a {
      for b in lista_b {
         if a == b
            { contagem += 1; }
      }
   }

   /* Se houver contado a mesma quantia de itens em cada lista, então 
    * eles contém os mesmos itens em cada. */
   contagem == lista_b.len()
}


#[cfg(test)]
#[allow(non_snake_case)]
mod tests {
   
   use super::*;
   use std::thread::{sleep};
   use std::path::{PathBuf};

   #[test]
   fn rascunho() {
      let instancia = Historico::gera();

      println!("{}", instancia);
   }

   #[ignore]
   #[test]
   fn como_criar_timestamp() {
      let mut clock = SystemTime::now();
      let inicio = SystemTime::UNIX_EPOCH;
      
      println!("{:?}", clock.duration_since(inicio).unwrap());
      sleep(Duration::from_millis(300));
      clock = SystemTime::now();
      println!("{:?}", clock.duration_since(inicio).unwrap());
   }

   fn amostra_aleatoria() -> PathBuf {
      let lista = todos_diretorios_fontes();

      lista.unwrap().pop().unwrap()
   }

   #[test]
   fn serializacao_do_librust() {
      let X = amostra_aleatoria(); 
      let input = LibRust::extrai(&X).unwrap();

      println!("{input:?}");
      let bytes = input.serializa();
      println!("{bytes:?}");
      let output = LibRust::deserializa(bytes);
      println!("{output:?}");

      assert_eq!(input, output);
   }

   #[test]
   fn igualdade_de_historicos() {
      let input_a = Historico::gera();
      let input_b = Historico::gera();

      assert_eq!(input_b, input_a);
   }

   #[test]
   fn serializacao_de_um_historico() {
      let input = Historico::gera();

      println!("{input:}");
      let bytes = input.serializa();
      println!("Serão gravados {} bytes.", bytes.len());
      println!("{bytes:?}");
      let output = Historico::deserializa(bytes);
      println!("{output:?}");

      assert_eq!(input, output);
   }
}
