#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "forca_biblio.h"

#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10
#define PORT 3024

char sendBuffer[MESSAGE_BUFFER];
char receiveBuffer[MESSAGE_BUFFER];
char username[USERNAME_BUFFER];

struct messageInfo{
    int fd; //descritor de socket
    char user[USERNAME_BUFFER]; //nome de usuario
    char word[MAX_WORD]; //palavra da forca
    int opt; // 0 = entrou no chat, 1 = saiu do chat, 2 = enviar mensagem, 3 = comecou o jogo e 4 = terminou o jogo
    int onLineNum; // qtd de usuarios online
};
struct messageInfo *sentMessage, *receivedMessage;

int socket_fd;
int gameOwner = 0; //0 = ele nao começou o jogo, 1 = comecou o jogo
int isPlaying = 0; 
char guessedWord[MAX_WORD];

void getWord(char str[], char word[], int start){
    int i = 0;
    for(int j = start; j < strlen(str); j++){
        word[i] = str[j];
        i++;
    }
    word[strlen(word)-1] = '\0';
}

/** Função a ser chamada antes de um exit, que fechará o socket */
void checkEnd(){
    close(socket_fd);
}

/** Função que faz a conexão ao servidor */
void *connectServer(int fd, struct sockaddr_in *address){
    int resp = connect(fd, (struct sockaddr *)address, sizeof(*address));
    if(resp < 0){
        printf("Erro: %s\n", strerror(errno));
        atexit(checkEnd);
        exit(1);
    }
    else{
        printf("Conectado em %s:%d", inet_ntoa(address->sin_addr), ntohs(address->sin_port));
    }
}

void *receive(void *msg){
    while(1){
        sentMessage = (struct messageInfo *)sendBuffer;
        sentMessage->fd = socket_fd;
        strcpy(sentMessage->user, username);
        fgets(sendBuffer + sizeof(struct messageInfo), MESSAGE_BUFFER - sizeof(struct messageInfo), stdin);

        //Usuario encerra sessão com /q
        if(strcmp(sendBuffer + sizeof(struct messageInfo), "/q\n") == 0){
            sentMessage->opt = 1;      
            if(send(socket_fd, sendBuffer, MESSAGE_BUFFER, 0) == -1)
                printf("Erro - Send: %s\n", strerror(errno));
            atexit(checkEnd);
            exit(0);
        }else{
            if(isPlaying == 0 && gameOwner == 0 && strncmp(sendBuffer + sizeof(struct messageInfo), "/play", 5) == 0){
                    getWord(sendBuffer + sizeof(struct messageInfo), sentMessage->word, 6);
                    if(verify_word(sentMessage->word) == 1){
                        sentMessage->opt = 3; //jogar
                        gameOwner = 1;
                        printf("Jogar forca!\n");
                        printf("%s> ", username);
                    }else
                        printf("Digite um palavra valida\n");                            
            }else{
                if(isPlaying == 1 && gameOwner == 0 && strncmp(sendBuffer + sizeof(struct messageInfo), "/guess", 6) == 0){
                    getWord(sendBuffer + sizeof(struct messageInfo), sentMessage->word, 7);
                    if (strlen(sentMessage->word) == 1 && verify_char(sentMessage->word[0])){
                        sentMessage->word[0] = standard_char(sentMessage->word[0]); //deixa minusculo
                        if (check_letter(guessedWord, sentMessage->word[0]) == 1){
                            printf("Acertou.\n");
                            printf("Palavra: \n%s\n", secret_word);
                        }
                        else{
                            printf("Se deu mal\n");
                            printf("Letras erradas: %s\n", wrong_letter);
                            printf("Palavra: \n%s\n", secret_word);
                        }
                        if(end_game()==1){
                            printf("Ganhou!\n");
                            isPlaying = 0;
                            sentMessage->opt = 4;
                        }else if(end_game()==2){
                            printf("Perdeu!\n");
                            isPlaying = 1;
                        }else
                            printf("Você ainda pode errar %d vezes.\n",7-tries);
                        printf("%s> ", username);
                    }else{
                        if(gameOwner == 1 && strncmp(sendBuffer + sizeof(struct messageInfo), "/guess", 6) == 0)
                            printf("Quem manda palavra nao joga\n");
                        else if (strlen(sentMessage->word)-7 != 1){
                            printf("Digitou um teste invalido.\n");
                        }else{
                            sentMessage->opt = 2; //envia mensagem para o servidor
                        }
                        printf("%s> ", username);
                    }
                }
                else{
                    sentMessage->opt = 2; //envia mensagem para o servidor
                    printf("%s> ", username);
                }
            }
        }

        if(send(socket_fd, sendBuffer, MESSAGE_BUFFER, 0) == -1)
            printf("Erro - Send: %s\n", strerror(errno));
        bzero(sendBuffer, MESSAGE_BUFFER); //seta os bytes para 0
    }
    return NULL;
}

int main(){
    int port;
    struct sockaddr_in address, cl_addr;
    char * server_address;
    int response;
    pthread_t thread;

    port = PORT;
    address.sin_family = AF_INET;
    address.sin_port = htons(port); //porta em que o servidor "escutará" as conexões
    address.sin_addr.s_addr = INADDR_ANY;
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    //Obtem nome de usuario
    do{
        fflush(stdin);
        printf("Nome de usuario: ");
        fgets(username, USERNAME_BUFFER+2, stdin);
        if(username[strlen(username)-1] == '\n')
            username[strlen(username) - 1] = 0; //remove \n
        if(strlen(username) > USERNAME_BUFFER){
            printf("Digite um username com %d ou menos caracteres.\n", USERNAME_BUFFER);
            getchar();
        }
    }while(strlen(username) > USERNAME_BUFFER);
    //Conecta ao servidor
    connectServer(socket_fd, &address);

    //Cria thread para receber mensagens
    pthread_create(&thread, NULL, receive, NULL);
    
    //Recebe mensagens de outros usuarios
    while(1){
        //limpa o buffer
        bzero(receiveBuffer, MESSAGE_BUFFER); //seta os bytes para 0
        if(recv(socket_fd, receiveBuffer, MESSAGE_BUFFER, 0) == -1)
            fprintf(stderr, "%s\n", strerror(errno));

        receivedMessage = (struct messageInfo *)receiveBuffer;
        if(receivedMessage->opt == 0 && receivedMessage->onLineNum != 0){
            printf("\nUsuario com ID %d entrou no chat\n", receivedMessage->fd);
            //printf("\nQuantidade de usuarios online: %d \n", receivedMessage->onLineNum);
            printf("%s> ", username);
        }
        else if(receivedMessage->opt == 1){
            printf("\nUsuario com ID %d saiu do chat\n", receivedMessage->fd);
            //printf("\nQuantidade de usuarios online: %d \n", receivedMessage->onLineNum);
            printf("%s> ", username);
        }
        else if(receivedMessage->opt == 2){
            printf("\n%s> %s", receivedMessage->user, receiveBuffer+sizeof(struct messageInfo));
            printf("%s> ", username);
        }
        else if(receivedMessage->opt == 3){
            printf("\nChamada para jogar forca, é isso, vai ter que jogar!\n");
            //inicia a forca
            strcpy(guessedWord, receivedMessage->word);
            create_secret(strlen(receivedMessage->word));
            printf("%s\n", secret_word);
            printf("%s> ", username);
            isPlaying = 1;
        }else{
            printf("Acabou o jogo\n");
            isPlaying = 0;
            gameOwner = 0;
        }
        fflush(stdout);
    }
    // Fecha o socket e mata a thread
    atexit(checkEnd);

    return 0;
}