all: 
	g++ server.cpp -pthread  -lpqxx -lpq  -o serv.out
	g++ client.cpp -o cli.out
run-server: all
	./serv.out
run-client: all
	./cli.out 127.0.0.1 2907
