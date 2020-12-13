/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Definimos os tamanhos doas variáveis */
#define ERRO 1 //indica se houve erro
#define VALIDO 0 //indica se eh valido
#define CRC 32 //32 bits do CRC-32
#define tamMsg 16 //tamanho maximo, em bytes, da mensagem
#define tamBitMsg (8*tamMsg) //tamanho maximo, em bits, da mensagem

/** Gerador do crc32, que é baseado no seu polinomio */
const int crc32[] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1};

/** Funcao que exibe os bits da mensagem */
void printBits(int*, int, int);

/** Esta função serve para percorrer toda a mensagem e somar todos os 1's a variavel auxiliar res e retorna-la */
int calcUms(int*, int, int);