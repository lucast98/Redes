all: client server

client:
	gcc -pthread -o client client.c forca_biblio.c

server:
	gcc -pthread -o server server.c forca_biblio.c
	
run_server: server
	./server 8008
	
run_client: client
	./client 8008
	
clean:
	rm -f client server
