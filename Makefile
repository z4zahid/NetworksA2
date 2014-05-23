XCC = g++
CFLAGS  = -c

udp: udpclient udpserver

tcp: tcpclient tcpserver

udpclient: udp_client.o
	$(XCC) udp_client.o -o client

udp_client.o: udp_client.cc
	$(XCC) $(CFLAGS) udp_client.cc

udpserver: common.o udp_server.o
	$(XCC) common.o udp_server.o -o server

udp_server.o: udp_server.cc
	$(XCC) $(CFLAGS) udp_server.cc


tcpclient: tcp_client.o
	$(XCC) tcp_client.o -o client

tcp_client.o: tcp_client.cc
	$(XCC) $(CFLAGS) tcp_client.cc

tcpserver: common.o tcp_server.o
	$(XCC) common.o tcp_server.o -o server

tcp_server.o: tcp_server.cc
	$(XCC) $(CFLAGS) tcp_server.cc


common.o: common.cc
	$(XCC) $(CFLAGS) common.cc

clean:
	-rm -f *.o server client