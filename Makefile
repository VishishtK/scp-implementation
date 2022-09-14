CC = g++
CPPFLAGS = -Wall -g -I/opt/homebrew/opt/openssl@1.1/include
LDFLAGS= -L/opt/homebrew/opt/openssl@1.1/lib

build: ufsend.o ufrec.o utils.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufsend ufsend.o utils.o -lcrypto
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o ufrec ufrec.o utils.o -lcrypto

ufsend.o: ufsend.cpp
	$(CC) $(CPPFLAGS) -c ufsend.cpp

ufrec.o: ufrec.cpp
	$(CC) $(CPPFLAGS) -c ufrec.cpp

utils.o: utils.cpp
	$(CC) $(CPPFLAGS) -c utils.cpp

clean: 
	rm -f *.o
	rm -f ufsend
	rm -f ufrec
	rm -f *.ufsec