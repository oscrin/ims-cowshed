#	PROJECT: BioVavrinec - produkce mléka na biofarme
#
#	FILE: Makefile - GNU Make commands
#
#	AUTHORS: Jan Kubica 		(xkubic39) - xkubic39@stud.fit.vutbr.cz
#		 Rostislav navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu

CC = g++
CFLAGS = -g -O2
PFLAGS = -lsimlib -lm

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
# parametry : pocet dni / pocet krav / pocet stani / pocet dojicek / pocet farmaru / vystupni soubor
	./BioVavrinec 365 70 3 1 1 vychozi.out
	./BioVavrinec 365 86 3 1 1 experiment1_86k311.out
	./BioVavrinec 365 90 3 2 2 experiment2_90k322.out
	./BioVavrinec 365 113 6 1 1 experiment3_113k611.out
zip:
	zip 03_$(AUTHORS).zip $(FILES)

targz:
	tar -zcf 03_$(AUTHORS).tar.gz $(FILES)

tar:
	tar -cf 03_$(AUTHORS).tar $(FILES)


