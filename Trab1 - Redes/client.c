#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10
#define PORT 8080

char sendbuf[MESSAGE_BUFFER];
char recvbuf[MESSAGE_BUFFER];
char username[USERNAME_BUFFER];

struct messageInfo{
    int fd;                     //descritor de socket
    char user[USERNAME_BUFFER]; //nome de usuario
    ushort tip;                 // 0 = entrou no chat, 1 = saiu do chat e 2 = enviar mensagem
    ushort onLineNum;           // qtd de usuarios online

};
struct messageInfo *sentMessage, *receivedMessage;

int socket_fd;

// Connect to server
void *connectServer(int fd, struct sockaddr_in *address){
    int response = connect(fd, (struct sockaddr *)address, sizeof(*address));
    if(response < 0){
        printf("Erro: %s\n", strerror(errno));
        exit(1);
    }
    else
        printf("Conectado em %s:%d\n", inet_ntoa(address->sin_addr), ntohs(address->sin_port));
}

void *receive(void *msg){
    while(1){
        sentMessage = (struct messageInfo *)sendbuf;
        sentMessage->fd = socket_fd;
        strcpy(sentMessage->user, username);
        
        printf("%s> ", username);
        fgets(sendbuf + sizeof(struct messageInfo), MESSAGE_BUFFER - sizeof(struct messageInfo), stdin);
        
        if(strncmp(sendbuf + sizeof(struct messageInfo), "end", 3) == 0){    // User leaves the chat room
            sentMessage->tip = 1;      
            if(send(socket_fd, sendbuf, MESSAGE_BUFFER, 0) == -1){
                fprintf(stderr, "%s\n", strerror(errno));
            }
            close(socket_fd);
            exit(0);
        }else
            sentMessage->tip = 2; //envia mensagem para o servidor
        
        if(send(socket_fd, sendbuf, MESSAGE_BUFFER, 0) == -1){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        bzero(sendbuf, MESSAGE_BUFFER);
    }
    return NULL;
}

int main(){
    int port;
    struct sockaddr_in address, cl_addr;
    char * server_address;
    int response;
    pthread_t thread;

    // Get user handle
    printf("Nome de usuario: ");
    fgets(username, USERNAME_BUFFER, stdin);
    username[strlen(username) - 1] = 0; // Remove newline char from end of string

    port = PORT;
    address.sin_family = AF_INET;
    address.sin_port = htons(port); //Define the port at which the server will listen for connections.
    address.sin_addr.s_addr = INADDR_ANY;
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);

    connectServer(socket_fd, &address);

    // Cria thread para receber mensagens
    pthread_create(&thread, NULL, receive, NULL);
    
    // Recebe mensagens de outros usuarios
    while(1){
        // limpa o buffer
        bzero(recvbuf, MESSAGE_BUFFER); //seta os bytes para 0
        if(recv(socket_fd, recvbuf, MESSAGE_BUFFER, 0) == -1){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        receivedMessage = (struct messageInfo *)recvbuf;
        if(receivedMessage->tip == 0 && receivedMessage->onLineNum != 0){
            printf("\nUsuario %s entrou no chat\n", receivedMessage->user);
            printf("Quantidade de usarios online: %d\n", receivedMessage->onLineNum);
        }
        else if(receivedMessage->tip == 1){
            printf("\nUsuario %s saiu do chat\n", receivedMessage->user);
            printf("Quantidade de usuarios online: %d\n", receivedMessage->onLineNum);
        }
        else if(receivedMessage->tip == 2)
            fprintf(stdout, "%s> %s\n", receivedMessage->user, recvbuf+sizeof(struct messageInfo));
    }

    // Fecha o socket e mata a thread
    close(socket_fd);
    pthread_exit(NULL);

    return 0;
}