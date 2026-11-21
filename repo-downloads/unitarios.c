#include <Python.h>
#include <stdlib.h>
#include "repodownloads.h"


int main(int total, char* argumentos[]) 
{
   Py_Initialize();
   listagem_do_repositorio();
   Py_Finalize();
   return EXIT_SUCCESS;
}
