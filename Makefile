CC = g++
CPPFLAGS = -Wall -g -I/opt/homebrew/opt/openssl@1.1/include
LDFLAGS= -L/opt/homebrew/opt/openssl@1.1/lib

build: ufsend.o ufrec.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufsend ufsend.o -lcrypto
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufrec ufrec.o -lcrypto

ufsend.o: ufsend.cpp
	$(CC) $(CPPFLAGS) -c ufsend.cpp

ufrec.o: ufrec.cpp
	$(CC) $(CPPFLAGS) -c ufrec.cpp

clean: 
	rm -f *.o