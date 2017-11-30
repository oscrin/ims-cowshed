/*
*	PROJECT: BioVavrinec - produkce mléka na biofarme
*
*	FILE: Biovavrinec.cpp - source code
*
*	AUTHORS: Jan Kubica 		(xkubic39) - xkubic39@stud.fit.vutbr.cz
*			   Rostislav Navrátil (xnavra57) - xnavra57@stud.fit.vutbr.cz - zastupce tymu
*/


#include "simlib.h"
#include <stdio.h>

bool JE_DEN = true;

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

      if (JE_DEN) {
         Enter(dojicka, 1);
         Wait(Uniform(20,30));
         Leave(dojicka, 1);
      }
    }
};

Stat denDoba("Denni doba");
Stat nocDoba("Nocni doba");

class Den_Noc : public Process {
   void Behavior() {
      double time = Time;
 
      while (1) {
         Wait(DEN); denDoba(DEN/3600);
         JE_DEN = !JE_DEN;
         Wait(NOC); nocDoba(NOC/3600);
         JE_DEN = !JE_DEN;
      }
   }
};

int main(int argc, char *argv[])
{
   Init(0,(7*(DEN+NOC))); // 200 dni casovy ramec

   (new Den_Noc)->Activate();

   Run();

   // dojicka.SetCapacity(1);
   // dojicka.Output();
   denDoba.Output();
   nocDoba.Output();
}