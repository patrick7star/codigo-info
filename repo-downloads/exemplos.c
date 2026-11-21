
#include <Python.h>
#include <stdlib.h>
// #include "exemplos.h"

char* funcao_portada_i(void)
{
   PyObject* modulo = PyImport_ImportModule("amostras");
   PyObject* funcao = PyObject_GetAttrString(modulo, "retorna_uma_string");
   PyObject* args = Py_BuildValue("()");
   PyObject* result = PyObject_CallObject(funcao, args);
   char* output = (char*)PyUnicode_AsUTF8(result);

   Py_DECREF(result);
   Py_DECREF(funcao);
   Py_DECREF(args);
   Py_DECREF(modulo);
   return output;
}

int funcao_portada_ii(void)
{
   PyObject* modulo = PyImport_ImportModule("amostras");
   PyObject* funcao = PyObject_GetAttrString(modulo, "retorna_um_inteiro");
   PyObject* args = Py_BuildValue("()");
   PyObject* result = PyObject_CallObject(funcao, args);
   int output = PyLong_AsLong(result);

   Py_DECREF(result);
   Py_DECREF(funcao);
   Py_DECREF(args);
   Py_DECREF(modulo);
   return output;
}

int main(int total, char* argumentos[]) 
{
   Py_Initialize();
   PyRun_SimpleString("import sys\nsys.path.append('repo-downloads')\n");

   char* string = funcao_portada_i();
   int inteiro = funcao_portada_ii();

   printf("Resultado:\n\t'%s'\n\t%d\n", string, inteiro);

   Py_Finalize();

   return EXIT_SUCCESS;
}
