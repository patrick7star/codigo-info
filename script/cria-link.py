#!/bin/python3 -OO
"""
   Cria um linque no repositório "oficial" de linque que o usuário define,
 que é $LINKS. Lá são colocados todos linques de aplicações CLI, que foram
 desenvolvidas por mim.
"""

from os import (getenv, environ as EnvironVars)
from pathlib import (Path)

if __debug__:
   print("\nRodando no modo DEBUG", end='\n\n')
   NOME = "codigo-info-debug"
else:
   NOME = "codigo-info-release"
IDENTIFICADOR = "LINKS"
# Caminho ao executável em relação ao projeto.
EXECUTAVEL = Path("./bin/%s" % NOME).resolve()


def cria_caminho_do_linque() -> Path:
   "Retorna caminho onde o linque será criado, como o NOME."
   caminho = Path(getenv(IDENTIFICADOR))
   NOME_ESPECIAL = "codigo-info"

   if __debug__:
      caminho = caminho.joinpath(NOME)
   else:
      caminho = caminho.joinpath(NOME_ESPECIAL)

   return caminho

def verificacao_basica() -> None:
   if "LINKS" in EnvironVars:
      print("Variável 'LINKS' está definida.")
   else:
      if __debug__:
         print("Variável 'LINKS' não foi definda.")

   caminho = cria_caminho_do_linque()

   if (not caminho.exists(follow_symlinks=False)):
      print("O linque de '%s' não existe." % NOME)

def cria_linques() -> None:
   caminho = cria_caminho_do_linque()

   try:
      caminho.symlink_to(EXECUTAVEL)
   except FileExistsError:
      caminho.unlink()
      print("Removido linque já lá.")
      caminho.symlink_to(EXECUTAVEL)
   finally:
      assert (caminho.exists(follow_symlinks=False))
      print("Linque criado com sucesso.")


print("Repositório dos linques: '{}'".format(getenv("LINKS")))
verificacao_basica()
cria_linques()
