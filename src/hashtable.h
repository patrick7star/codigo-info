
/* Cabeçalho com os tipos de dados, métodos deles ou funções relacionadas.
 * 
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef HASHTABLE_H
#define HASHTABLE_H
 // todos os tipos de dados e seus apelidos:
 typedef struct tabela_de_dispersao *Hashtable, hashtable_t, Mapa; 
 // necessário mudar se copiado o arquivo para usar com outro tipo:
 typedef char* Chave, chave_t;
 typedef uint16_t Valor, valor_t;

 // alocação e destruição da estrutura.
 hashtable_t* cria_com_capacidade_ht(size_t q); 
 hashtable_t* cria_ht();
 bool destroi_ht(hashtable_t* m);

 /* Insere a chave e o valor(relacionado a ela) no Mapa. */
 bool insere_ht(hashtable_t* m, Chave ch, Valor v); 

/* acha uma chave e troca o valor dela, o retorno é bem sucedido se 
 * a atualização foi definitivamente feita, e falho caso o contrário,
 * que seria se por exemplo, se não houvesse a chave demanda para
 * atualização. */
 bool atualiza_ht(hashtable_t* m, Chave ch, Valor v); 

/* A operação de remoção é apenas usar o método de atualização, para
 * inserir uma chave-em-branco no lugar da 'chave' dada, o resto é 
 * apenas realinhar das 'entradas' na array, tipo chaves-em-branco
 * no fim dela, 'entradas' não vázia no começo, não importando a ordem.
 */
 bool deleta_ht(hashtable_t* m, Chave ch);

 /* Operações simples do estado do Mapa. Elas verificam se contém entradas,
  * quantas entradas(líquido de inserções e remoções), e se uma chave dada
  * já pertence ao 'dicionario'. 
  */
 bool vazia_ht(hashtable_t* m);
 size_t tamanho_ht(hashtable_t* m);
 bool contem_ht(hashtable_t* m, Chave ch); 

/* Obtém o endereço do valor da dada 'chave' na tabela, se a tal não 
 * pertencer a 'tabela', então simplesmente retorna 'null'. Talvez
 * o nome 'obtem' não seja adequado, no futuro pode ser alterado ou
 * ter outros métodos que 'embrulham' este.
 */
 Valor* obtem_ht(hashtable_t* m, Chave ch); 

 // transformação da 'tabela hash' numa string, se ela não for gigante.
 char* ht_to_str(Hashtable m); 

#endif // FIM_DO_HASHTABLE_H
