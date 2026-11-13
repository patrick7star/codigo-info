"""
   Ele pega o linque da minha página do GitHub, então faz uma varredura no seu
 código filtrando todos repositórios públicos lá. Ele também faz o download
 de tal repositório se requisitado.
"""
from pathlib import (Path)
from pycurl import (Curl, URL, WRITEDATA, FOLLOWLOCATION, RESPONSE_CODE)
from queue import (SimpleQueue as Queue)
from subprocess import (Popen, PIPE)


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

def filtra_todos_repositorios_disponiveis(In: Path) -> list[str]:
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

def extrai_nome_na_url_especifica(In: str) -> str:
   assert isinstance(In, str)

   marcador = In.rfind('/') + 1
   In = In[marcador:]

   return In.replace('-', ' ').removesuffix(".git").title()

def listagem_de_repositorios_disponiveis() -> list[str]:
   "Por enquanto este apenas processa o resultado de 'FTRD'."
   repo = realiza_download_do_html_com_repositorios()
   urls = filtra_todos_repositorios_disponiveis(repo)

   if __debug__:
      os.unlink(repo)

   return list(map(extrai_nome_na_url_especifica, urls))

def realiza_download(url: str) -> Path:
   "Realiza o download da url no repositório temporario(em '/tmp')."
   # Extrai o nome do pacote que será baixado, baseado na sua URL.
   posicao = url.rfind('/') + 1
   nome = url[posicao:].removesuffix(".git")
   diretorio = "/tmp/codigo-info-data"
   # Define onde colocar o que foi baixado.
   DESTINO = Path(diretorio).joinpath(nome)
   PROGRAMA = "/usr/bin/git"

   if __debug__:
      print("URL: '%s'" % url)

   handle = Popen([PROGRAMA, "clone", url, DESTINO], )
   handle.wait()

   if handle.returncode == 0:
      print("Foi baixado com sucesso.")

   return DESTINO

from unittest import (TestCase)
import os
from random import (randint, choice)
from shutil import(rmtree)

class BaixaTodosRepositoriosDaFonte(TestCase):
   def setUp(self):
      self.bolsa = []

   def tearDown(self):
      print("Tudo que foi baixado, e o caminho a ele:")

      for caminho in self.bolsa:
         print('\t\b\b\b-', caminho, end=' ')
         rmtree(caminho)
         print("excluido.")

   def runTest(self):
      raiz = realiza_download_do_html_com_repositorios()
      lista_de_repositorios = filtra_todos_repositorios_disponiveis(raiz)

      for url in lista_de_repositorios:
         self.bolsa.append(realiza_download(url))


class BaixaRepositorioNoDiretorioTemporario(TestCase):
   def runTest(self):
      raiz = realiza_download_do_html_com_repositorios()
      lista_de_repositorios = filtra_todos_repositorios_disponiveis(raiz)
      escolha = choice(lista_de_repositorios)

      print(realiza_download(escolha))

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

class ListagemSimplesDosRepositorios(TestCase):
   """
   Como ainda é apenas um wrapper de filtra_todos_repositorios_disponiveis,
   então retornarar apenas uma lista com os nomes capitlizados. O objetivo
   no futuro será fazer isso bem mais detalhado.
   """
   def runTest(self):
      output = listagem_de_repositorios_disponiveis()

      print("A listagem dos repositórios disponvéis no Git:")

      for item in output:
         print('\t-', item)
