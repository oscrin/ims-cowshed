/*
*	PROJECT: BioVavrinec - produkce mléka na biofarme
*
*	FILE: Biovavrinec.cpp - source code
*
*	AUTHORS: Jan Kubica 		    (xkubic39) - xkubic39@stud.fit.vutbr.cz
*			     Rostislav Navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu
*/


#include "simlib.h"
#include <stdio.h>
#include <iostream>
using namespace std;

// Urcuje pracovni dobu.
bool JE_DEN = true;

// Kravy se dopasly.
bool PO_PASTVE = true;

// Mleko
double MLEKO = 0;

// Pocet podojenych krav.
unsigned int pocetPodojenychKrav = 0;

// Kravy s kterymi dojicka zapocala dojeni.
unsigned int pocetDojicichKrav = 0;

// Cas.
const int MINUTA = 60;
const int HODINA = 60*60;

// Pracovni doba.
const int DEN = 13* HODINA;

// Nepracovni doba.
const int NOC = 24* HODINA - DEN;

// Pocet krav na farme.
const int POCET_KRAV = 50;

// Pocet stani pri dojeni.
const int STANI = 3;

// Pocet dojicek.
const int POCET_DOJICEK = 1;

// Kravy jsou k dispozici pro dojeni.
bool KRAVY_READY = false;


// Dojicka se stara o dojeni a nahaneni.
Store dojicka("Dojicka", POCET_DOJICEK);

// Misto, kde dojicka doji kravu.
Store stani("Mista na dojeni", STANI);

// Kravy produkujici mleko.
Store kravy("Kravy", POCET_KRAV);


class Krava : public Process {
    void Behavior() {
        while(1) {
            if (KRAVY_READY) {                  
                    
                Enter(stani);
                Enter(dojicka);
                
                cout << "Krava vstupuje do dojiciho boxu." << endl;

                Wait(Uniform(1 * MINUTA, 3 * MINUTA));
                Leave(dojicka);

                cout << "Krava doji bez dojicky." << endl;

                // Dojeni samotne kravy.
                Wait(Uniform(4 * MINUTA, 6 * MINUTA));

                double nadojeno = Uniform(8, 10);
                MLEKO += nadojeno; // Produkce mleka.
                pocetPodojenychKrav++;

                cout << "Krava cislo " << pocetPodojenychKrav << " nadojila " << nadojeno << "litru mleka." << endl;
                
                Enter(dojicka);
                Wait(Uniform(2* MINUTA, 3* MINUTA));
                Leave(stani);
                Leave(dojicka);

                cout << "Krava je uvolnena z boxu dojickou." << endl;

            }
            else {
              WaitUntil(KRAVY_READY);
            }
        }
    }
};

class Stado : public Process{
    void Behavior() {
        while(true) {
            if(JE_DEN) {

                // Nahaneni krav.
                if(dojicka.Empty() && PO_PASTVE){

                    cout << "Dojicka nahani kravy." << endl;

                    PO_PASTVE = false;
                    Enter(dojicka);
                    Wait(Uniform(20*MINUTA,30*MINUTA));
                    Leave(dojicka);

                    cout << "Kravy jsou nahnane." << endl;

                    cout << "Stado ceka na podojeni." << endl;

                    KRAVY_READY = true;

                    // Cekani nez se podoji cele stado.
                    WaitUntil(pocetPodojenychKrav == 50);

                    KRAVY_READY = false;

                    cout << "Podojilo se 50 krav a bylo nadojeno " << MLEKO << " litru mleka." << endl;

                    pocetPodojenychKrav = 0;

                    cout << "Kravy cekaji na farme." << endl;

                    // Kravy jsou na farme.
                    Wait(Uniform(1*HODINA, 2*HODINA));

                    cout << "Stado je vypusteno na pastvu." << endl;

                    // Kravy se pasou.
                    Wait(Uniform(4*HODINA, 5*HODINA));
                    PO_PASTVE = true;

                    cout << "Kravy jsou ready na nahnani." << endl << endl;

                }
                if (!JE_DEN) {
                    WaitUntil(JE_DEN);
                }
            }
        }
    }
};

Stat denDoba("Denni doba");
Stat nocDoba("Nocni doba");

class Den_Noc : public Process {
    void Behavior() {
        while (1) {
            cout << "Je den." << endl;
            Wait(DEN); denDoba(DEN/3600);
            JE_DEN = !JE_DEN;
            cout << "Je noc." << endl;
            Wait(NOC); nocDoba(NOC/3600);
            JE_DEN = !JE_DEN;
        }
    }
};

int main(int argc, char *argv[]) {

    Init(0,(1*(DEN+NOC))); // 7 dni.
    (new Den_Noc)->Activate();

    (new Stado)->Activate();

    // Vygenerovat 50 krav do systemu.
    for (int a = 0; a < POCET_KRAV; a++)
        (new Krava)->Activate();

    Run();

    dojicka.Output();
    stani.Output();
    denDoba.Output();
    nocDoba.Output();
}