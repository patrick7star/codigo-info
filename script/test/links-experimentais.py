"""
  Criar linques para experimentos dos algoritmos de ordenação. Eles também
 serão facilmente excluíveis.
"""
from pathlib import (Path)
import os, threading
from time import (time)
from random import (choice, randint)
from shutil import (move as move_file)


def binario_experimental() -> Path:
   "Arquivo binário zerado com contagem até overflow o byte em 2^10 + 1 bytes."
   CAMINHO = Path("./ExperimentaL")
   contador = 0

   if Path(CAMINHO).exists():
      raise FileExistsError("Só é possível criar apenas um!")

   with open(CAMINHO, "wb") as arquivo:
      for _ in range(1025):
         valor = contador % 256
         byte = valor.to_bytes(length=1, byteorder='little')
         arquivo.write(byte)
         contador += 1

   if Path(CAMINHO).exists():
      print("Binário '{}' criado com sucesso.".format(CAMINHO.name))
   return CAMINHO

def monitor(valores: list[int]):
   (a, r) = valores
   total = a + r
   final = time()

   if (final - monitor.INICIO) > 0.400:
      print("\rCaminhos acumulados %-4d/%4d."%(a, total), end='')
      monitor.INICIO = time()
monitor.INICIO = time()

def selecao_de_diretorio() -> list[Path]:
   RAIZ = Path(os.getenv("CCODES"))
   POSSIBILIDADES_WIN = os.walk(RAIZ.parent)
   RAIZ = Path(os.getenv("HOME"))
   POSSIBILIDADES_WSL = os.walk(RAIZ)
   apuracao = [0, 0]
   Output = set([])

   for tupla in POSSIBILIDADES_WIN:
      if tupla[0] is not []:
         Output.add(Path(tupla[0]))
         apuracao[0] += 1
      else:
         apuracao[1] += 1
      monitor(apuracao)
   for tupla in POSSIBILIDADES_WSL:
      if tupla[0] is not []:
         Output.add(Path(tupla[0]))
         apuracao[0] += 1
      else:
         apuracao[1] += 1
      monitor(apuracao)
   return Output

class SelecaoDeVariadoDiretorios:
   def __init__(self, LIMITE:int = 37) -> None:
      self.LIMITE = LIMITE
      self.HOME_LINUX = "/home"
      self.HOME_WIN = "/mnt/c"
      self.distribuicao = {
         # Caminho do Windows.
         0.20: 0,
         # Caminho do Linux:
         0.75: 0,
         # Caminho oculto do Linux:
         0.05: 0
      }

   def __str__(self) -> str:
      distribuicao = self.distribuicao
      n = sum(distribuicao.values())
      percent = (lambda K: distribuicao[K] / (n + 1) * 100.0)

      return ("""
      \rDistribuição final(N: {0}):
      \r   Windows[{7}]: {1:0.1f}% | {2:0.1f}%
      \r   Linux[{8}]: {4:0.1f}% | {3:0.1f}%
      \r   Oculto[{9}]: {5:0.1f}% | {6:0.1f}%
         """.format(n,
            75, percent(0.75), 20, percent(0.20),
            5 , percent(0.05), distribuicao[0.75],
            distribuicao[0.20], distribuicao[0.05]
            )
      )

   def __call__(self) -> set[Path]:
      urna = selecao_de_diretorio()
      n = self.LIMITE
      Out = set([])
      distribuicao = self.distribuicao

      while len(Out) < n:
         try:
            remocao = urna.pop()
         except KeyError:
            print("Sem elementos na 'urna'!")
            break

         # Critério de distribuição dos percentuais.
         if self.diretorio_windows(remocao) and self.atingiu_limite(0.20, n):
            distribuicao[0.20] += 1
            Out.add(remocao)
         elif self.diretorio_linux(remocao) and self.atingiu_limite(0.75, n):
            distribuicao[0.75] += 1
            Out.add(remocao)
         elif self.diretorio_oculto(remocao) and self.atingiu_limite(0.05, n):
            distribuicao[0.05] += 1
            Out.add(remocao)

      return Out

   def atingiu_limite(self, chave: float, n: int) -> bool:
      return (self.distribuicao[chave] / n) < chave

   def diretorio_oculto(self, caminho: Path) -> bool:
      path = caminho
      caminho = str(caminho)
      # Proposições:
      e_um_caminho_linux = (caminho.startswith(self.HOME_LINUX))
      um_diretorio_oculto = (path.name[0] == ".")

      return e_um_caminho_linux and um_diretorio_oculto

   def diretorio_linux(self, caminho: Path) -> bool:
      path = caminho
      caminho = str(caminho)
      nao_tem_ponto_no_comeco = (not path.name.startswith("."))
      return caminho.startswith(self.HOME_LINUX) and nao_tem_ponto_no_comeco

   def diretorio_windows(self, caminho: Path) -> bool:
      caminho = str(caminho)
      return caminho.startswith(self.HOME_WIN)

class GeradorDePalavrasAleatoria:
   def __init__(self):
      PATHNAME = Path(os.getenv("CCODES"))
      PATHNAME = PATHNAME.parent.joinpath("english3.txt")
      TAMANHO = os.stat(PATHNAME).st_size
      MARGEM = 100

      self.intervalo = (MARGEM, TAMANHO - MARGEM)
      self.stream = open(PATHNAME, "rt")

   def __call__(self) -> str:
      """
      O algoritmo sortea uma posição possível no stream aberto, então itera
      atravéz dele até que ache o começo e o fim(quebras de linhas). A palavra
      própria dele, provavelmente será a escolhida.
      """
      arquivo = self.stream
      (a, b) = self.intervalo
      X = randint(a, b)
      arquivo.seek(X, os.SEEK_SET)

      byte = arquivo.read(1)
      bytes = []

      # Encontra primeira quebra de linha.
      while byte != '\n':
         byte = arquivo.read(1)

      # Encontra segunda quebra de linha, enquanto adiciona bytes/letras lidas.
      byte = arquivo.read(1)

      while byte != '\n':
         bytes.append(byte)
         byte = arquivo.read(1)

      return ''.join(bytes)

   def __del__(self):
      self.stream.close()

def algoritmo_de_lincamento_de_binario_movel() -> None:
   """
   O algoritmo consiste em criar um binário, e mover ele sobre uma seleção de
   diretórios internos, sobre cada um, criar um linque no repositório de
   linques. Claro que tudo isso fará com que tais linques fiquem quebrados.
   """
   amostra = binario_experimental()
   origem = Path().absolute()
   manifesto = open("manifesto.txt", "wt")
   gerador = GeradorDePalavrasAleatoria()
   REPO = Path(os.getenv("LINKS"))
   selecao = SelecaoDeVariadoDiretorios(45)

   def desloca(destino: Path) -> None:
      "Realiza o deslocamento, registro do novo diretório, também acessa tal diretório, além de gravar na variável 'amostra' o novo caminho do arquivo movido."
      nonlocal amostra, manifesto

      novo = move_file(amostra, destino)
      amostra = Path(novo).absolute()
      amostrastr = str(destino) + '\n'

      # Muda para novo local e registra ele também.
      os.chdir(destino)
      manifesto.write(amostrastr)

   def linque_target() -> Path:
      "Cria o linque no repositório com um nome razoável."
      nonlocal REPO
      return REPO.joinpath(gerador())

   for destino in selecao():
      atual = os.getcwd()
      target = linque_target()

      os.symlink(amostra, target)
      desloca(destino)

   else:
      # Volta o arquivo prá origem onde ele foi criado.
      desloca(origem)

if __name__ == "__main__":
   algoritmo_de_lincamento_de_binario_movel()

from unittest import (skip, skipUnless, TestCase)
from io import (TextIOBase)
import string, subprocess, sys
from queue import (SimpleQueue)

@skipUnless(os.getenv("IGNORE"), "Não bagunçar com os arquivos locais!")
class TravessiaDiretoriosSelecionados(TestCase):
   def setUp(self):
      self.exe = binario_experimental()
      self.manifesto = open("manifesto.txt", "wt")
      self.origem = Path().absolute()

   def tearDown(self):
      nao_removido = self.exe.exists()
      self.manifesto.close()
      manifesto = Path("./manifesto.txt")

      print("Binário ainda disponível? {}".format(nao_removido))
      print("Manifesto existe? {}".format(manifesto.exists()))

      manifesto.unlink()
      self.exe.unlink()
      self.assertFalse(self.exe.exists())
      self.assertFalse(manifesto.exists())

   def desloca(self, destino):
      novo = move_file(self.exe, destino)
      self.exe = Path(novo)
      selfexestr = str(destino) + '\n'

      # Muda para novo local e registra ele também.
      os.chdir(destino)
      self.manifesto.write(selfexestr)

   def runTest(self):
      selecao = SelecaoDeVariadoDiretorios(60)

      for destino in selecao():
         self.desloca(destino)
         print("Atual: %s" % os.getcwd())
      else:
         self.desloca(self.origem)

class SorteadorDePalavras(TestCase):
   def runTest(self):
      motor = GeradorDePalavrasAleatoria()

      for k in range(20):
         if (k + 1) % 7 == 0:
            print('')
         print(motor(), end=' -- ')
      else:
         print('')

class CriacaoDeTargets(SorteadorDePalavras):
   def runTest(self):
      gerador = SelecaoDeVariadoDiretorios(45)
      fila = SimpleQueue()
      selecao = gerador()
      gerador2 = GeradorDePalavrasAleatoria()

      for _ in range(len(selecao)):
         palavra = gerador2()
         remocao = selecao.pop()
         print(remocao.joinpath(palavra))
         fila.put(remocao)

class SelecaoDePoucosDiretorios(TestCase):
   """
   Buscando um algoritmo para a seleção de alguns tipos de aleatórios
   para todos diretórios varridos.
   """
   def diretorio_oculto(self, caminho: Path) -> bool:
      path = caminho
      caminho = str(caminho)

      return (
         caminho.startswith("/home") and 
         path.name[0] == "."
         )

   def diretorio_linux(self, caminho: Path) -> bool:
      path = caminho
      caminho = str(caminho)
      return caminho.startswith("/home") and (not path.name.startswith("."))

   def diretorio_windows(self, caminho: Path) -> bool:
      caminho = str(caminho)
      return caminho.startswith("/mnt/c")
   
   def visualizacao_resultado(self, distribuicao: dict) -> None:
      n = sum(distribuicao.values())
      percent = (lambda K: distribuicao[K] / n * 100.0)
      print("""
      \rDistribuição final(N: {0}):
      \r   Windows[{7}]: {1:0.1f}% | {2:0.1f}%
      \r   Linux[{8}]: {4:0.1f}% | {3:0.1f}%
      \r   Oculto[{9}]: {5:0.1f}% | {6:0.1f}%
         """.format(n,
            75, percent(0.75), 20, percent(0.20),
            5 , percent(0.05), distribuicao[0.75], 
            distribuicao[0.20], distribuicao[0.05] 
            )
      )

   def escolhe_aleatoriamente(self, urna: set[Path], n: int) -> set[Path]:
      assert isinstance(urna, set)
      assert isinstance(n, int)
      assert n <= len(urna)

      if __debug__:
         print("\nTamanho da Urna: {}".format(len(urna)))
         print("N: {}".format(n))

      Out = set([])
      distribuicao = {
         # Caminho do Windows.
         0.20: 0,
         # Caminho do Linux:
         0.75: 0,
         # Caminho oculto do Linux:
         0.05: 0
      }

      def percentual_menor(chave: float) -> bool:
         return (distribuicao[chave] / n) < chave

      while len(Out) < n:
         try:
            remocao = urna.pop()
         except KeyError:
            print("Sem elementos na 'urna'!")
            break

         # Critério de distribuição dos percentuais.
         if self.diretorio_windows(remocao) and percentual_menor(0.20):
            distribuicao[0.20] += 1
            Out.add(remocao)
         elif self.diretorio_linux(remocao) and percentual_menor(0.75):
            distribuicao[0.75] += 1
            Out.add(remocao)
         elif self.diretorio_oculto(remocao) and percentual_menor(0.05):
            distribuicao[0.05] += 1
            Out.add(remocao)

      self.visualizacao_resultado(distribuicao)
      self.assertTrue(n <= len(Out))
      return Out

   def runTest(self):
      print("Seleção de poucos diretórios:")

      LIMITE = 47
      SELECAO = selecao_de_diretorio()
      SUBCONJUNTO = self.escolhe_aleatoriamente(SELECAO, LIMITE)
      QUANTIA = len(SUBCONJUNTO)
      
      for caminho in SUBCONJUNTO:
         print('-', caminho)
      print("Foram {} ao todo.".format(QUANTIA))

class AnalisaMetodoPopDoSetII(TestCase):
   def runTest(self):
      alfabeto = set(string.ascii_uppercase) 
      sacola_a = set([])
      sacola_b = sacola_a.copy()
      mediana = len(alfabeto) // 2
      letra_mediana = string.ascii_uppercase[mediana]
      MEIO = (ord('A') + ord('Z')) // 2

      for item in alfabeto:
         if ord(item) > MEIO:
            sacola_a.add(item)
         else:
            sacola_b.add(item)

      print("Sacola(a):", sacola_a)
      print("Sacola(b):", sacola_b)

class AnalisaMetodoPopDoSetI(TestCase):
   def runTest(self):
      alfabeto = set(string.ascii_uppercase) 
      novo = set([])
      QUANTIA = len(alfabeto)
      contador = 0

      print("Antes:", alfabeto)
      for _ in range(QUANTIA):
         X = alfabeto.pop()

         novo.add(X)
         alfabeto.add(X)
      print("Depois:", novo)

      self.assertEqual(frozenset(novo), frozenset(alfabeto))

      for _ in range(QUANTIA // 3):
         X = alfabeto.pop()

         print(X, end=' ')
         alfabeto.add(X)
      else:
         print("")

@skipUnless(os.getenv("IGNORE"), "Mexendo com multiprocesso!")
class AnalisaMetodoPopDoSet(TestCase):
   """
   Nota: necessário variável ATIVADO_AMPDS para rodar corretamente este 
         teste unitário.
   """
   def extrai_necessario(self, stream: TextIOBase) -> str:
      linhas = stream.read().decode(encoding="utf8").split('\n')
      
      for linha in linhas:
         if '{' in linha and '}' and linha:
            return linha

   def runTest(self):

      alfabeto = set(string.ascii_uppercase) 
      pid = os.getpid()
      quantia = len(os.environ)

      if os.getenv("ATIVADO_AMPDS") is not None:
         print(
           alfabeto,
           "[PID %d] Quantia de variáveis definidas: %d\n "% (pid, quantia),
           "Hash e ordem dos mesmos elementos parece mudar a cada execução?\n",
            sep='\n'
         )
      else:
         print(
           alfabeto,
           "[PID %d] Quantia de variáveis definidas: %d\n "% (pid, quantia),
            sep='\n'
         )

      # Apenas ativa este trecho se a variável 'ATIVADO_AMPDS' for definida.
      if os.getenv("ATIVADO_AMPDS") is not None:
         for _ in range(5):
            arquivo = "%s.AnalisaMetodoPopDoSet" % __name__
            cmdline = [sys.executable, "-m", "unittest", arquivo],

            try:
               cmd = subprocess.Popen(
                  [sys.executable, "-m", "unittest", arquivo], 
                  env={}, stdout=subprocess.PIPE
               )
               cmd.wait()
               print(self.extrai_necessario(cmd.stdout))
               cmd.stdout.close()

            except ModuleNotFoundError:
               print("Arquivo:", arquivo)
            finally: pass

      if os.getenv("ATIVADO_AMPDS") is not None:
         print("Se ver que ele cria diferentes conjuntos toda execução.")

@skipUnless(os.getenv("IGNORE"), "Saída bem extensa!")
class AmostraDeTodosDiretorios(TestCase):
   def runTest(self):
      print("Diretórios selecionados para dispersão:")
      SELECAO_AUTOMATICA = selecao_de_diretorio()
      QUANTIA = len(SELECAO_AUTOMATICA)
      
      for caminho in SELECAO_AUTOMATICA:
         print('-', caminho)
      print("Foram {} ao todo.".format(QUANTIA))

@skip("Teste prá verificar função, muito verborragico.")
class ExperimentandoWalk(TestCase):
   def runTest(self):
      for item in os.walk(os.getenv("CCODES")):
         print(item)

class VerificandoCriacaoDeBinario(TestCase):
   def setUp(self):
      self.result = None

   def tearDown(self):
      self.assertTrue(self.result.exists())
      self.result.unlink()
      self.assertFalse(self.result.exists())
      
   def runTest(self):
      self.result = binario_experimental()
      print("caminho do binário '{}'".format(self.result))
