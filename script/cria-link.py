#!/bin/python3 -OO
"""
   Cria um linque no repositório "oficial" de linque que o usuário define,
 que é $LINKS. Lá são colocados todos linques de aplicações CLI, que foram
 desenvolvidas por mim.
"""

from os import (getenv, environ as EnvironVars)
from pathlib import (Path)

IDENTIFICADOR = "LINKS"
if __debug__:
   print("\nRodando no modo DEBUG", end='\n\n')
   NOME = "codigo-info-debug"
   EXECUTAVEL = Path("./bin/codigo-info-debug").resolve()
else:
   NOME = "codigo-info"
   EXECUTAVEL = Path("./bin/codigo-info").resolve()


def cria_caminho_do_linque() -> Path:
   "Retorna caminho onde o linque será criado, como o NOME."
   caminho = Path(getenv(IDENTIFICADOR))
   caminho = caminho.joinpath(NOME)

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

   #assert (not caminho.exists(follow_symlinks=False))

   try:
      caminho.symlink_to(EXECUTAVEL)
   except FileExistsError:
      caminho.unlink()
      print("Removido linque já lá.")
      caminho.symlink_to(EXECUTAVEL)
   finally:
      assert (caminho.exists(follow_symlinks=False))
      print("Linque criado com sucesso.")


verificacao_basica()
cria_linques()
