CC = g++
CPPFLAGS = -Wall -g -I/opt/homebrew/opt/openssl@3/include
LDFLAGS= -L/opt/homebrew/opt/openssl@3/lib

build: ufsend.o ufrec.o utils.o crypto.o network.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufsend ufsend.o utils.o crypto.o network.o -lcrypto
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufrec ufrec.o utils.o crypto.o network.o -lcrypto

ufsend.o: ufsend.cpp
	$(CC) $(CPPFLAGS) -c ufsend.cpp

ufrec.o: ufrec.cpp
	$(CC) $(CPPFLAGS) -c ufrec.cpp

utils.o: utils.cpp
	$(CC) $(CPPFLAGS) -c utils.cpp

crypto.o: crypto.cpp
	$(CC) $(CPPFLAGS) -c crypto.cpp

network.o: network.cpp
	$(CC) $(CPPFLAGS) -c network.cpp

clean: 
	rm -f *.o
	rm -f ufsend
	rm -f ufrec
	rm -f *.ufsec