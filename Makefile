#	PROJECT: BioVavrinec - produkce mléka na biofarme
#
#	FILE: Makefile - GNU Make commands
#
#	AUTHORS: Jan Kubica 		(xkubic39) - xkubic39@stud.fit.vutbr.cz
#			 Rostislav navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu

CC = g++
CLAGS=-g -O2 
PFLAGS=-lsimlib -lm

FILE = BioVavrinec
FILES = BioVavrinec.cpp Makefile dokumentace.pdf
AUTHORS = xnavra57_xkubic39

all: BioVavrinec

BioVavrinec: BioVavrinec.cpp
	$(CC) $(CFLAGS) -o $@ $(FILE).cpp $(PFLAGS)

clean:
	rm -f $(FILE) *.tar.gz *.zip *.out

rebuild: clean all

run:
	./BioVavrinec

zip:
	zip 03_$(AUTHORS).zip $(FILES)

targz:
	tar -zcf $(AUTHORS).tar.gz $(FILES)

tar:
	tar -cf $(AUTHORS).tar $(FILES)


