#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "forca_biblio.h"

/*
char* secret_word;  //String que contém a palavra exibida durante o jogo (letras ocultas = #)
char* wrong_letter; //String que contém os erros dos jogadores
int tries;            //Contador de erros
*/

//Verifica se o char é válido
//char letter -> char a ser testado
//return 1 -> caso seja um char válido
//return 0 -> caso seja um char inválido
int verify_char(char letter){
    return ((letter >= 'a' && letter <= 'z')||(letter >= 'A' && letter <= 'Z'));
}

//Padroniza o char para letra minuscula
//char letter -> char a ser padronizado
//return -> letra na forma minúscula
char standard_char(char letter){
    return ((letter >= 'A' && letter <= 'Z') ? (letter + ('a' - 'A')) : letter);
}

//Cria a string com letras ocultas
//int height -> tamanho da palavra
//return 0 -> Sucesso
//return -1 -> Erro na alocação de momória
int create_secret(int height){
    tries = 0;
    secret_word = (char*)malloc(height*sizeof(char));
    wrong_letter = (char*)malloc(7 * sizeof(char));
    if(wrong_letter == NULL || secret_word == NULL) return -1;
    for(int i = 0; i < height; i++){
        secret_word[i] = '#';
    }return 0;
}

//Retorna o ponteiro da string com letras ocultas
char* get_secret(){
    return secret_word;
}

//Retorna o ponteiro da string de erros
char* get_wrongs(){
    return wrong_letter;
}

//Retorna o número de erros
int get_wrongtry(){
    return tries;
}

//Verifica se a string é válida
//char* word -> string a ser testada
//return 1 -> caso seja uma string válida (cria o segredo)
//retunr 0 -> caso seja um char inválido
int verify_word(char* word){
    int i = 0;
    for(i = 0; word[i] != 0 && i < MAX_WORD; i++){
        if(verify_char(word[i])){
            word[i] = standard_char(word[i]);
        }else return 0;
    }create_secret(i);
    return 1;
    
}

//Verifica se a jogada foi correta
//char* real_word -> palavra real
//char letter -> char a ser testado
//return 1 -> caso seja um char existente (mostra na palavra secreta)
//retunr 0 -> caso seja um char errado (adiciona na lista de erros, caso não esteja)
int check_letter(char* real_word, char letter){
    int flag = 0;
    for(int i = 0; real_word[i] != 0; i++){
        if(real_word[i] == letter){
            if(i == 0) secret_word[i] = (letter - ('a' - 'A'));
            else secret_word[i] = letter;
            flag = 1;
        }
    }if(flag == 0){
        for(int i = 0; i < tries; i++){
            if(letter == wrong_letter[i]) return flag;
        }wrong_letter[tries] = letter;
        tries++;
    }return flag;
}

//Verifica o status do jogo
//return -1 -> caso não exista um jogo
//return 2 -> caso o jogo tenha acabado (jogador perdeu)
//return 1 -> caso o jogo tenha acabado (jogador venceu)
//retunr 0 -> caso o jogo ainda esteja em andamento
int end_game(){
    if(secret_word == NULL) return -1;
    if(tries == 7) return 2;
    for(int i = 0; secret_word[i] != 0; i++){
        if(secret_word[i] == '#') return 0;
    }return 1;
}