/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Importamos as bibliotecas iostream pra realizar o fluxo de entrada e saída de dados*/
#include <iostream>
/** A biblioteca bits/stdc++ possui uma enorme quantidade de funções padroes, e por causa dela reduzimos o numero de importações*/
#include <bits/stdc++.h>
/** As bibliotecas util, EnlaceTransmissao e EnlaceRecepcao foram criadas por nos e utilizadas nesta etapa*/
#include "util.h"
#include "EnlaceTransmissao.h"
#include "EnlaceRecepcao.h"

using namespace std;

/** Função que converte os bits para uma string novamente, exibindo uma mensagem e a string recebida */
void AplicacaoReceptora(int quadro[], int tam){
    int valorAscii, i;

    //exibe mensagem
    cout << "A mensagem recebida foi: ";
    
    //converte binario para decimal
    for (i = 0; i < tam; i++){
        valorAscii = 0;
        for (int j = 0; j < 8; j++)
            valorAscii += pow(2, 7-j)*quadro[j+(8*i)]; //mais significativo a esquerda
        cout << (char)valorAscii; //obtem caracetere
    }
    cout << endl;
}

/** Função que representa o meio de comunicação entre a aplicação transmissora e a receptora. Possui uma porcentagem de erro que deve
 * ser alterada para testes.
 */
void MeioDeComunicacao (int fluxoBrutoDeBits[]){
    //trabalhar com bits
    int i = 0;
    int porcentagemDeErros;
    int fluxoBrutoDeBitsPontoA[tamBitMsg + CRC], fluxoBrutoDeBitsPontoB[tamBitMsg + CRC];

    porcentagemDeErros = 0; //alterar valor

    /** Fluxo no ponto a recebe o fluxo original */
    for (i = 0; i < tamBitMsg + CRC; i++)
        fluxoBrutoDeBitsPontoA[i] = fluxoBrutoDeBits[i]; //salva o fluxo original de bits em um vetor auxiliar
    fill(fluxoBrutoDeBitsPontoB, fluxoBrutoDeBitsPontoB+tamBitMsg+CRC, 0); //zera o vetor para limpar lixo de memoria

    for (i = 0; i < tamBitMsg + CRC; i++){
        if (rand()%100 >= porcentagemDeErros) //numero aleatorio deve ser maior ou igual a porcentagem de erros
            fluxoBrutoDeBitsPontoB[i] += fluxoBrutoDeBitsPontoA[i];
        else{ //caso contrario, verifica se o bit do fluxo ponto b é 0. 
        //Caso seja, passará 1 para o fluxo do ponto A. Caso não seja, passará 0 para o fluxo do ponto B.
            fluxoBrutoDeBitsPontoB[i] == 0 ?
            fluxoBrutoDeBitsPontoA[i] = ++fluxoBrutoDeBitsPontoB[i]:
            fluxoBrutoDeBitsPontoA[i] = --fluxoBrutoDeBitsPontoB[i];
        }
    }

    //fluxo original recebe recebe o fluxo com possiveis erros
    for (i = 0; i < tamBitMsg + CRC; i++)
        fluxoBrutoDeBits[i] = fluxoBrutoDeBitsPontoA[i];
}//fim do metodo MeioDeComunicacao

/** Função que representa a camada fisica transmissora. Responsavel por realizar a comunicacao com o meio de comunicacao */
void CamadaFisicaTransmissora(int quadro[]){
    MeioDeComunicacao(quadro); //chama o meio de comunicacao
}//fim do metodo CamadaFisicaTransmissora

/** Função responsavel por transformar os caracteres da string em binarios (bits) */
void CamadaDeAplicacaoTransmissora(string *mensagem, int quadro[]){
    int aux[8];
    int valorAscii = 0, k = 0;
    
    //trabalhar com bits
    for (int i = 0; i < (*mensagem).length(); i++){
        valorAscii = (int)(*mensagem)[i]; //valor ascii do caractere

        /** Aqui convertemo o numero decimal para binario */
        for (int j = 7; j > 0; j--){
            aux[j] = valorAscii % 2;
            valorAscii = valorAscii / 2;
        }
        aux[0] = valorAscii;
        cout << (*mensagem)[i] << ": "; //exibe o caractere
        for (int j = 0; j < 8; j++){ //exibe o valor do bit do caractere
            quadro[k] = aux[j];
            cout << quadro[k];
            k++;
        }
        cout << endl;
    }
}//fim do metodo CamadaDeAplicacaoTransmissora

/** Função que representa a camada de enlace de dados transmissora */
void CamadaEnlaceDadosTransmissora (int quadro[], int tipoDeControleDeErro){
    CamadaEnlaceDadosTransmissoraControleDeErro(quadro, tipoDeControleDeErro); //chama a função de controle de erros
}//fim do metodo CamadaEnlaceDadosTransmissora

/** Função que representa a aplicacao transmissora. Exibe uma mensagem e chama a camada de aplicacao transmissora. */
void AplicacaoTransmissora(string *mensagem, int quadro[]){
    cout << "Digite uma mensagem: " << endl;
    cin >> *mensagem;
    CamadaDeAplicacaoTransmissora(mensagem, quadro); //chama a proxima camada
}//fim do metodo AplicacaoTransmissora

int main (void){
    string mensagem;
    int tipoDeControleDeErro = 2; /* determina o tipo de controle de erro a ser utilizado pela aplicacao de transmissao e recepcao.
    0 = controle por bit de paridade par; 1 = controle por bit de paridade impar; 2 = controle por CRC */
    int erro; //determina
    int mensagemBit[tamBitMsg + CRC]; //representa os bits da mensagem
    fill(mensagemBit, mensagemBit+tamBitMsg+CRC, 0); //preenche a variavel mensagemBit com 0 para evitar lixos de memoria

    /** Aplicacao Transmissora */
    AplicacaoTransmissora(&mensagem, mensagemBit);
    CamadaEnlaceDadosTransmissora(mensagemBit, tipoDeControleDeErro);
    
    /** Camada Fisica Transmissora */
    CamadaFisicaTransmissora(mensagemBit);

    /** Camada Fisica Receptora */
    //CamadaFisicaReceptora(mensagemBit);

    /** Aplicacao Receptora */
    erro = CamadaEnlaceDadosReceptora(mensagemBit, tipoDeControleDeErro);
    if (!erro) //se nao tiver erro
        AplicacaoReceptora(mensagemBit, mensagem.length());
    return 0;
}//fim da aplicação
