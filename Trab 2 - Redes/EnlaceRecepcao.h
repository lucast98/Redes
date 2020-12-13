/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Função que representa o controle de erros da aplicacao receptora por CRC */
int CamadaEnlaceDadosReceptoraControleDeErroCRC(int *);

/** Funcao que representa o controle de erros por paridade impar da aplicacao receptora */
int CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar (int*);

/** Funcao que representa o controle de erros por paridade par da aplicacao receptora. */
int CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (int*);

/** Funcao que determina qual dos algoritmos que controle de erros será utilizado */
int CamadaEnlaceDadosReceptoraControleDeErro (int*, int);

/** Funcao utilizada para chamar a função de erros dentro da camada receptora utilizando cada quadro */
int CamadaEnlaceDadosReceptora(int*, int);