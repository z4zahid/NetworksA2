XCC = gcc
CFLAGS  = -o

udp: client.o server.o

client.o: udp_client.cc
	$(XCC) $(CFLAGS) udp_client.cc

server.o: udp_server.cc
	$(XCC) $(CFLAGS) udp_server.cc common.cc

clean:
	-rm -f *.o
