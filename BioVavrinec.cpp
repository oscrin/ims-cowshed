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

// Vyprodukovane mleko.
double MLEKO = 0;

// Mleko prodane zakaznikovi.
double MLEKO_ZAKAZNIK = 0;

// Mleko pro zpracovani.
double MLEKO_ZPRACOVANI = 0;

// Mleko nadojeno za posledni dojeni.
double POSLEDNI_DOJENI = 0;

// Pocet podojenych krav.
unsigned int pocetPodojenychKrav = 0;

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
const int POCET_STANI = 3;

// Pocet dojicek.
const int POCET_DOJICEK = 1;

// Pocet farmaru.
const int POCET_FARMARU = 1;

// Kravy jsou k dispozici pro dojeni.
bool KRAVY_READY = false;

// Indikátor dodojené krávy.
bool DODOJENO = false;

// Dojicka se stara o dojeni a nahaneni.
Store dojicka("Dojicka", POCET_DOJICEK);

// Misto, kde dojicka doji kravu.
Store stani("Mista na dojeni", POCET_STANI);

// Kravy produkujici mleko.
Store farmar("Farmar", POCET_FARMARU);

std::string cas(double t);

class Zpracovani : public Process {
    void Behavior(){
        cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[34mVyrobna potrebuje mleko pro vyrobu masla.\e[39m" <<endl;

            if(MLEKO >= 400){
                cout << "\e[33m>\e[39m \e[2mZ nadrze se odebralo 400 litru mleka.\e[0m" <<endl;
                MLEKO -= 400;
                MLEKO_ZPRACOVANI += 400;
            }else{
                cout << "\e[33m>\e[39m \e[2mZ nadrze se odebral zbytek "<< MLEKO << " litru mleka.\e[0m" <<endl;
                MLEKO_ZPRACOVANI += MLEKO;
                MLEKO = 0;
            }
    }
};

class Krava : public Process {
    int cisloKravy;
public:
    Krava(int cislo) { cisloKravy = cislo; }

    void Behavior() {
        while(1) {
            if (KRAVY_READY) {                  

                    // Dojicka vezme kravu.
                    Enter(dojicka);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " zabrala dojicku.\e[0m" << endl;
                
                    // Dojicka privede kravu na stani.
                    Enter(stani);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " zabrala stani.\e[0m" << endl;
                    
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " vstupuje do dojiciho boxu." << endl;

                    // Doba pro privedeni kravy a napojeni v dojicim boxu.
                    Wait(Uniform(1 * MINUTA, 3 * MINUTA));
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " je pripravovana dojickou.\e[0m" << endl;

                    // Po napojeni se zarizeni spousti a dojicka odchazi.
                    Leave(dojicka);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " uvolnila dojicku.\e[0m" << endl;
                    
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " zacina dojit bez dojicky." << endl;

                    // Dojeni samotne kravy.
                    Wait(Uniform(4 * MINUTA, 6 * MINUTA));
                    double nadojeno = Uniform(8, 10);
                    POSLEDNI_DOJENI += nadojeno;
                    MLEKO += nadojeno; // Produkce mleka.
                    
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " nadojila " << nadojeno << " litru mleka." << endl;
                    
                    // TODO Indikator dodojene kravy - nevyuzit
                    DODOJENO = true;

                    // Farmar odpojuje kravu z dojiciho boxu
                    Enter(farmar);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " zabrala farmare.\e[0m" << endl;

                    // Doba pro odpojeni jedne kravy.
                    Wait(Uniform(2* MINUTA, 3* MINUTA));
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Krava cislo " << cisloKravy << " je uvolnena z boxu farmarem." << endl;

                    // Krava je odvedena z dojicio boxu.
                    Leave(stani);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " opustila stani.\e[0m" << endl;

                    // Farmar odchazi.
                    Leave(farmar);
                    cout << "\e[33m>\e[39m \e[2mKrava " << cisloKravy << " opustila farmare.\e[0m" << endl;

                    // Indikator pro generovani vyroby a premeny krav na operace se stadem.
                    pocetPodojenychKrav++;

                    WaitUntil(KRAVY_READY == false);        
            }
            else {
                // Procesz jsou pozastaveny, pokud jsou kravy seskupeny ve stadu.
                WaitUntil(KRAVY_READY);
                // Mleko z posledniho dojeni je vyresetovano.
                POSLEDNI_DOJENI = 0;   
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

                    // Dojicka privadi kravy z pastvy na farmu.
                    Enter(dojicka);
                    Wait(Uniform(20*MINUTA,30*MINUTA));
                    Leave(dojicka);

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy jsou nahnane." << endl;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Stado ceka na podojeni." << endl;

                    // Indikator pro spusteni vsech procesu krav.
                    KRAVY_READY = true;

                    // Cekani nez se podoji cele stado.
                    WaitUntil(pocetPodojenychKrav == POCET_KRAV);

                    // Ukonceni procesu krav.
                    KRAVY_READY = false;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[32mPodojilo se " << POCET_KRAV << " krav a bylo nadojeno " << POSLEDNI_DOJENI << " litru mleka.\e[39m" << endl;
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[32mV nadrzi je " << MLEKO << " litru mleka.\e[39m" << endl;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy cekaji na farme." << endl;

                    // Technivka prestavka.
                    Wait (10*MINUTA);

                    // Mleko je z farmy odeslano do vyrobny.
                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[95mGeneruji odber mleka do vyrobny.\e[39m" << endl;
                    (new Zpracovani)->Activate();

                    // Kravy odpocivaji na farme a jsou dokrmovany.
                    Wait(Uniform(1*HODINA, 2*HODINA));

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Stado je vypusteno na pastvu." << endl;

                    // Kravy se pasou.
                    Wait(Uniform(4*HODINA, 5*HODINA));

                    pocetPodojenychKrav = 0;
                    PO_PASTVE = true;

                    cout << "\e[33m[" << cas(Time) << "]\e[39m " << "Kravy jsou ready na nahnani." << endl << endl;

                }
                if (!JE_DEN) {
                    // V noci stado ceka na pastve, nez je dovedeno na farmu dojickou.
                    WaitUntil(JE_DEN);
                }
            }
        }
    }
};

Stat denDoba("Denni doba");
Stat nocDoba("Nocni doba");

class GeneratorDenNoc : public Process {
    void Behavior() {
        while (1) {
            // System zacina denni dobou v 06:00 rano.
            cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[31mNa farme se zacina pracovat. (zacina den)\e[39m" << endl;
            Wait(DEN); denDoba(DEN/60);

            // V 19:00 zavira stanek s mlekem na farme
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

class Zakaznik : public Process {
    void Behavior(){
        if(JE_DEN){
            cout << "\e[33m[" << cas(Time) << "]\e[39m " << "\e[95mGeneruji prichazejiciho zakaznika.\e[39m" << endl;
            cout << "\e[33m>\e[39m \e[2mZakaznik vstupuje do obchodu.\e[0m" <<endl;

            if(MLEKO > 0){
                // Zakaznik si bere jednu lahev,
                if (Random() <= 0.8) {
                    cout << "\e[33m>\e[39m \e[2mZakaznik si bere jednu lahev.\e[0m" <<endl;
                    MLEKO_ZAKAZNIK += 1.5;
                    MLEKO = MLEKO - 1.5;
                // Zakaznik si bere dve lahve.
                }else{
                    cout << "\e[33m>\e[39m \e[2mZakaznik si bere dve lahve.\e[0m" <<endl;
                    MLEKO_ZAKAZNIK += 3;
                    MLEKO = MLEKO - 3;
                }
            }else {
                // V nadrzi neni zadne mleko.
                cout << "\e[33m>\e[39m \e[2mNeni mleko pro zakaznika.\e[0m" << endl;
            }
        }
    }
};

// Pomocna promenna - prvni zakaznik nechodi pri startu systemu.
int genZak = 0;

class	GeneratorZakazniku : public Event {
    void Behavior() {
        if (genZak > 0) (new Zakaznik)->Activate();
        genZak++;
        Activate(Time + Exponential(156));
    }
};

int main(int argc, char *argv[]) {

    Init(0,(1*(DEN+NOC))); // 1 den.

    (new GeneratorDenNoc)->Activate();
    (new GeneratorZakazniku)->Activate();

    (new Stado)->Activate();

    // Vygenerovat POCET_KRAV krav do systemu.
    for (int a = 1; a <= POCET_KRAV; a++)
        (new Krava(a))->Activate();

    Run();

    dojicka.Output();
    stani.Output();
    denDoba.Output();
    nocDoba.Output();
}