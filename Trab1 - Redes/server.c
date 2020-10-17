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

#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10
#define MAX_USERS 10
#define PORT 3007

struct msgHdr{
    int fd;  //descritor de socket
    char user[USERNAME_BUFFER]; //nome de usuario
    ushort opt;  // 0 = entrou no chat, 1 = saiu do chat e 2 = enviar mensagem
    ushort onLineNum;  // qtd de usuarios online
};

struct msgHdr *sendMsgHdr, *recvMsgHdr;
int qtdOn = 0; //quantidade de usuarios online no servidor
int socket_fd; //descritor do socket
int clients[MAX_USERS];
char sendbuf[MESSAGE_BUFFER];
char recvbuf[MESSAGE_BUFFER];


/** Função a ser chamada antes de um exit, que fechará o socket */
void checkEnd(){
    close(socket_fd);
    //pthread_exit(NULL);
}

void receiveMessage(int fd){
    for(int i = 0; i < (qtdOn+1); i++){
        if(clients[i] == fd){
            for(;i < qtdOn; i++)
                clients[i] = clients[i+1];
            if (qtdOn != 0)
                printf("Usuarios conectados: \n");
            for(int j = 0; j < qtdOn; j++)
                printf("ID: %d\n", clients[j]);
        }
    }
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
        if(recv(fd, recvbuf, MESSAGE_BUFFER, 0) == -1){
            printf("Erro: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
        recvMsgHdr = (struct msgHdr *)recvbuf;
        recvMsgHdr->fd = fd;
        if(recvMsgHdr->opt == 1){
            qtdOn--;
            recvMsgHdr->onLineNum = qtdOn;
            // Move the left socket descriptor away from the online list array
            receiveMessage(fd);
            printf("Usuario com ID %d desconectou.\n", fd);
            messageClient(recvbuf, fd);
            close(fd);
            return NULL;
        }
        // Send this user's message to other users
        messageClient(recvbuf, fd);
    }
    close(fd);
    return NULL;
}

int main() {
    int port, clientFD;
    struct sockaddr_in address, cl_addr;
    socklen_t length;
    pthread_t thread;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    //verifica se o socket foi criado com sucesso
    if(socket_fd == -1){
        printf("Erro1: %s.\n", strerror(errno));
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
    if(listen(socket_fd, MAX_USERS) == -1){
        printf("Erro - Listen: %s\n.", strerror(errno));
        atexit(checkEnd);
        exit(1);
    }
    printf("Porta conectada: %d\n", port);
    printf("Esperando...\n");

    while(1){
        if((clientFD = accept(socket_fd, (struct sockaddr *)&cl_addr, &length)) == 0){
            printf("Erro: %s\n", strerror(errno));
            continue;
        }
        else if (clientFD == -1){
            printf("Erro - FD: %s\n", strerror(errno));
            atexit(checkEnd);
            exit(1);
        }
        clients[qtdOn++] = clientFD; //armazena o descritor de socket num vetor
        printf("Cliente com socket %d esta conectado!\n", clientFD);

        sendMsgHdr = (struct msgHdr *)sendbuf; 
        sendMsgHdr->fd = clientFD;
        sendMsgHdr->opt = 0;
        sendMsgHdr->onLineNum = qtdOn;

        //avisa todos no servidor que um novo usario está conectado
        for(int i = 0; i < qtdOn; i++){
            if(send(clients[i], sendbuf, MESSAGE_BUFFER, 0) == -1)
                printf("Erro4: %s.\n", strerror(errno));
        }
        bzero(sendbuf, MESSAGE_BUFFER); //seta bytes para 0

        // Create a receiving user message processing thread
        if(pthread_create(&thread, NULL, receive, &clientFD) != 0){
            printf("Erro ao criar thread.\n");
            atexit(checkEnd);
            exit(1);
        }
    }
    atexit(checkEnd);
    
    return 0;
}