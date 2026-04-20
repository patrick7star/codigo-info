"""
   Teste de Integração

   Testa todas opções -- pelo menos as definidas até então -- em variádos 
 diretórios definidos no sistema de arquivo, dados mesmos argumentos. Isso
 tudo, pra ver se todas executam em: ambientes diferentes, e também mostram
 os mesmos resultados.
"""
from pathlib import (Path)
from random import (randint, choice)
from os import (getenv, getcwd)
from fractions import (Fraction)
from subprocess import (Popen, DEVNULL)


def explora_de_ate(raiz: Path, profundidade: Fraction, LIMITE: int,
  output: list[Path]):
   '''
     Pega um caminho, então vai entrando nele, e seus subdiretórios de 
   maneira totalmente aleatória. Quando alcança uma determinada profundidade
   a chamada acaba. Enquanto ele faz isso, vai listando todos os diretórios
   que adentrou na exploração.
   '''
   if profundidade >= LIMITE:
      return None

   for entrada in raiz.iterdir():
      # Apenas pega se for um diretório.
      if entrada.is_dir():
         output.append(entrada)
         profundidade += 1
         explora_de_ate(entrada, profundidade, LIMITE, output)

def numero_aleatorio(n: int) -> Fraction:
   return Fraction(randint(0, n))

def seleciona_diretorio_randomicamente(raiz: Path) -> Path:
   """
     Tenta selecionar um diretório randômico, dada a raíz como argumento. Se
   o diretório não tem nenhum sub, provalvamente uma exceção será lançada.
     Um teste empírico diz que, a taxa de sucesso, num determinado diretório,
   é aproximadamente uns 80%.
   """
   # Limite definido de forma arbitraria, por enquanto.
   LIMITE = numero_aleatorio(6)
   profundidade = Fraction(0)
   lista = []
   explora_de_ate(raiz, profundidade, LIMITE, lista)

   try:
      return choice(lista)
   except IndexError:
      raise LookupError("a raíz não tem subdiretórios pra vasculhar")
   except:
      raise Exception("um erro não previsto!")
   else:
      pass
   finally:
      pass

def verificacao_do_programa_pacotes() -> None:
   "Exceção é lançada caso não esteja nenhuma das demandas certa."
   base = getenv("LINKS")

   if base is None:
      raise SystemError("Variável 'LINKS' não está definida.")

   caminho = Path(base).joinpath("pacotes")

   if (not caminho.exists()):
      raise FileNotFoundError("Não há o programa 'pacotes' aqui.")

def baixa_e_compila(diretorio: Path) -> None:
   verificacao_do_programa_pacotes()
   assert (diretorio.is_dir())

   print("Caminho escolhido:", diretorio)
   print("Atual diretório: '%s'" % getcwd())

   download = Popen(
      ["pacotes", "--obtem", "c++", "Código Info"],
      stdout= DEVNULL, cwd = diretorio
   )

   if download.wait() == 0:
      projeto = "codigo-info"
      novo_dir = diretorio.joinpath(projeto)
      comando = ["make", "release"]

      print("Download realizado com sucesso.")

      compilacao = Popen(comando, cwd=novo_dir, stdout=DEVNULL)
      status = compilacao.wait()

      print("Programa 'codigo-info' foi compilado com sucesso.")


# === === === === === === === === === === === === === === === === === === ===
#                          Testes Unitários
# === === === === === === === === === === === === === === === === === === ===
from unittest import (TestCase, skip)
from timeit import (timeit)
from shutil import (rmtree)


class TestandoFraction(TestCase):
   def runTest(self):
      a = Fraction(15)
      b = Fraction(99)

      print("Incremento e visualização normal:")
      print(a); a += 1; print(a, end="\n\n")
      print("Comparação:")
      print(a, ">", b, "?", a > b)
      print(a, "<", b, "?", a < b)
      print(a, "=", b, "?", a == b)

class ExploraDeAte(TestCase):
   def setUp(self):
      self.raiz = Path(getenv("HOME"))

   def runTest(self):
      depth = numero_aleatorio(2)
      colecao = []
      result = explora_de_ate(self.raiz, depth, 6, colecao)

      print("Coletado: %d" % len(colecao))
      for entry in colecao:
         print("\t\b\b\b-", entry)

class NumeroAleatorio(TestCase):
   def runTest(self):
      for _ in range(15):
         print(numero_aleatorio(10), end=", ")
      else:
         print("")

class SelecionaDiretorioRandomicamente(ExploraDeAte):
   def runTest(self):
      motor = seleciona_diretorio_randomicamente
      raiz = self.raiz

      for _ in range(4):
         try:
            selecao = motor(raiz)
            print("Escolha:", selecao)
         except LookupError:
            print("Não há subdiretórios para fazer tal seleção")

@skip("Comsome algum tempo")
class ProbabilidadeDeFalhaDaSelecao(ExploraDeAte):
   def runTest(self):
      falhas = Fraction(1)
      sucessos = Fraction(1)
      funcao = seleciona_diretorio_randomicamente
      REPETICOES = 300 
      BARRA = REPETICOES / 25 

      for p in range(REPETICOES):
         try:
            funcao(self.raiz)
            sucessos += 1
            if p % (BARRA) == 0:
               print("Sucessos: %d" % int(sucessos))
         except LookupError:
            falhas += 1
            if p % (BARRA - 5) == 0:
               print("Falhas: %d" % int(falhas))

      total = int(sucessos + falhas)
      n = int(sucessos)
      percentual = 100.0 * (n / total)
      print("Taxa de retorno: %0.1f%%" % percentual)

@skip("Leva bastante tempo pra executar")
class TempoMedioDeExecucao(ExploraDeAte):
   def runTest(self):
      TOTAL = 400
      trecho_de_codigo = "try: chamada(root)\nexcept LookupError: pass"
      resultado = timeit(
         stmt = trecho_de_codigo, setup="print('Inicializado.')", 
         number = TOTAL, globals = {
            "chamada": seleciona_diretorio_randomicamente,
            "root": self.raiz
         }
      )
      conversao = float(repr(resultado))
      calculo = (conversao / TOTAL) * 1000
      print("Levou %0.1fms por chamada." % calculo)

class VerificacaoDoProgramaPacotes(TestCase):
   def runTest(self):
      verificacao_do_programa_pacotes()
      print("Funcinou tudo bem.")

class SimplesAplicacaoDoPrototipo(TestCase):
   def setUp(self):
      self.entulho = []
      self.raiz = Path(getenv("HOME")).joinpath("Desktop")
      # Adicionando futuro diretório colocado.
      self.entulho.append(self.raiz.joinpath("codigo-info"))

   def tearDown(self):
      for entry in self.entulho:
         rmtree(entry)

   def runTest(self):
      baixa_e_compila(self.raiz)
