XCC = g++
CFLAGS  = -c

udp: client server

client: udp_client.o
        $(XCC) udp_client.o -o client

udp_client.o: udp_client.cc
        $(XCC) $(CFLAGS) udp_client.cc

server: common.o udp_server.o
        $(XCC) common.o udp_server.o -o server

udp_server.o: udp_server.cc
        $(XCC) $(CFLAGS) udp_server.cc

common.o: common.cc
        $(XCC) $(CFLAGS) common.cc

clean:
        -rm -f *.o server client