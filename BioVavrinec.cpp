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
#include <iostream>
#include <string>

using std::cout;
using std::endl;

// Urcuje pracovni dobu.
bool JE_DEN = true;

// Kravy se dopasly.
bool PO_PASTVE = true;

// Mleko
double MLEKO = 0;

// Mleko nadojeno za posledni dojeni.
double POSLEDNI_DOJENI = 0;

// Pocet podojenych krav.
unsigned int pocetPodojenychKrav = 0;

// Kravy s kterymi dojicka zapocala dojeni.
unsigned int pocetDojicichKrav = 0;

// Cas.
const int MINUTA = 1;
const int HODINA = 60*MINUTA;

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

std::string cas(double t);

int cisloKravy = 1;

class Krava : public Process {
    void Behavior() {
        while(1) {
            if (KRAVY_READY) {                  

                Enter(stani);
                Enter(dojicka);
                
                cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " vstupuje do dojiciho boxu." << endl;

                Wait(Uniform(1 * MINUTA, 3 * MINUTA));
                Leave(dojicka);

                cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " doji bez dojicky." << endl;

                // Dojeni samotne kravy.
                Wait(Uniform(4 * MINUTA, 6 * MINUTA));

                double nadojeno = Uniform(8, 10);
                POSLEDNI_DOJENI += nadojeno;
                MLEKO += nadojeno; // Produkce mleka.
                pocetPodojenychKrav++;

                cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " nadojila " << nadojeno << " litru mleka." << endl;
                
                Enter(dojicka);
                Wait(Uniform(2* MINUTA, 3* MINUTA));
                Leave(stani);
                Leave(dojicka);

                cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " je uvolnena z boxu dojickou." << endl;

                cisloKravy++;

                WaitUntil(KRAVY_READY == false);
            }
            else {
              WaitUntil(KRAVY_READY);
              POSLEDNI_DOJENI = 0;
              cisloKravy = 1;   
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

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Dojicka nahani kravy." << endl;

                    PO_PASTVE = false;

                    Enter(dojicka);
                    Wait(Uniform(20*MINUTA,30*MINUTA));
                    Leave(dojicka);

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy jsou nahnane." << endl;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Stado ceka na podojeni." << endl;

                    KRAVY_READY = true;

                    // Cekani nez se podoji cele stado.
                    WaitUntil(pocetPodojenychKrav == 50);

                    KRAVY_READY = false;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[32mPodojilo se 50 krav a bylo nadojeno " << POSLEDNI_DOJENI << " litru mleka.\e[39m" << endl;
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[32mV nadrzi je " << MLEKO << " litru mleka.\e[39m" << endl;

                    pocetPodojenychKrav = 0;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy cekaji na farme." << endl;

                    // Kravy jsou na farme.
                    Wait(Uniform(1*HODINA, 2*HODINA));

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Stado je vypusteno na pastvu." << endl;

                    // Kravy se pasou.
                    Wait(Uniform(4*HODINA, 5*HODINA));
                    PO_PASTVE = true;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy jsou ready na nahnani." << endl << endl;

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
            cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[31mNa farme se zacina pracovat. (zacina den)\e[39m" << endl;
            Wait(DEN); denDoba(DEN/60);
            JE_DEN = !JE_DEN;
            cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[31mObchod na farme zavira. (zacina noc)\e[39m" << endl;
            Wait(NOC); nocDoba(NOC/60);
            JE_DEN = !JE_DEN;
        }
    }
};

std::string cas(double t) {
    int s = (t - int(t))*60;
    int m = int(t) % 60;

    // Casova korekce (6 hodin je rano)
    int h = (int(t/60) + 6) % 24;
    
    std::string sh;
    std::string sm;
    std::string ss;
    if (h < 10) { sh = "0" + std::to_string(h); } else {sh = std::to_string(h);}
    if (m < 10) { sm = "0" + std::to_string(m); } else {sm = std::to_string(m);}
    if (s < 10) { ss = "0" + std::to_string(s); } else {ss = std::to_string(s);}

    return sh + ":" + sm + ":" + ss;
}

int main(int argc, char *argv[]) {

    Init(0,(7*(DEN+NOC))-1); // 7 dni.
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