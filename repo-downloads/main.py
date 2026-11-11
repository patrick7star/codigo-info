"""
   Ele pega o linque da minha página do GitHub, então faz uma varredura no seu
 código filtrando todos repositórios públicos lá. Ele também faz o download
 de tal repositório se requisitado.
"""
from pathlib import (Path)
from pycurl import (Curl, URL, WRITEDATA, FOLLOWLOCATION, RESPONSE_CODE)
from queue import (SimpleQueue as Queue)


def realiza_download_do_html_com_repositorios() -> Path:
   "Faz o download da página com os respectivos linques aos seus repositórios."
   url = "https://github.com/patrick7star?tab=repositories"
   PATHNAME = "data/git-repositories.html"

   with open(PATHNAME, "wb") as stream:
      curl = Curl()

      try:
         curl.setopt(URL, url)
         curl.setopt(WRITEDATA, stream)
         curl.setopt(FOLLOWLOCATION, True)
         curl.perform()

         status = curl.getinfo(RESPONSE_CODE)

         if status >= 400:
            raise RuntimeError("HTTP error: %s" % status)
      finally:
         curl.close()

   return Path(PATHNAME)

def extrai_apenas_o_endpoint(Input: str) -> str:
   assert isinstance(Input, str)

   Out = Input.split(' ')

   # Removendo espaços vázios:
   while '' in Out:
      Out.pop(0)

   # É o segundo item na lista. Más antes, é preciso também tirar um prefixo
   # dele referente a hyperlink. Também retira as aspas que tal trecho está
   # enclausurado.
   return Out[1].removeprefix("href=").strip('\"').rstrip('\"')

def filtra_todos_repositorios_disponiveis(In: Path) -> list[Path]:
   assert In.is_file()

   PROPRIETARIO = "patrick7star"
   fila = Queue()

   with open(In, "rt") as stream:
      for linha in stream:
         esta_fechado           = ('<' in linha) and ('>' in linha)
         tem_um_linque          = ("a href" in linha)
         dono_esta_correto      = (PROPRIETARIO in linha)
         identificador_proprio  = ("itemprop" in linha)

         if (esta_fechado and tem_um_linque and dono_esta_correto and
             identificador_proprio):
               fila.put(extrai_apenas_o_endpoint(linha))

   # Esta parte vamos complementar os endpoints obtidos com o restante da 
   # URL, e o sufixo do tipo de download, assim como URL gits oferecem para
   # download.
   output = []

   while (not fila.empty()):
      endpoint = fila.get()
      url = "https://www.github.com{}.git".format(endpoint)
      output.append(url)

   return output

def git_esta_instalado() -> bool:
   executavel = Path("/usr/bin/git")
   return executavel.exists() and executavel.is_file()

from unittest import (TestCase)
import os

class DownloadDoArquivoHTML(TestCase):
   def tearDown(self):
      self.assertTrue(self.path.exists())
      self.path.unlink()
      self.assertFalse(self.path.exists())
      print("Excluido com sucesso.")

   def runTest(self):
      pagina = realiza_download_do_html_com_repositorios()

      print("Local: '%s'" % str(pagina))
      print(os.stat(pagina))
      self.path = pagina

class FiltraListaDeRepositorios(TestCase):
   def tearDown(self):
      self.assertTrue(self.pagina.exists())
      self.pagina.unlink()
      self.assertFalse(self.pagina.exists())
      print("Excluido com sucesso.")

   def runTest(self):
      self.pagina = realiza_download_do_html_com_repositorios()

      print("Processando ...", end='\t')
      output = filtra_todos_repositorios_disponiveis(self.pagina)
      print("feito.")
      print("Listagem do resultado:")

      for item in output:
         print('\t\b\b', item)

class VerificaSeOGitEstaInstalado(TestCase):
   def runTest(self):
      resposta = git_esta_instalado()
      print("\nEstá o 'git' instalado? %s." % resposta)
