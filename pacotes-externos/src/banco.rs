use std::fs::{OpenOptions};
use std::io::{self, Write, Read};
use std::collections::{VecDeque};
use crate::historico::{Historico, Serializador};
use std::ffi::{OsStr};
use std::env::{current_exe};
use std::path::{PathBuf};

/* Nota: se este código foi copiado prá algum outro projeto, diferente do 
 * original dele, a 'cobrinha-classica', provavelmente você precisa mudar 
 * a constante literal 'string', com o nome do projeto original, prá o nome
 * do diretório que está o código do projeto. */
const PROJETO: &'static str = "codigo-info";
const BANCO:&'static str = {
   /* Uma versão debug e uma "release", porque em testes unitários eu costumo
    * a apagar a versao debug modificada. Se fizesse isso com uma só versão, 
    * teria que desabilitar tais testes, pois modificaria versões não já em
    * execução. */
   if cfg!(debug_assertions)
      { "./banco-debug.dat" }
   else
      { "./banco.dat" }
};

type Particao           = Vec<u8>;
type FilaDeParticoes    = VecDeque<Particao>;
type FilaDeHistoricos   = VecDeque<Historico>;
type FilaDeBytes        = VecDeque<u8>;


/** Grava um 'histórico' passado na memória não volátil. O retorno, em bytes,
 * de quanto foi gravado é retornado se algum erro não acontecer. */
pub fn registra_um_historico(obj: Historico) -> io::Result<usize> {
   let caminho = caminho_do_arquivo_bd();
   let mut banco = {
      OpenOptions::new()
      .create(true).append(true)
      .open(&caminho)?
   };
   let dados = obj.serializa();
   let total: u16 = dados.len().try_into().unwrap();
   // Bytes do valor total de bytes. Sim, ele também tem que ser decodificado
   // e então registrado.
   let bytes = total.to_le_bytes();
       
   // Primeiro os bytes que 
   banco.write(bytes.as_slice()).unwrap();
   banco.write(dados.as_slice())
}

pub fn carrega_historicos() -> io::Result<FilaDeHistoricos> {
   // Capacidade incial:
   const N: usize = 1e3 as usize;

   let mut output = FilaDeHistoricos::new();
   let caminho = caminho_do_arquivo_bd();
   let mut banco = OpenOptions::new().read(true).open(&caminho)?;
   let mut dados = Vec::<u8>::with_capacity(N);

   banco.read_to_end(&mut dados)?;
   // Agora, a parte do processamento em lote ...
   for lote in particiona_bytes_em_bytes_de_historicos(dados)
      { output.push_back(Historico::deserializa(lote)); }
   Ok(output)
}

/* == === === === === === === === === === === === === === === ==== == === ===
 *                   Todas Funções Auxiliares Privadas
 * == === === === === === === === === === === === === === === ==== === === */

/** Remove e retira os bytes iniciais que indicam quantos byte abaixo fazem
 * parte de uma instância de dados. Tais bytes são removidos. O retorno
 * são somente os bytes que compoem a instância. */
fn le_inteiro_dos_bytes_iniciais(input: &mut FilaDeBytes) -> u16 {
   let mut output = [0u8, 0u8];

   output[0] = input.pop_front().unwrap();
   output[1] = input.pop_front().unwrap();

   u16::from_le_bytes(output)
}

/** Dado uma fila de sucessivos bytes, ele arranca os bytes apenas relevante
 * a primeira instãncia da interação. */
fn extrai_bytes_da_proxima_instancia (input: &mut FilaDeBytes) 
  -> FilaDeBytes
{
   let mut saida = FilaDeBytes::new();
   let mut total = le_inteiro_dos_bytes_iniciais(input); 
   let mut byte: u8;

   while total > 0 {
      byte = input.pop_front().unwrap();
      saida.push_back(byte);
      total -= 1;
   }
   saida
}

fn enfileira(input: Vec<u8>) -> FilaDeBytes {
   let n = input.len();
   let mut output = FilaDeBytes::with_capacity(n);

   input.into_iter()
   .for_each(|b| output.push_back(b));
   output
}

fn enlista(mut input: FilaDeBytes) -> Particao {
   let n = input.len();
   let mut output = Vec::<u8>::with_capacity(n);

   while let Some(byte) = input.pop_front()
      { output.push(byte); }
   output
}

fn particiona_bytes_em_bytes_de_historicos(input: Vec<u8>) 
  -> FilaDeParticoes
{
   let mut output = FilaDeParticoes::new(); 
   let mut particao: FilaDeBytes;
   let mut fila = enfileira(input);

   assert!(!fila.is_empty());

   while !fila.is_empty() {
      particao = extrai_bytes_da_proxima_instancia(&mut fila);
      output.push_back(enlista(particao));
   }
   output
}

/// Complementa link ao executável à partir do caminho do executável ...
fn computa_caminho(caminho_str: &str) -> PathBuf {
   const NOME: &'static str = PROJETO;
   let barreira = Some(OsStr::new(NOME));

   /* O método novo, busca algo mais flexível. Ele pode capturar o caminho
    * do projeto baseado em qualquer profundidade dentro dele. */
   match current_exe() {
      Ok(mut executavel) => {
         while executavel.file_name() != barreira {
            executavel.pop();
         }
         executavel.push(caminho_str);
         executavel
      } Err(_) =>
         { panic!("não foi possível obter o caminho do executável!"); }
   }
}

/// Caminho até o arquivo com dados dos históricos registrados.
fn caminho_do_arquivo_bd() -> PathBuf 
   { computa_caminho(BANCO) }


#[cfg(test)]
mod tests {
   use super::*;
   use std::thread::{sleep};
   use std::time::{Duration};

   fn registro_realizados_sucessivamentes() {
      let pausa = Duration::from_secs(3);

      for _ in 1..=6 {
         let snap = Historico::gera();

         registra_um_historico(snap).unwrap();
         sleep(pausa);
         println!("Mais um registro realizado no 'mass storage'.");
      }
   }

   #[test]
   fn grava_um_historico_e_recarrega_novamente() {
      let input = Historico::gera();
      let _= registra_um_historico(input);
      let mut output = carrega_historicos().unwrap();
      let caminho = caminho_do_arquivo_bd();

      while let Some(partition) = output.pop_front()
         { println!("{:?}", partition); }

      std::fs::remove_file(&caminho).unwrap();
   }

   #[test]
   fn operacoes_de_registro_e_carregamento() {
      let _= registro_realizados_sucessivamentes();
      let mut output = carrega_historicos().unwrap();
      let caminho = caminho_do_arquivo_bd();

      while let Some(history) = output.pop_front()
         { println!("{:?}", history); }
      std::fs::remove_file(&caminho).unwrap();
   }

   #[test]
   fn diretorio_base() {
      println!("{}", computa_caminho("banco-debug.dat").display());
   }
}
