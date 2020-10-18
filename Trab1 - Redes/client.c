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
#define PORT 3020

char sendBuffer[MESSAGE_BUFFER];
char receiveBuffer[MESSAGE_BUFFER];
char username[USERNAME_BUFFER];

struct messageInfo{
    int fd; //descritor de socket
    char user[USERNAME_BUFFER]; //nome de usuario
    char word[MAX_WORD]; //palavra da forca
    int opt; // 0 = entrou no chat, 1 = saiu do chat, 2 = enviar mensagem e 3 = jogar
    int onLineNum; // qtd de usuarios online
};
struct messageInfo *sentMessage, *receivedMessage;

int socket_fd;

void getWord(char str[], char word[]){
    int i = 0;
    for(int j = 6; j < strlen(str); j++){
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
            if(strncmp(sendBuffer + sizeof(struct messageInfo), "/play", 5) == 0){
                getWord(sendBuffer + sizeof(struct messageInfo), sentMessage->word);
                printf("%s\n", sentMessage->word);
                sentMessage->opt = 3; //jogar
                printf("Jogar forca!\n");
            }else{
                sentMessage->opt = 2; //envia mensagem para o servidor
                printf("%s> ", username);
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
            create_secret(strlen(receivedMessage->word));
            printf("%s\n", secret_word);
        }
        fflush(stdout);
    }
    // Fecha o socket e mata a thread
    atexit(checkEnd);

    return 0;
}