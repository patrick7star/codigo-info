/*!
 * Obtém os arquivos mais recentes de um dado diretório. Quero dizer, aqueles
 * que foram modificados recentementes, ou em devio com a média geral.
 */
 use std::io::{self, Read};
 use std::path::{PathBuf, Path};
 use std::fs::{read_dir, OpenOptions};
 use std::time::{SystemTime, Duration};

type Caminhos = Vec<PathBuf>;
// Bytes referentes a arquivos ELF.
const MATCH: [u8; 4] = [0x7f, 0x45, 0x4c, 0x46];


/** Parte da raíz, e vasculha por arquivos regulares do sistema. Qualquer
 * outro tipo, será apenas ignorado, assim como diretórios. A lista de
 * todos arquivos encontrados será retornado. */
fn varredura_por_arquivos(raiz: &Path, coletador: &mut Caminhos)
  -> io::Result<()>
{
   if raiz.is_dir() {
      for entry in read_dir(raiz)?
      {
         let caminho = entry?.path();
         let _= varredura_por_arquivos(&caminho, coletador);
      }
   } else if raiz.is_file()
      { coletador.push(raiz.to_path_buf()); }

   Ok(())
}

/** Pega um arquivo, abre-o, então extrai sua assinatura(magic number).
 * Como é preciso verifica apenas com ELFs, arquivos com mais bytes de
 * assinaturas não são pegos todos eles, ou seja, foram truncados. */
fn bytes_iniciais(arquivo: &Path) -> io::Result<[u8; 4]>
{
   let mut config = OpenOptions::new();
   let mut output = [0x00_u8; 4];
   let mut file = config.read(true).open(arquivo)?;

   file.read_exact(&mut output[..])?;
   // Fecha arquivo, apenas liberando num novo escopo...
   Ok(output)
}

/** Pega uma lista de arquivos, e remove todos que forem ELF(binários).
 * A lista é então retornada novamente. */
fn filtragem_de_executaveis(mut entrada: Caminhos) -> Caminhos
{
   let n = entrada.len();
   let mut saida = Caminhos::with_capacity(n);

   for caminho in entrada.drain(..) {
      if let Ok(bytes) = bytes_iniciais(&caminho)
      {
         if bytes != MATCH
            { saida.push(caminho); }
      }
   }
   saida
}

extern crate utilitarios;

use utilitarios::legivel::{tempo_legivel_duration};

/** Realiza uma filtragem, dado o período decorrido de cada arquivo. */
fn filtra_por_tempo_modificado(mut lista: Caminhos, tempo: Duration)
  -> Caminhos
{
   let mut output = Caminhos::with_capacity(lista.len());
   let momento = SystemTime::now();

   for item in lista.drain(..) {
      let metadados = item.metadata().unwrap();
      let selo = metadados.modified().unwrap();
      let decorrido = momento.duration_since(selo).unwrap();

      if decorrido < tempo
         { output.push(item); }

      if cfg!(debug_assertions) {
         println!("{:?} ~ {:?}",
            tempo_legivel_duration(decorrido, true),
            tempo_legivel_duration(tempo, true)
         );
      }
   }

   output.shrink_to_fit();
   output
}

trait ExtensaoDuracao {
   fn from_minutes(unidades: u64) -> Self;
   fn from_hours(unidades: u64) -> Self;
   fn from_days(unidades: u64) -> Self;
   fn from_weeks(unidades: u64) -> Self;
   fn from_months(unidades: u64) -> Self;
}

#[allow(unstable_name_collisions)]
impl ExtensaoDuracao for Duration {
   fn from_minutes(unidades: u64) -> Self
      { Duration::from_secs(unidades * 60) }

   fn from_hours(unidades: u64) -> Self
      { Duration::from_minutes(unidades * 60) }

   fn from_days(unidades: u64) -> Self
      { Duration::from_hours(unidades * 24) }

   fn from_weeks(unidades: u64) -> Self
      { Duration::from_days(unidades * 7) }

   fn from_months(unidades: u64) -> Self
      { Duration::from_weeks(unidades * 4) }
}

pub fn varredura_de_arquivos_por_periodo(raiz: &Path, periodo: Duration)
  -> Caminhos
{
   let mut output = Caminhos::new();
   let result: io::Result<()>;

   result = varredura_por_arquivos(raiz, &mut output);
   output = filtragem_de_executaveis(output);
   output = filtra_por_tempo_modificado(output, periodo);

   output
}

use std::ffi::{c_char, c_ulonglong};
#[repr(C)]
pub struct ListCaminhos
   { list: *const *const c_char, total: c_ulonglong }


#[cfg(test)]
mod tests {
   use super::*;

   #[test]
   fn funcao_de_varredura_output() {
      let mut coletador = Vec::<PathBuf>::new();
      let root = Path::new(".");

      let _= varredura_por_arquivos(&root, &mut coletador).unwrap();
      println!("{coletador:#?}");
   }

   #[test]
   fn leitura_de_bytes_dos_arquivos() {
      let mut coletador = Vec::<PathBuf>::new();
      let root = Path::new(".");
      let _= varredura_por_arquivos(&root, &mut coletador);

      for item in coletador {
         let fmt = item.display();

         if let Ok(bytes) = bytes_iniciais(&item) {
            if MATCH == bytes
               { println!("{bytes:?} {} \u{2705}", fmt); }
            else
               { println!("{bytes:?} {} \u{1f6ab}", fmt); }
         }
      }
   }

   #[test]
   fn filtragem_de_binarios_da_lista() {
      let mut entrada = Vec::<PathBuf>::new();
      let root = Path::new(".");
      let _= varredura_por_arquivos(&root, &mut entrada);
      let antes = entrada.len();
      let saida = filtragem_de_executaveis(entrada);
      let depois = saida.len();

      println!("Antes({}) > Depois({})", antes, depois);
      assert!(antes > depois);
   }

   fn processa_arquivos_deste_diretorio() -> Caminhos {
      let mut entrada = Vec::<PathBuf>::new();
      let root = Path::new(".");
      let _= varredura_por_arquivos(&root, &mut entrada);

      entrada
   }

   #[test]
   fn filtragem_baseado_no_tempo() {
      let minutos = Duration::from_hours(20);
      let entrada = processa_arquivos_deste_diretorio();
      let antes = entrada.len();
      let mut saida = filtra_por_tempo_modificado(entrada, minutos);
      let depois = saida.len();

      println!("Antes({}) > Depois({})", antes, depois);
      for item in saida.drain(..)
         { println!("{}", item.display()); }
      assert!(antes > depois);
   }

   #[allow(unstable_name_collisions)]
   #[test]
   fn extensao_das_duracoes() {
      let entradas = [
         Duration::from_minutes(45), Duration::from_hours(1),
         Duration::from_days(3), Duration::from_weeks(2),
         Duration::from_months(7)
      ];
      let saidas = [
         Duration::from_secs(45 * 60), Duration::from_secs(3600),
         Duration::from_secs(3 * 24 * 3600),
         Duration::from_secs(2 * 7 * 24 * 3600),
         Duration::from_secs(7 * 30 * 24 * 3600),
      ];

      for (a, b) in entradas.iter().zip(saidas.iter())
      {
         let p = b.as_secs_f32() / a.as_secs_f32();
         println!("{:>20?} ~ {:0.2}%", b, p * 100.0);
         // Erro com tolerância de no mínimo 10%.
         assert!((p - 0.99) <= 0.10);
      }
   }

   #[allow(non_snake_case)]
   #[test]
   fn VarreduraDeArquivosPorPeriodo() {
      let a = Path::new(".");
      let b = Duration::from_days(2);
      let result = varredura_de_arquivos_por_periodo(a, b);

      println!("{:?}", result);
   }
}
