/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Importamos as bibliotecas iostream pra realizar o fluxo de entrada e saída de dados*/
#include <iostream>

using namespace std;

/** Funcao que exibe os bits da mensagem */
void printBits(int val[], int inicio, int fim){
    for (int i = inicio; i < fim; i++){
        cout << val[i];
    }
    cout << endl;
}

/** Esta função serve para percorrer toda a mensagem e somar todos os 1's a variavel auxiliar res e retorna-la */
int calcUms(int val[], int inicio, int fim){
    int res = 0;
    for (int i = inicio; i < fim; i++){
        if (val[i] == 1)
            res++;
    }
    return res;
}