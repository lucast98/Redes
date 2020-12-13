/*
10345251 - Higor Tessari
10295180 - Lucas Tavares dos Santos 
10716612 - Renan Peres Martins
10373663 - Renata Oliveira Brito
*/

/** Importamos as bibliotecas iostream pra realizar o fluxo de entrada e saída de dados */
#include <iostream>

/** As bibliotecas util e EnlaceRecepcao foram criadas por nos e utilizadas nesta etapa */
#include "util.h"
#include "EnlaceRecepcao.h"

using namespace std;

/*
O receptor, verifica a parcela de bits enviada pelo transmissor, assim o receptor divide os bits recebidos
pelo gerador, se o resto for diferente de zero, o receptor saberá que ocorreu um erro, caso contrário, os dados
são aceitos como corretos.
*/
/** Função que representa o controle de erros da aplicacao receptora por CRC */
int CamadaEnlaceDadosReceptoraControleDeErroCRC(int quadro[]){
    //implementação do algoritmo
    /*Utilizamos uma variavel auxiliar (quadroAux) para poder trabalharmos corretamente com os dados 
    visto que estamos passando por referencia. Essa variavel tera a parcela de bits enviados do transmissor.
    */
    int quadroAux[tamBitMsg + CRC];

    /** Aqui percorremos todos os quadros e adicionamos ela a uma variável auxiliar */
    for (int i = 0; i < tamBitMsg + CRC; i++)
        quadroAux[i] = quadro[i];

    /*Buscando o ponto do vetor de mensagem onde será escrita a chave*/
    for (int i = 0; i < tamBitMsg; i++){
        if (quadroAux[i] == 0) //Pula os bits se for 0
            continue;
        
        /** Divisao da parcela de bits que recebeu do transmissor */    
        for (int j = 0; j < CRC; j++){
            if (quadroAux[i+j] == crc32[j])//Verifica o valor do dado
                quadroAux[i+j] = 0;//Caso dois valores iguais = resultado da divisão = 0
            else
                quadroAux[i+j] = 1;//Caso dois valores diferentes = resultado da divisão = 1
        }
    }
    /** Printamos o resultado para verificacao */
    cout << "Depois do CRC (receptora): "; 
    printBits(quadroAux, 0, tamBitMsg+CRC);

    /** Verifica se no resto da divisao tem bit igual a 1, se tiver ocorreu erro */
    for (int i = tamBitMsg; i < tamBitMsg+CRC; i++){
        if (quadroAux[i] == 1){
            cout << "Erro CRC" << endl;
            return ERRO; //erro
        }
    }
    cout << "\nMensagem validada com sucesso!";
    return VALIDO;
} //fim do método CamadaEnlaceDadosReceptoraControleDeErroCRC

/** O receptor apenas conta quantos bits "1" existe no quadro que recebeu, 
 * como usamos aqui o esquema de paridade impar e for encontrado um numero par de bits "1"
 * no quadro recebido, o receptor sabera que ocorreu pelo menos um erro de bit.
 * /

/** Funcao que representa o controle de erros por paridade impar da aplicacao receptora */
int CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar (int quadro[]){
    //implementação do algoritmo
    int res = calcUms(quadro, 0, tamBitMsg + CRC);
    if (res % 2 == 0){ // se o resto for 0 quer dizer que a paridade é par
        cout << "Erro! Paridade invalida." << endl; // então informamos o erro
        return ERRO;
    }else{
        /** apesar de a paridade ser valida, ainda pode ocorrer de o dado estar corrupto, o que não é mostrado por esse metodo */
        cout << "Mensagem com paridade valida!"; // caso contrário o resto for 1 a paridade é impar e está tudo certo
        return VALIDO;
    }
    cout << "\nDepois do Bit de Paridade Impar (receptora): ";
    printBits(quadro, 0, tamBitMsg);
}//fim do método CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar

/** O receptor apenas conta a quantos bits "1" existe no quadro que recebeu, 
 * como usamos aqui o esquema de paridade par e for encontrado um numero impar de bits "1"
 * no quadro recebido, o receptor sabera que ocorreu pelo menos um erro de bit.
 *   
 * Mas pode existir erro nao detectado, caso aconteca um erro com numero par de bits.
 * Na condicao de rajadas de erros, a probabilidade de haver erros nao detectados em um quadro protegido
 * por um esquema de paraidade de bit unico pode chegar perto de 50%.
*/
/** Funcao que representa o controle de erros por paridade par da aplicacao receptora. */
int CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (int quadro[]){
    //implementação do algoritmo
    int res = calcUms(quadro, 0, tamBitMsg + CRC);
    if (res % 2 == 0){ // caso o resto for 0 a paridade é par e está tudo certo
        /*apesar de a paridade ser valida, ainda pode ocorrer de o dado estar corrupto, 
        o que nao eh mostrado por esse metodo
        */
        cout << "Mensagem com paridade valida!";
        return VALIDO;
    }else{ // se o resto for 1 quer dizer que a paridade é impar
        cout << "Erro! Paridade invalida." << endl; // então informamos o erro
        return ERRO;
    }
    cout << "\nDepois do Bit de Paridade Par (receptora): ";
    printBits(quadro, 0, tamBitMsg);
}//fim do método CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar


/** Utilizamos essa função para o controle de erro, para isso recebemos os quadros e o tipo de erro */
/** Funcao que determina qual dos algoritmos que controle de erros será utilizado */
int CamadaEnlaceDadosReceptoraControleDeErro (int quadro[], int tipoDeControleDeErro){
    int erro;
    switch (tipoDeControleDeErro){ // Verificamos as diferente possibilidades de erro, e enviamos o quadro
    case 0: //bit de paridade par
        erro = CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(quadro);
        break;
    case 1: //bit de paridade impar
        erro = CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(quadro);
        break;
    case 2: //CRC
        erro = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro);
        break;
    default:
        break;
    }//fim do switch case
    cout << endl;
    return erro;
}//fim do metodo CamadaEnlaceDadosReceptoraControleDeErro

/** Funcao utilizada para chamar a função de erros dentro da camada receptora utilizando cada quadro */
int CamadaEnlaceDadosReceptora(int quadro[], int tipoDeControleDeErro){
    return CamadaEnlaceDadosReceptoraControleDeErro(quadro, tipoDeControleDeErro);
    /** retorna um valor de erro (0 se tiver ok, 1 se tiver erro)*/
}
