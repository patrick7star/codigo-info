/* cria link simbólico tanto para a versão em debug, quanto para o 
 * binário final. */
#[cfg(target_os="linux")]
use std::os::unix::fs::symlink;
#[cfg(target_os="windows")]
use std::os::windows::fs::{symlink_file as symlink};
use std::env::current_exe;
use std::ffi::{OsStr, c_char, CString};
use std::path::{PathBuf, Path, Component};
use std::env::{var, VarError};
use std::io;

// Nome do programa aqui.
const NOME_DO_PROGRAMA: &'static str = "cargo-listagem.exe";


/// Computa o caminho até o projeto, baseado que, tal executável deve está
/// no lugar comum onde uma simples compilação do Rust o faz se não for 
/// definida diferente. Portanto em algum subdiretório do diretório 'target'.
pub fn computa_caminho(caminho_str:&str) -> PathBuf {
   match current_exe() {
   // à partir do caminho do executável ...
      Ok(mut base) => {
         // remove executável do caminho.
         base.pop(); 
         // sai do subdiretório 'release'.
         base.pop(); 
         // sai do subdiretório 'target'.
         base.pop();
         // complementa com o caminho passado.
         base.push(caminho_str);
         return base;
      } Err(_) =>
         { panic!("não foi possível obter o caminho do executável!"); }
   }
}

pub fn linca_executaveis(nome_do_linque: &str) 
{
   match cria_linques_locais(nome_do_linque) {
      Ok(_) => 
         { println!("O linque local foi criado com sucesso."); }
      Err(erro) => match erro.kind() {
         io::ErrorKind::AlreadyExists => {
            if cfg!(debug_assertions)
               { println!("Já existe um linque local do 'modo debug'."); }
            else 
               { println!("Já existe um linque local."); }
         } _ =>
         // Demais erros ainda não tratados.
            { panic!("{}", erro); }
      }
   };

   match cria_linques_no_repositorio(nome_do_linque) {
      Ok(caminho) => { 
         assert!(caminho.exists()); 
         println!("Linque criado com sucesso em $LINKS."); 
      } Err(classificacao_do_erro) => { 
         match classificacao_do_erro.kind() {
            io::ErrorKind::AlreadyExists =>
               { println!("Já existe um linque em $LINKS."); }
            io::ErrorKind::Unsupported =>
               { println!("Sistema ou ambiente não suportado.");}
            _ =>
               { panic!("{}", classificacao_do_erro); }
         }
      } 
   }
}

/// Remove trechos até a base dada, se não houve uma, então retorna None.
fn retira_a_base(base: &str) -> Option<PathBuf>
{
   let mut absoluto = current_exe().unwrap();
   let correspodente = Some(OsStr::new(base));

   while absoluto.file_name() != correspodente
      { absoluto.pop(); }

   if absoluto.file_name().is_none()
      { return None; }
   Some(absoluto)
}

fn path_to_rawstr(caminho: &Path) -> *mut c_char
{
/* Transforma de um caminho nativo de Rust para uma raw-string pro C. */
   match caminho.to_str() 
   {
      Some(caminhostr) => {
         let copia = caminhostr.to_string();

         CString::new(copia).unwrap().into_raw()
      } None =>
         { std::ptr::null_mut::<c_char>() }
   }
}
#[repr(C)]
pub struct CaminhoRust { data: *mut c_char, desalocado: bool }

/** Isso está sendo feito aqui, já aproveitando a função que faz algo similar.
 * Também porque o Rust tem um bom manipulador de caminhos, diferente do C,
 * quen não possui nada na biblioteca padrão. */
#[no_mangle]
pub extern "C" fn computa_caminho_externo(pathname: *mut c_char) 
  -> CaminhoRust
{
   const NULO: *mut c_char = std::ptr::null_mut::<c_char>();
   const INVALIDO: CaminhoRust = CaminhoRust { data: NULO, desalocado: true };
   const BASE: &str = "codigo-info";
   let output = match retira_a_base(BASE) {
      Some(path) => path,
      None =>
         { return INVALIDO; }
   };
   let complemento = unsafe { CString::from_raw(pathname) };
   // Converte novamente para string.
   let caminho_string = complemento.into_string().unwrap();
   let caminho = output.join(Path::new(&caminho_string));

    // Retornando a CString como uma raw string.
    CaminhoRust { data: path_to_rawstr(&caminho), desalocado: false }
}

pub extern "C" fn libera_caminho_rust(obj: *mut CaminhoRust)
{
   unsafe {
      if !(*obj).desalocado { 
         let _= CString::from_raw((*obj).data);  
         (*obj).desalocado = true;

         println!("CaminhoRust liberado.");
      }
   }
}

fn cria_linques_no_repositorio(nome_do_linque: &str) -> io::Result<PathBuf> 
{
   let caminho_do_executavel = current_exe()?;
   let caminho_repositorio =  match var("LINKS") {
      Ok(data) => Ok(data),
      Err(tipo_de_erro) => 
      {
         let erro_a = io::ErrorKind::InvalidInput;
         let erro_b = io::ErrorKind::InvalidData;

         match tipo_de_erro {
            VarError::NotPresent => Err(erro_a),
            VarError::NotUnicode(_) => Err(erro_b)
         }
      }
   }?;
   let mut fonte = caminho_do_executavel;
   let destino = Path::new(&caminho_repositorio).join(nome_do_linque);
   let bate = Component::Normal(OsStr::new("release"));

   if destino.exists()
      { return Err(io::ErrorKind::AlreadyExists.into()); }

   // Verificação se estamos nos referindo apenas da parte 'release'.
   if fonte.components().any(|part| part == bate) 
   {
      // Corrigindo para atual caminho se for um linque simbólico.
      if fonte.is_symlink() 
         { fonte = fonte.read_link()?; }

      symlink(&fonte, &destino)?;
      Ok(destino)
   } else
      { Err(io::ErrorKind::Unsupported.into()) }
}

fn cria_linques_locais(nome_do_linque: &str) -> io::Result<PathBuf>
{
   let (fonte, destino): (PathBuf,PathBuf);

   // Seleção baseado no tipo de optimização na compilação:
   if cfg!(debug_assertions) 
   {
      let novo_nome = format!("{}-debug", nome_do_linque);
      let antigo_nome = format!("target/debug/{}", NOME_DO_PROGRAMA);

      // fonte = computa_caminho("target/debug/limpa_downloads");
      fonte = computa_caminho(&antigo_nome);
      destino = computa_caminho(&novo_nome);

   } else {
      let antigo_nome = format!("target/release/{}", NOME_DO_PROGRAMA);

      fonte = computa_caminho(&antigo_nome);
      destino = computa_caminho(nome_do_linque);
   }

   // Escolhe a criação do linque, baseado no tipo de execução aplicada.
   symlink(fonte, &destino)?;
   // Retorno do linque que acabou de ser criado.
   Ok(destino)
}

#[cfg(test)]
mod tests {
   use super::{CString, c_char, CaminhoRust, libera_caminho_rust};
   use std::mem::{transmute};

   #[test]
   fn visualizando_o_computa_caminho()
   {
      println!("{:?}", super::computa_caminho("nova_pasta"));
   }

   fn imprime_um_caminho_em_c(pathname: *const c_char)
   {
      let mut cursor: isize = 0;

      loop { unsafe {
         let ptr = pathname.offset(cursor);
         let byte: i8 = *ptr;
         let code = byte as u32;
         let caractere = char::from_u32_unchecked(code);

         if code == 0x00 
            { break; }
         print!("{}", caractere);
         cursor += 1;
      }}
      print!("\n");
   }

   #[test]
   fn simulacao_da_aplicacao_em_c()
   {
      let a = "data/info/saldo.txt";
      let b = CString::new(a).unwrap();
      let c = super::computa_caminho_externo(b.into_raw());
      let r = unsafe { transmute::<&CaminhoRust, *mut CaminhoRust>(&c)};

      print!("Caminho criado: ");
      imprime_um_caminho_em_c(c.data);
      libera_caminho_rust(r);
   }
}
