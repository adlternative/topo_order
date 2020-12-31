all:star_war
star_war:star_war.o
	g++ star_war.o -g -lm -lpthread -lssl -lcrypto  -fsanitize=address -o star_war
star_war.o:sha256.h
	g++ -c star_war.cpp -o star_war.o

clean :
	rm *.o