
#ifndef MOTOR_H
#define MOTOR_H

 typedef struct { double uptime;  double idle; } UptimeConteudo;
 typedef double UptimeConteudoWin;
 
 UptimeConteudoWin tempo_windows_ativado(void);
 UptimeConteudo tempo_wsl_ativado(void); 
 void debug_uc(UptimeConteudo* obj); 
 void formata_um_horario(char* Output); 

#endif
