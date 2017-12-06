/*
*	PROJECT: BioVavrinec - produkce mléka na biofarme
*
*	FILE: Biovavrinec.cpp - source code
*
*	AUTHORS: Jan Kubica 		(xkubic39) - xkubic39@stud.fit.vutbr.cz
*			 Rostislav Navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu
*/

#define _DEBUG false
// #define _DEBUG true

#include "simlib.h"
#include <stdio.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

// Vystupni soubor.
std::string FILENAME;

// Urcuje pracovni dobu.
bool JE_DEN = true;

// Kravy se dopasly.
bool PO_PASTVE = true;

// Mleko
double MLEKO = 0;

// Mleko prodane zakaznikovi.
double MLEKO_ZAKAZNIK = 0;

// Mleko pro zpracovani.
double MLEKO_ZPRACOVANI = 0;

// Mleko vyprodukovane farmou za den.
double MLEKO_FARMA_DEN = 0;

// Mleko vyprodukovane kravou za den.
double MLEKO_KRAVA_DEN = 0;

// Mleko nadojeno za posledni dojeni.
double POSLEDNI_DOJENI = 0;

// Pocet podojenych krav.
unsigned int pocetPodojenychKrav = 0;

// Kravy s kterymi dojicka zapocala dojeni.
unsigned int pocetDojicichKrav = 0;

// Cas.
const int SEKUNDA = 1;
const int MINUTA = 60 * SEKUNDA;
const int HODINA = 60 * MINUTA;

// Pracovni doba.
const int DEN = 13 * HODINA;

// Nepracovni doba.
const int NOC = 24 * HODINA - DEN;

// Pocet dni simulace.
int POCET_DNI = 365;

// Pocet krav na farme.
int POCET_KRAV = 70;

// Pocet stani pri dojeni.
int POCET_STANI = 3;

// Pocet dojicek.
int POCET_DOJICEK = 1;

// Pocet farmaru.
int POCET_FARMARU = 1;

// Kravy jsou k dispozici pro dojeni.
bool KRAVY_READY = false;

// Dojicka se stara o dojeni a nahaneni.
Store dojicka("Dojicka", POCET_DOJICEK);

// Farmar se stara o vytahovani z dojicich boxu a vyhaneni.
Store farmar("Farmar", POCET_FARMARU);

// Misto, kde dojicka doji kravu.
Store stani("Boxy na dojeni", POCET_STANI);

Queue frontaNaDojeni("Fronta na dojeni");
Queue frontaNaOdpojeni("Fronta na odpojeni");
Queue frontaProStado("Priprava procesu");

Stat casNaStani("Jak dlouho ceka dojicka s kravou na stani?");
Stat mlekoFarmaNaDen("Kolik mleka se denne vyprodukuje na farme?");
Stat dobaDojeni("Jak dlouho trva dojeni stada?");
Stat vypusteniKrav1("V kolik bylu kravy vypusteny na pastvu poprve?");
Stat vypusteniKrav2("V kolik bylu kravy vypusteny na pastvu podruhe?");
Stat mlekoNaDojeniStada("Kolik mleka se vyprodukuje po podojeni stada?");

Histogram dojickaCekaHist("Jak moc ceka dojicka na volne misto v boxu?",0,10,52);
Stat dojickaCekaStat();

std::string cas(double t, bool day, bool corr);

class Zakaznik : public Process {
    void Behavior(){
        if(JE_DEN){
#if _DEBUG
            cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[95mGeneruji prichazejiciho zakaznika.\e[39m" << endl;
            cout << "\e[33m>\e[39m \e[2mZakaznik vstupuje do obchodu.\e[0m" <<endl;
#endif
            if(MLEKO > 0){
                // Zakaznik si bere jednu lahev,
                if (Random() <= 0.8) {
#if _DEBUG                   
                    cout << "\e[33m>\e[39m \e[2mZakaznik si bere jednu lahev.\e[0m" <<endl;
                    MLEKO_ZAKAZNIK += 1.5;
                    MLEKO = MLEKO - 1.5;
#endif
                // Zakaznik si bere dve lahve.
                }else{
#if _DEBUG
                    cout << "\e[33m>\e[39m \e[2mZakaznik si bere dve lahve.\e[0m" <<endl;
#endif
                    MLEKO_ZAKAZNIK += 3;
                    MLEKO = MLEKO - 3;
                }
            }else {
                // V nadrzi neni zadne mleko.
#if _DEBUG
                cout << "\e[33m>\e[39m \e[2mNeni mleko pro zakaznika.\e[0m" << endl;
#endif
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
        Activate(Time + Exponential(156*MINUTA));
    }
};


class Zpracovani : public Process {
    void Behavior(){
#if _DEBUG
        cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[34mVyrobna potrebuje mleko pro vyrobu masla.\e[39m" <<endl;
#endif
            if(MLEKO >= 400){
#if _DEBUG
                cout << "\e[33m>\e[39m \e[2mZ nadrze se odebralo 400 litru mleka.\e[0m" <<endl;
#endif
                MLEKO -= 400;
                MLEKO_ZPRACOVANI += 400;
            }else {
#if _DEBUG
                cout << "\e[33m>\e[39m \e[2mZ nadrze se odebral zbytek "<< MLEKO << " litru mleka.\e[0m" <<endl;
#endif
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

                dojicka.SetQueue(frontaNaDojeni);

                Enter(dojicka);
                double time = Time;

                Enter(stani);
                casNaStani(Time - time);
                dojickaCekaHist(Time - time);
#if _DEBUG
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[2mKrava " << cisloKravy << " zabrala dojicku.\e[0m" << endl;
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[2mKrava " << cisloKravy << " zabrala stani.\e[0m" << endl;
    
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " vstupuje do dojiciho boxu." << endl;
    
                cout<< "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " zacina byt pripravovana na dojeni." << endl;
#endif    
                Wait(Uniform(1 * MINUTA, 3 * MINUTA)); // Priprava kravy na dojeni
#if _DEBUG    
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " je pripravena dojickou." << endl;
#endif    
                Leave(dojicka); // dojicka odchazi
#if _DEBUG    
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[2mKrava " << cisloKravy << " uvolnila dojicku.\e[0m" << endl;
    
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " zacina dojit bez dojicky." << endl;
#endif
                // Dojeni samotne kravy.
                Wait(Uniform(4 * MINUTA, 6 * MINUTA));
                double nadojeno = Uniform(7.5, 10);
                POSLEDNI_DOJENI += nadojeno;
                MLEKO += nadojeno; // Produkce mleka.
                MLEKO_FARMA_DEN += nadojeno;
                
#if _DEBUG
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[36mKrava " << cisloKravy << " nadojila " << nadojeno << " litru mleka.\e[39m" << endl;
#endif
                farmar.SetQueue(frontaNaOdpojeni);

                Enter(farmar);
#if _DEBUG
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " zacala byt uvolnovana z boxu dojickou." << endl;
#endif                
                Wait(Uniform(2* MINUTA, 3* MINUTA));
#if _DEBUG                
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Krava " << cisloKravy << " je uvolnena z boxu dojickou." << endl;
#endif
                Leave(farmar);
#if _DEBUG               
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[2mKrava " << cisloKravy << " uvolnila dojicku podruhe.\e[0m" << endl;
#endif
                Leave(stani);
#if _DEBUG
                cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[2mKrava " << cisloKravy << " uvolnila stani.\e[0m" << endl;
#endif    
                pocetPodojenychKrav++; // pro WaitUntil ve stadu.

                WaitUntil(!KRAVY_READY);
            }
            else {
                // Procesy jsou pozastaveny, pokud jsou kravy seskupeny ve stadu.
                WaitUntil(KRAVY_READY);
    
                // Mleko z posledniho dojeni je vyresetovano.
                POSLEDNI_DOJENI = 0; 
            }             
        }
    }
};

// Pomocna promenna pro rozliseni prvniho a druheho vyhnani na pastvu.
int pom = 1;

class Stado : public Process{
    void Behavior() {
        while(true) {
            if(JE_DEN) {

                // Nahaneni krav.
                if(dojicka.Empty() && PO_PASTVE){
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Dojicka nahani kravy." << endl;
#endif
                    PO_PASTVE = false;

                    Enter(dojicka);
                    Wait(Uniform(20*MINUTA,30*MINUTA));
                    Leave(dojicka);
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Kravy jsou nahnane." << endl;

                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Stado ceka na podojeni." << endl;
#endif
                    KRAVY_READY = true;
                    double time = Time;

                    // Cekani nez se podoji cele stado.
                    WaitUntil(pocetPodojenychKrav == POCET_KRAV);

                    dobaDojeni(Time - time);
                    mlekoNaDojeniStada(POSLEDNI_DOJENI);
                    KRAVY_READY = false;
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[32mPodojilo se " << POCET_KRAV << " krav a bylo nadojeno " << POSLEDNI_DOJENI << " litru mleka.\e[39m" << endl;
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[32mV nadrzi je " << MLEKO << " litru mleka.\e[39m" << endl;
#endif
                    pocetPodojenychKrav = 0;
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Kravy cekaji na farme." << endl;
#endif
                    // Mleko je z farmy odeslano do vyrobny.
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[95mGeneruji odber mleka do vyrobny.\e[39m" << endl;
#endif
                    (new Zpracovani)->Activate();

                    // Kravy jsou na farme.
                    Wait(Uniform(1*HODINA, 2*HODINA));

                    if (pom == 1) {
                        vypusteniKrav1(int(Time) % (DEN+NOC));
                        pom++;
                    }
                    else if (pom == 2) {
                        vypusteniKrav2(int(Time) % (DEN+NOC));
                        pom = 1;
                    }
                    
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Stado je vypusteno farmarem na pastvu." << endl;
#endif
                    // Kravy se pasou.
                    Wait(Uniform(4*HODINA, 5*HODINA));
                    PO_PASTVE = true;
#if _DEBUG
                    cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "Kravy jsou ready na nahnani." << endl << endl;
#endif
                }
                if (!JE_DEN) {
                    WaitUntil(JE_DEN);
                }
            }
        }
    }
};

class GeneratorDenNoc : public Process {
    void Behavior() {
        while (1) {
#if _DEBUG
            cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[31mNa farme se zacina pracovat. (zacina den)\e[39m" << endl;
#endif
            Wait(DEN);
            JE_DEN = !JE_DEN;
#if _DEBUG
            cout << "\e[33m[" << cas(Time, true, true) << "]\e[39m " << "\e[31mObchod na farme zavira. (zacina noc)\e[39m" << endl;
#endif
            Wait(NOC);
            JE_DEN = !JE_DEN;
            mlekoFarmaNaDen(MLEKO_FARMA_DEN);
            MLEKO_FARMA_DEN = 0;
        }
    }
};

std::string cas(double t, bool day, bool corr) {
    int s = int(t) % MINUTA;
    int m = int(t) % HODINA / MINUTA;

    int d = (int(t)) / (DEN+NOC) + 1;

    int h;
    // Casova korekce (6 hodin je pri startu systemu).
    if (corr)
        h = ((int(t)) % (DEN+NOC) / HODINA + 6) % 24;
    else
        h = ((int(t)) % (DEN+NOC) / HODINA) % 24;

    std::string sh;
    std::string sm;
    std::string ss;
    if (h < 10) { sh = "0" + std::to_string(h); } else {sh = std::to_string(h);}
    if (m < 10) { sm = "0" + std::to_string(m); } else {sm = std::to_string(m);}
    if (s < 10) { ss = "0" + std::to_string(s); } else {ss = std::to_string(s);}

    if (day)
        return "Day:(" + std::to_string(d) + ")|" + sh + ":" + sm + ":" + ss;
    else
        return sh + ":" + sm + ":" + ss;
}

int main(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        switch (i) {
            case 1 : { POCET_DNI = atoi(argv[i]); break;}
            case 2 : { POCET_KRAV = atoi(argv[i]); break;}
            case 3 : { POCET_STANI = atoi(argv[i]); break;}
            case 4 : { POCET_DOJICEK = atoi(argv[i]); break;}
            case 5 : { POCET_FARMARU = atoi(argv[i]); break;}
            case 6 : { FILENAME = (argv[i]); break;}
        }
    }

    // Dojicka se stara o dojeni a nahaneni.
    dojicka.SetCapacity(POCET_DOJICEK);

    // Farmar se stara o vytahovani z dojicich boxu a vyhaneni.
    farmar.SetCapacity(POCET_FARMARU);

    // Misto, kde dojicka doji kravu.
    stani.SetCapacity(POCET_STANI);

    SetOutput(FILENAME.c_str());

    Init(0,(POCET_DNI*(DEN+NOC))); // 7 dni.
    (new GeneratorDenNoc)->Activate();
    (new GeneratorZakazniku)->Activate();

    (new Stado)->Activate();

    // Vygenerovat 85 krav do systemu.
    for (int a = 1; a <= POCET_KRAV; a++)
        (new Krava(a))->Activate();

    Run();

    // Statistiky
    frontaNaDojeni.Output();
    frontaNaOdpojeni.Output();
    mlekoFarmaNaDen.Output();
    mlekoNaDojeniStada.Output();
    dobaDojeni.Output();
    Print("Minimalni doba dojeni = %s\n", cas(dobaDojeni.Min(), false, false).c_str());
    Print("Maximalni doba dojeni = %s\n", cas(dobaDojeni.Max(), false, false).c_str());
    Print(" Prumerna doba dojeni = %s\n", cas(dobaDojeni.MeanValue(), false, false).c_str());
    Print("             Odchylka = %s\n", cas(dobaDojeni.StdDev(), false, false).c_str());
    casNaStani.Output();
    Print("Minimalni doba cekani = %s\n", cas(casNaStani.Min(), false, false).c_str());
    Print("Maximalni doba cekani = %s\n", cas(casNaStani.Max(), false, false).c_str());
    Print(" Prumerna doba cekani = %s\n", cas(casNaStani.MeanValue(), false, false).c_str());
    Print("             Odchylka = %s\n", cas(casNaStani.StdDev(), false, false).c_str());
    dojickaCekaHist.Output();

    // Prumerne casy vypusteni
    Print(" Prumerny cas prvniho vypusteni krav = %s\n", cas(vypusteniKrav1.MeanValue(), false, true).c_str());
    Print(" Prumerny cas druheho vypusteni krav = %s\n\n", cas(vypusteniKrav2.MeanValue(), false, true).c_str());

    Print(" Pocet dojicek = %d\n", POCET_DOJICEK);
    Print(" Pocet farmaru = %d\n", POCET_FARMARU);
    Print("    Pocet boxu = %d\n", POCET_STANI);
    Print("    Pocet krav = %d\n\n", POCET_KRAV);

    return 0;
}