UDPClient : UDPClient.c udp.h
	gcc -Wall -I UDPClient.c -o UDPClient

TCPServer : UDPServer.c udp.h
	gcc -Wall -I UDPServer.c -o UDPServer
