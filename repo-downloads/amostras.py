import random

__all__ = [
   "retorna_uma_string"
]

def retorna_uma_string() -> str:
   print("O valor de string foi retornado.")
   return "Água mole e pedra dura tanto bate até que fura."

def retorna_um_inteiro() -> int:
   print("O valor do tipo 'int' foi retornado.")
   return random.randint(1, 99)
