all: app

app:
	g++ Aplicacao.cpp EnlaceTransmissao.cpp EnlaceRecepcao.cpp util.cpp -o app
	
runApp: app
	./app

clean:
	rm -f app