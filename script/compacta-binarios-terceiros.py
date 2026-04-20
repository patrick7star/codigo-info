"""
  Função compacta e descompacta binários de terceiro no programa.
 Isso baseado na arquitetura/e sistema atual do sistema.
"""
import tarfile, glob,platform
from pathlib import Path

BINARIOS_ESTATICOS = glob.glob("./lib/*.a")
CABECALHOS = glob.glob("./lib/*/*.h")
(SISTEMA, CPU) = (platform.system().lower(), platform.machine())
NOME = "utils-{}-{}".format(SISTEMA, CPU)
ARQUIVO = Path("./lib/{}.tar".format(NOME))

if ARQUIVO.exists():
   print("O arquivo '{}' já existe!".format(ARQUIVO))
   exit(0)

with tarfile.TarFile(ARQUIVO, mode="w") as arquivo:
   for caminho in BINARIOS_ESTATICOS:
      arquivo.add(caminho)
   arquivo.add("./lib/include")

   print("\nTudo que foi colocado no arquivo:")
   arquivo.list()

print("Arquivado com sucesso.")



