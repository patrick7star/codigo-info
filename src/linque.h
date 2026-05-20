
#ifndef __LINQUE_H__
#define __LINQUE_H__

 typedef enum {
      Nenhuma = 1 << 0,       Alfabetica  = 1 << 1,
      Criacao = 1 << 2,       Acesso      = 1 << 3,
      Sistema = 1 << 4,       Aleatoria   = 1 << 5
 } OrdemDirEnt ;

 void info_sobre_repositorio_de_linques(void); 
 void info_sobre_repositorio_de_linques_ordenada(void); 
 const char* ordemdirent_to_str(OrdemDirEnt);
 void salva_ode(OrdemDirEnt);

#endif
