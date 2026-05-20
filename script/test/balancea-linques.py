"""
  Já que existe um script para criar linques quebrados, em diferentes sistemas
 e seus respectivos sistemas de arquivos. Agora, irei fazer algo para
 balancear a conta. Já que a assimetria de se criar tais linques quebrados é
 imensamente maior do que criar los manualmente. Estou pensando numa taxa de
 45%, mas provavelmente será decidido de acordo com o valor codificado, ou
 passada por variável de ambiente.
"""
import os
from pathlib import (Path)
from random import (choice)


# Taxa imposta pelo o programador.
LIMITE = 45.0

def todos_linques() -> list[Path]:
   return list(Path(os.getenv("LINKS")).iterdir())

def linque_quebrado(linque: Path) -> bool:
   caminho = linque.readlink()
   return caminho.name == "ExperimentaL" and not caminho.exists()

def percentual_linques_quebrados(lista: list[Path]) -> float:
   total = 0; quebrados = 0

   for item in lista:
      caminho = item.readlink()

      if caminho.name == "ExperimentaL" and not caminho.exists():
         quebrados += 1
      total += 1

   return 100.0 * quebrados / total

def remove_linque_quebrado(lista: list[Path]) -> Path:
   # Realiza um sorteio nas fronteiras da lista.
   selecao = choice(lista)

   while not linque_quebrado(selecao):
      selecao = choice(lista)

   lista.remove(selecao)
   return selecao

def remove_ate_limite(lista: list[Path]) -> set[Path]:
   remocoes = set([])
   L = lista
   percentual = percentual_linques_quebrados(L)

   while percentual > LIMITE:
      caminho = remove_linque_quebrado(L)
      percentual = percentual_linques_quebrados(L)
      remocoes.add(caminho)

      print("\rProgresso está em {:3.1f}%.".format(percentual), end='')

   print("")
   return remocoes

lista = todos_linques()
percentual = percentual_linques_quebrados(lista)

remocoes = remove_ate_limite(lista)

print(
   "Os linques quebrados compõem atualmente %0.1f do repositório."
   % percentual_linques_quebrados(lista)
)
print("\nProcesso de exclusão em série dos linques...")

for item in remocoes:
   item.unlink()
   print("'{}' foi deletado.".format(item.name))


