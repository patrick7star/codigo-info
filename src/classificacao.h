/* Vários tipos de arquivos serão analizados, e assim classificados. O código
 * deste módulo cuida de todo ferramental necessários para isso.
 */

#ifndef __CLASSIFICACAO_H__
#define __CLASSIFICACAO_H__
#include <stdbool.h>

 typedef enum tipo_de_codigo { 
    // linguagens de programação:
   C, CPLUSPLUS, PYTHON, RUST, JAVA, GOLANG, JAVASCRIPT, HTML, XML,
   BUILD,
   // archives:
   TAR, ZIP, GZIP,
   // arquivos de dados e metadados:
   DAT, JSON, SRT, METADADOS,
   // arquivos binários:
   ELF, ARTEFATO, 
   // arquivos de multimídia:
   PNG, JPEG, WEBP, GIF, MP3, MP4, MKV, AVI, 
   // arquivos de textos:
   PDF, EPUB, MOBI,
   // arquivos de edições de texto:
   TEXTO, DOCS,
   // ainda não catalogado.
   Desconhecido

 } TipoDeArquivo;

 bool          e_um_executavel   (char* caminho); 
 TipoDeArquivo tipo_do_arquivo   (char* arquivo_caminho); 
 const char*   ta_to_str         (TipoDeArquivo tipo); 
 bool          pode_ser_lido     (TipoDeArquivo tipo);

#endif
