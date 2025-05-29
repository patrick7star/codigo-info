// Declaração do módulo:
#include "classificacao.h"
// Biblioteca padrão do C:
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
// Apenas em testes:
#ifdef __debug__
#include "macros.h"
#endif
// Resultado de confirmação da igualdade entre strings.
#define STRINGS_IGUAIS 0

// Lembrete: os primeiros 6/24 bytes dos executáveis são identificação.
const uint8_t TOTAL = 4;
const uint8_t ID_EXECUTAVEL[24] = {
   // Identificador: 
   //    0x7F45_4C46_0201_0100_0000_0000_0000_0000_0003_003E_0001_0000
   0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x03, 0x00, 0x3E, 0x00, 0x01, 0x00, 0x00, 0x00
}; 


bool e_um_executavel (char* caminho) {
   /* verifica se o executável é ELF. Para isso verifica os primeiros 24
    * bytes do arquivo, se cada um bater com a identificação universal, 
    * então é um. */
   FILE* arq = fopen (caminho, "rb");
   uint8_t bytes[TOTAL];

   // lê o necessário de bytes, e tem que ler tudo.
   int lido = fread(bytes, sizeof(uint8_t), TOTAL, arq);
   if (lido != TOTAL) { return false; }

   // todos vinte e quatro bytes precisam ser checados.
   for (size_t k = 1; k <= TOTAL; k++) {
      if (ID_EXECUTAVEL[k - 1] != bytes[k - 1]) 
         return false;
   }
   // se chegar até aqui é porquê eles são iguais.
   return true;
}

static bool verifica_o_makefile(char* caminho) {
/* Diz se um caminho é um makefile, obviamente para tirar dos demais arquivos
 * analisados. Ele precisa de uma função especializada nisso, pois arquivos
 * do tipo geralmente não tem extensão. */
   return strstr(caminho, "makefile") != NULL || 
          strstr(caminho, "Makefile") != NULL;
}

TipoDeArquivo tipo_do_arquivo(char* arquivo_caminho) {
   arquivo_caminho = realpath(arquivo_caminho, NULL);

   char* extensao = strrchr(arquivo_caminho, '.');
   bool e_um_makefile = verifica_o_makefile(arquivo_caminho);
   bool nome_tem_uma_extensao = (
      // Achou algo.
      extensao != NULL &&
      // Más,... não é o ponto do atual diretório.
      extensao != arquivo_caminho
   );

   // Verifica se a 'extensão' extraída bate com um 'sufíxo' dado.
   bool bate_com_extensao(const char* sufixo) { 
      if (extensao == NULL)
         return false;
      size_t resultado = strcmp(extensao, sufixo);
      return  resultado == STRINGS_IGUAIS; 
   }
   
   if (nome_tem_uma_extensao) {
      // proposições mais trabalhadas:
      bool e_codigo_c = {
         strcmp(extensao, ".c") == STRINGS_IGUAIS ||
         strcmp(extensao, ".h") == STRINGS_IGUAIS
      };
      bool e_codigo_cplusplus = {
         strcmp(extensao, ".cpp") == STRINGS_IGUAIS ||
         strcmp(extensao, ".h") == STRINGS_IGUAIS
      };
      bool e_um_codigo_python = {
         strcmp(extensao, ".py") == STRINGS_IGUAIS 
      };

      if (e_codigo_c) return C;
      else if (e_codigo_cplusplus) return CPLUSPLUS;
      else if (e_um_codigo_python) return PYTHON;
      else if (strcmp(extensao, ".java") == STRINGS_IGUAIS)
         return JAVA;
      else if (strcmp(extensao, ".js") == STRINGS_IGUAIS)
         return JAVASCRIPT;
      else if (strcmp(extensao, ".html") == STRINGS_IGUAIS)
         return HTML;
      else if (strcmp(extensao, ".xml") == STRINGS_IGUAIS)
         return XML;
      else if (strcmp(extensao, ".rs") == STRINGS_IGUAIS)
         return RUST;
      else if (strcmp(extensao, ".zip") == STRINGS_IGUAIS)
         return ZIP;
      else if (strcmp(extensao, ".tar") == STRINGS_IGUAIS)
         return TAR;
      else if (strcmp(extensao, ".gzip") == STRINGS_IGUAIS)
         return GZIP;
      else if (bate_com_extensao(".dat") || bate_com_extensao(".data"))
         return DAT;
      else if (strcmp(extensao, ".json") == STRINGS_IGUAIS)
         return JSON;
      else if (strcmp(extensao, ".txt") == STRINGS_IGUAIS)
         return TEXTO;
      else if (strcmp(extensao, ".srt") == STRINGS_IGUAIS)
         return SRT;
      else if (bate_com_extensao(".jpg") || bate_com_extensao(".jpeg"))
         return JPEG;
      else if (strcmp(extensao, ".png") == STRINGS_IGUAIS)
         return PNG;
      else if (strcmp(extensao, ".gif") == STRINGS_IGUAIS)
         return GIF;
      else if (strcmp(extensao, ".mp4") == STRINGS_IGUAIS)
         return MP4;
      else if (bate_com_extensao(".mp3"))
         return MP3;
      else if (bate_com_extensao(".mkv"))
         return MKV;
      else if (bate_com_extensao(".pdf"))
         return PDF;
      else if (bate_com_extensao(".epub"))
         return EPUB;
      else if (bate_com_extensao(".doc") || bate_com_extensao(".docx")
         || bate_com_extensao(".pptx") || bate_com_extensao(".ppt")
         || bate_com_extensao(".ods") || bate_com_extensao(".xlsx")
         || bate_com_extensao(".odt")
      )
         // Todos tipos de extensão do Suíte Office colocados juntos.
         return DOCS;
      else if (bate_com_extensao(".o") || bate_com_extensao(".a"))
         return ARTEFATO;
      else if (bate_com_extensao(".opf"))
         /* Metadados, sempre acompanha o formato 'epub'.*/
         return METADADOS;
      else 
         return Desconhecido;
   } else {
      bool e_um_arquivo_binario_elf = {
         e_um_executavel (arquivo_caminho) || 
         bate_com_extensao(".so")
      };

      if (e_um_arquivo_binario_elf)
         return ELF;
      else if (e_um_makefile)
         return BUILD;
      else 
         return Desconhecido;
   }
   // Liberando memória alocada.
   free(arquivo_caminho);
}

const char* ta_to_str(TipoDeArquivo tipo) {
   switch (tipo) {
      // códigos-fontes:
      case C:           return "C Code";
      case CPLUSPLUS:   return "C++ Code";
      case PYTHON:      return "Python Code";
      case JAVA:        return "Java Code";
      case TEXTO:       return "Texto Unicode";
      case RUST:        return "Rust Code";
      case GOLANG:      return "Go Code";
      case JAVASCRIPT:  return "Javascript Code";
      case HTML:        return "HTML Code";
      case XML:         return "XML Code";
      // arquivos comprimidos:
      case TAR:         return "Tar archive";
      case ZIP:         return "Zip compacted archive";
      case GZIP:        return "Gzip compacted archive";
      // bancos de dados:
      case DAT:         return "Data database";
      case JSON:        return "JSON database";
      case SRT:         return "Subtitles database";
      // binários:
      case ELF:         return "ELF executable";
      // arquivos de multimídia:
      case JPEG:        return "JPEG image data";
      case PNG:         return "PNG image data";
      case GIF:         return "GIF image data";
      case MKV:         return "Mastroka(mkv) video data";
      case MP4:         return "MP4 video data";
      case MP3:         return "MP3 song data";
      case PDF:         return "PDF file";
      case EPUB:        return "EPUB file";
      case DOCS:        return "DOCs office suite";
      case METADADOS:   return "Metadados";
      case ARTEFATO:    return "Object artefacts";
      case BUILD:       return "START build files";

      // caso de nenhum acima, interrompe o programa.
      default:
         return "Desconhecido";
   }
}

bool pode_ser_lido(TipoDeArquivo tipo) {
   /*   Apenas alguns arquivos podem ser lidos, tipos códigos e arquivos 
    * de texto puro, já outros como binários, archives, imagens e tal, é 
    * desnecessário lê isso procurando pelas informações que são 
    * processadas, logo serão descartadas.
    * 
    *   Tal função poupa várias chamadas de sistemas, assim economiza um
    * processsamento pesado.
    */
   bool arquivos_permitidos = {
      tipo == PYTHON || tipo == XML || tipo == TEXTO ||
      tipo == CPLUSPLUS || tipo == C || tipo == RUST ||
      tipo == JAVASCRIPT || tipo == JAVA || tipo == GOLANG
      || tipo == BUILD
      // a colocar mais futuramente ...
   };

   // qualquer uma das comparações acima gera um valor lógico verdadeiro.
   return arquivos_permitidos;
}

