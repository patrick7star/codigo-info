/* Ao invés de executar o script via shell commands, iremos criar uma itnerface
 * FFI do Python ao C. Aqui, vamos trazer todas as funções relevantes do script
 * que realiza todo o trabalho.
 */
#include "repodownloads.h"
#include <Python.h>


void mostra_o_repositorio_atual(void)
{
   PyObject* funcao, *result, *modulo, *args;
   const char* FUNCAO_NOME = "proprietario_do_repositorio";

   PyRun_SimpleString(
      "import sys\n"
      "sys.path.append('./repo-downloads')\n"
         );
   modulo = PyImport_ImportModule("principal");
   funcao = PyObject_GetAttrString(modulo, FUNCAO_NOME);
   args   = Py_BuildValue("()");
   result = PyObject_CallObject(funcao, args);
}

