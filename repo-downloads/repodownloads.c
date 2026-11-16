/* Ao invés de executar o script via shell commands, iremos criar uma itnerface
 * FFI do Python ao C. Aqui, vamos trazer todas as funções relevantes do script
 * que realiza todo o trabalho.
 */
#include "repodownloads.h"
#include <Python.h>


int realiza_download_neste_diretorio(char* url)
{
   const char* FUNCAO_NOME = "realiza_download_no_atual_diretorio";
   PyObject* modulo = PyImport_ImportModule("main");

   if (modulo == NULL) {
      PyErr_Print(); 
      Py_DECREF(modulo);
      return 1;
   }

   PyObject* funcao = PyObject_GetAttrString(modulo, FUNCAO_NOME);

   if (funcao == NULL || !PyCallable_Check(funcao))
   {
      Py_DECREF(modulo);
      Py_DECREF(funcao);
      PyErr_Print();
      return 1;
   }

   PyObject* result = PyObject_CallFunction(funcao, "s", url);

   if (result == NULL)
   {
      PyErr_Print();
      Py_DECREF(modulo);
      Py_DECREF(funcao);
      Py_DECREF(result);
      return 1;
   }

   long valor = PyLong_AsLong(result);

   if (PyErr_Occurred()) 
      PyErr_Print();
   else
      printf("Result: %ld\n", valor);

   return EXIT_SUCCESS;
}

void listagem_do_repositorio(void)
{
   Py_Initialize();
   PyRun_SimpleString(
      "import sys\n"
      "sys.path.append('./repo-downloads')\n"
         );

   const char* FUNCAO_NOME = "listagem_de_repositorios_disponiveis";
   PyObject* modulo = PyImport_ImportModule("main");

   if (modulo == NULL) {
      PyErr_Print(); 
      return;
   }

   PyObject* funcao = PyObject_GetAttrString(modulo, FUNCAO_NOME);

   if (funcao == NULL || !PyCallable_Check(funcao))
   {
      PyErr_Print();
      return;
   }

   PyObject* result = PyObject_CallFunction(funcao, NULL);

   if (result == NULL)
   {
      PyErr_Print();
      return;
   }

   long valor = PyLong_AsLong(result);

   if (PyErr_Occurred()) 
      PyErr_Print();
   else
      printf("Result: %ld\n", valor);

   Py_Finalize();
}

