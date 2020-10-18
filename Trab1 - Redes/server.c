#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "forca_biblio.h"

#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10
#define PORT 3020

struct msgHeader{
    int fd;  //descritor de socket
    char user[USERNAME_BUFFER]; //nome de usuario
    char word[MAX_WORD]; //palavra da forca
    int opt;  // 0 = entrou no chat, 1 = saiu do chat, 2 = enviar mensagem e 3 = jogar
    int onLineNum;  //qtd de usuarios online
};

struct msgHeader *sendMsg, *receiveMsg;
int qtdOn = 0; //quantidade de usuarios online no servidor
int socket_fd; //descritor do socket
int clients[10];
char sendBuffer[MESSAGE_BUFFER];
char receiveBuffer[MESSAGE_BUFFER];


/** Função a ser chamada antes de um exit, que fechará o socket */
void checkEnd(){
    close(socket_fd);
}

/** Função para enviar a mensagem para os demais clientes */
void messageClient(char *buffer, int fd){
    for(int i = 0; i < qtdOn; i++){
        if(clients[i] == fd)
            continue;
        if(send(clients[i], buffer, MESSAGE_BUFFER, 0) == -1){
            printf("%s\n", strerror(errno));
        }
    }
    bzero(buffer, MESSAGE_BUFFER);
}

// Receive client message thread
void *receive(void *arg){
    int fd = *(int *)arg; //converte para int

    while(1){
        if(recv(fd, receiveBuffer, MESSAGE_BUFFER, 0) == -1){
            printf("Erro: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
        receiveMsg = (struct msgHeader *)receiveBuffer;
        receiveMsg->fd = fd;
        if(receiveMsg->opt == 1){
            qtdOn--;
            receiveMsg->onLineNum = qtdOn;
            printf("Usuario com ID %d desconectou.\n", fd);
            messageClient(receiveBuffer, fd);
            close(fd);
            /*if(qtdOn == 0){
                atexit(checkEnd);
                exit(1);
            }*/
            return NULL;
        }
        else if (receiveMsg->opt == 2){
            //Manda mensagem para os outros clientes
            messageClient(receiveBuffer, fd);
        }
        else if (receiveMsg->opt == 3){
            printf("oi\n");
            messageClient(receiveBuffer, fd);
        }

    }
    close(fd);
    return NULL;
}

int main() {
    int port, clientFD;
    struct sockaddr_in address, cl_addr;
    socklen_t length;
    pthread_t thread;

    //printf("%d", sizeof(clients));

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    //verifica se o socket foi criado com sucesso
    if(socket_fd == -1){
        printf("Erro: %s.\n", strerror(errno));
        atexit(checkEnd);
        exit(1);
    }
    port = PORT;
    address.sin_family = AF_INET;
    address.sin_port = htons(port); //Define the port at which the server will listen for connections.
    address.sin_addr.s_addr = INADDR_ANY;

    //verifica se houve erro no bind
    if(bind(socket_fd, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1){
        printf("Erro - Bind: %s.\n", strerror(errno));
        atexit(checkEnd);
        exit(1);
    }

    //verifica se houve erro no listen
    if(listen(socket_fd, 100) == -1){
        printf("Erro - Listen: %s\n.", strerror(errno));
        atexit(checkEnd);
        exit(1);
    }
    
    printf("Porta conectada: %d\n", port);
    printf("Esperando...\n");

    while(1){
        if((clientFD = accept(socket_fd, (struct sockaddr *)&cl_addr, &length)) == 0){
            printf("Erro - Accept: %s\n", strerror(errno));
            continue;
        }
        else if (clientFD == -1){
            printf("Erro - FD: %s\n", strerror(errno));
            atexit(checkEnd);
            exit(1);
        }

        clients[qtdOn] = clientFD; //armazena o descritor de socket num vetor
        qtdOn++;
        //printf("%d\n", qtdOn);
        printf("Cliente com socket %d esta conectado!\n", clientFD);

        sendMsg = (struct msgHeader *)sendBuffer; 
        sendMsg->fd = clientFD;
        sendMsg->opt = 0;
        sendMsg->onLineNum = qtdOn;

        //Avisa todos no servidor que um novo usario está conectado
        for(int i = 0; i < qtdOn; i++){
            if(send(clients[i], sendBuffer, MESSAGE_BUFFER, 0) == -1)
                printf("Erro - Send: %s.\n", strerror(errno));
        }
        bzero(sendBuffer, MESSAGE_BUFFER); //seta bytes para 0

        //Thread para processar mensagens recebidas
        if(pthread_create(&thread, NULL, receive, &clientFD) != 0){
            printf("Erro ao criar thread.\n");
            atexit(checkEnd);
            exit(1);
        }
    }
    atexit(checkEnd);
    
    return 0;
}