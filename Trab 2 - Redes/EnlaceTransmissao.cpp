/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Importamos as bibliotecas iostream pra realizar o fluxo de entrada e saída de dados*/
#include <iostream>

/** As bibliotecas util e EnlaceTransmissão foram criadas por nos e utilizadas nesta etapa*/
#include "util.h"
#include "EnlaceTransmissao.h"

using namespace std;

/* 
 * A ideia do controle de erros por paridade par eh basicamente que o transmissor, 
 * inclui um bit adicional e escolhe o valor desse bit de modo que o numero total de "1" 
 * na parcela de bits do quadro + o bit de paridade seja par. 
 */
/** Funcao que representa o controle de erros por paridade par da aplicacao transmissora. */
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (int quadro[]){
    //implementação do algoritmo
    int res = calcUms(quadro, 0, tamBitMsg + CRC); //calcula quantidade de "1" nos bits da mensagem
    if (res % 2 == 0) //se essa quantidade for par, o bit de paridade será 0
        quadro[tamBitMsg-1] = 0;
    else //se for impar, o bit de paridade será 1
        quadro[tamBitMsg-1] = 1;

    cout << "\nDepois do Bit de Paridade Par (transmissora): ";
    printBits(quadro, 0, tamBitMsg); //exibe os bits
}//fim do método CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar

/* 
 * No esquema abaixo, de paridade impar, diferente do anterior, o valor do bit de paridade
 * eh escolhido para que haja um numero impar de "1". 
 */

/** Funcao que representa o controle de erros por paridade impar da aplicacao transmissora */
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar (int quadro[]){
    //implementação do algoritmo
    int res = calcUms(quadro, 0, tamBitMsg + CRC); //calcula quantidade de "1" nos bits da mensagem
    if (res % 2 == 0) //se essa quantidade for par, o bit de paridade será 1
        quadro[tamBitMsg-1] = 1;
    else //se for impar, o bit de paridade será 1
        quadro[tamBitMsg-1] = 0;

    cout << "\nDepois do Bit de Paridade Impar (transmissora): ";
    printBits(quadro, 0, tamBitMsg); //exibe os bits
}//fim do método CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar

/*
O código CRC (código de redundancia ciclica, ou codigos polinomiais)
E seu funcionamento pode ser interpretado da seguinte forma:
O transmissor quer enviar uma parcela de bits de dados D para o receptor,
o remetente e o receptor devem concordar com um padrao de bits, conhecido como gerador
que no nosso caso é a variavel const int crc32[] do "util.h".
Então antes do transmissor enviar a parcela de bits eh necessario acrescentar um quantidade de bits 0 adicionais
do tamanho da quantidade de bits do gerador-1, e fazer a divisao pelo gerador, resultando na parcela de bits
a ser enviada.
Assim, o receptor pode passar por um processo de verificacao em "EnlaceRecepcao.h".
*/
/** Função que representa o controle de erros da aplicacao transmissora por CRC */
void CamadaEnlaceDadosTransmissoraControleDeErroCRC (int quadro[]){
    //implementação do algoritmo
    /*Utilizamos uma variavel auxiliar (quadroAux) para poder trabalharmos corretamente com os dados 
    visto que estamos passando por referencia. Essa variavel tera a parcela de bits enviados do transmissor.
    */
    int quadroAux[tamBitMsg + CRC];

    //Aqui percorremos todos os quadros e adicionamos ela a uma variável auxiliar
    for (int i = 0; i < tamBitMsg + CRC; i++){
        quadroAux[i] = quadro[i];
    }

     /*Buscando o ponto do vetor de mensagem onde será escrita a chave*/
    for (int i = 0; i < tamBitMsg; i++){
        if (quadroAux[i] == 0) //Pula os bits se for 0
            continue;

        /*Divisao da parcela de bits para envio ao receptor como chave de validação */
        for (int j = 0; j < CRC; j++){
            if (quadroAux[i+j] == crc32[j])//Verifica o valor do dado
                quadroAux[i+j] = 0;//Caso dois valores iguais = resultado da divisão = 0
            else
                quadroAux[i+j] = 1;//Caso dois valores diferentes = resultado da divisão = 1
        }
    }
    
    /*Volta a mensagem da variavel auxiliar para o quadro, pois sera utilizado pela receptora*/
    for (int i = tamBitMsg; i < tamBitMsg+CRC; i++)
        quadro[i] = quadroAux[i];

    cout << "\nDepois do CRC (transmissora): ";
    printBits(quadro, 0, tamBitMsg + CRC);
}//fim do método CamadaEnlaceDadosTransmissoraControleDeErroCRC

/** Funcao que determina qual dos algoritmos que controle de erros será utilizado */
void CamadaEnlaceDadosTransmissoraControleDeErro (int quadro[], int tipoDeControleDeErro){
    switch (tipoDeControleDeErro){
    case 0: //bit de paridade par
        CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(quadro);
        break;
    case 1: //bit de paridade impar
        CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar(quadro);
        break;
    case 2: //CRC
        CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro);
        break;
    default://
        break;
    }//fim do switch case
    cout << endl;
}//fim do metodo CamadaEnlaceDadosTransmissoraControleDeErro