/*
*	PROJECT: BioVavrinec - produkce mléka na biofarme
*
*	FILE: Biovavrinec.cpp - source code
*
*	AUTHORS: Jan Kubica 		(xkubic39) - xkubic39@stud.fit.vutbr.cz
*			 Rostislav Navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu
*/


#include "simlib.h"
#include <stdio.h>

// Cas
const int MINUTA = 60;
const int HODINA = 60*60;

// Pracovni doba.
const int DEN = 13* HODINA;

// Nepracovni doba.
const int NOC = 24* HODINA - DEN;

// Pocet krav na farme.
const int KRAVY = 50;

// Pocet stani pri dojeni.
const int STANI = 3;

// Pocet dojicek.
const int POCET_DOJICEK = 1;


// Dojicka co se stara o dojeni krav.
Store dojicka("Dojicka co doji kravy", POCET_DOJICEK);


class NahaneniKrav : public Process
{
    void Behavior() {

        Enter(dojicka, 1);
        Wait(Uniform(20,30));
        Leave(dojicka, 1);
    }
};


int main(int argc, char *argv[])
{

    dojicka.SetCapacity(1);
    dojicka.Output();

}