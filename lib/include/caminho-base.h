/*   Biblioteca criada à partir do módulo 'linque.rs' em 'pacotes-externos', 
 * mas especificamente em 'src' do projeto. Este header file é para 
 * realmente fazer uma biblioteca. O nome da biblioteca compartilhada gerada 
 * pelo o compilador do Rust é o mesmo que este projeto, 'caminho base', 
 * porém junto, mais o clássico 'lib', logo o resultado gerado será algo 
 * como 'libcaminhobase'.
 */
#ifndef __CAMINHO_BASE_H__
#define __CAMINHO_BASE_H__

 /* Retorna o caminho concatenada com o 'complemento' baseado neste projeto. 
  * Se houver alguma falha, NULL será retornado no lugar. A desalocação da 
  * array fica à cargo do chamador.*/
 char* computa_caminho_externo(char* complemento); 
 void  libera_caminho_externo(char* caminho);

#endif
