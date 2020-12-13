/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Funcao que representa o controle de erros por paridade par da aplicacao transmissora. */
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (int*);

/** Funcao que representa o controle de erros por paridade impar da aplicacao transmissora */
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar (int*);

/** Função que representa o controle de erros da aplicacao transmissora por CRC */
void CamadaEnlaceDadosTransmissoraControleDeErroCRC (int*);

/** Funcao que determina qual dos algoritmos que controle de erros será utilizado */
void CamadaEnlaceDadosTransmissoraControleDeErro (int*, int);