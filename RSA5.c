#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <gmp.h>
#include <string.h>
#include <math.h>

#define MAXIMALNI_PRVOCISLO 600
#define MINIMALNI_PRVOCISLO 10
#define PORADI_MAXIMALNI 109
#define PORADI_MINIMALNI 1
#define ZAKLADNI_VELIKOST 110
#define VELIKOST_BUFFERU 1024

const char *delici_znak = "|";
const char *jmeno_vstupu = "vstup.txt";
char *jmeno_zasifr = "zasifrovane.txt";
char *jmeno_desifr = "desifrovane.txt";

int Sifrovat_nebo_desifrovat(void);
void ClearBuffer(void);
char NactiZnak(void);
int NactiCislo(void);
int Reset_souboru(char *jmeno_souboru);
int Sifrovani(int enkryptovavane, int e, int n);
int Desifrovani(int zasifrovane, int n, int d);
int Umocnovani_ctvercem(int x, int n);
int Eukliduv_alg(int a, int m);
int Nejvetsi_delitel(int a, int b, int* x, int* y);
int ErasthetonovoSito( int hranice, int* p, int* q, int* e);

int main()
{
  int limit_prvocisla;
  int p;
  int q;
  int n;
  int euler;
  int e;
  int d;
  int x;
  int y;
  int zasifrovavane;
  FILE *vstup = fopen(jmeno_vstupu, "r");
  char buffer[VELIKOST_BUFFERU];
  char *token;
  int i;

  if (Reset_souboru(jmeno_desifr) == -1 || Reset_souboru(jmeno_zasifr) == -1) { //promazani souboru
    return 0;
  }
  if( vstup == NULL ) { //overeni, zda existuje vstup
    fprintf( stderr, "Nepodarilo se otevrit soubor: %s\n", jmeno_vstupu );
    return 0;
  }
  printf("Pro sifrovani zadejte 's' a pro desifrovani 'd' : "); //chceme sifrovat nebo desifrovat
  i = Sifrovat_nebo_desifrovat();
  while (i == 0 ) {     // 0 = error, 1 = sifrovat, 2 = desifrovat
    printf("Neplatny znak.\n");
    i = Sifrovat_nebo_desifrovat();
  }
  if (i == 1 ) { //sifrovani
    printf("Mate uz vygenerovany verejny klic? a/n : ");
    if (NactiZnak() == 'a') {
      printf("n = ");
      n = NactiCislo();
      printf("e = ");
      e = NactiCislo();
    } else {
      //matematika potrebna pro generaci klice
      srand(time(0));       //randomizace pro generator pseudonahodnych cisel
      limit_prvocisla = rand()% MAXIMALNI_PRVOCISLO + MINIMALNI_PRVOCISLO;  //pri kazdem spusteni je jiny vrchni limit, takze vetsi nahodnost
      do {
        if (ErasthetonovoSito(limit_prvocisla, &p, &q, &e) == -1) { //vybrani prvocisla Eratosthenovym sitem
          printf("Nemuze byt alokovana pamet.");    //pokud vrati -1 = error
          return 0;
        }
        n = p * q;
        euler = ((p-1)*(q-1))/(Nejvetsi_delitel(p-1,q-1,&x,&y));
        d = Eukliduv_alg(e, euler);
      } while(euler % e == 0 || d == 0);    //pokud by cislo spocitane eulerovou funkci bylo delitelne beze zbytku e, musi e najit jine kombinace prvocisel
      //vypsani vygenerovaneho klice
      printf("verejny klic: n = %i; e = %i \n",n,e);
      printf("soukromy klic: n = %i; d = %i \n",n,d);
    }
    while( fgets(buffer, VELIKOST_BUFFERU, vstup) != NULL ) {   //sifrovani textu ze souboru pomoci fce Sifrovani
      i = 0;
      while (i != strlen(buffer)) { //rozebrani textu na jednotlive chary a zasifrovani
        zasifrovavane = buffer[i];
        if (Sifrovani(zasifrovavane, e, n) == -1) {
          return 0;
        }
        i++;
      }
    }
  } else {   //desifrovani
    printf("Zadejte soukromy klic: \n");    //uzivatel zada soukromy klic
    printf("n = ");
    n = NactiCislo();
    printf("d = ");
    d = NactiCislo();
    while( fgets(buffer, VELIKOST_BUFFERU, vstup) != NULL ) {   //tokenizace jednotlivych pismen
      token = strtok(buffer, delici_znak);
      while(token != NULL) {
        if (Desifrovani(atoi(token),n, d) == -1) {
          return 0;
        }
        token = strtok( NULL, delici_znak );
      }
    }
  }
  fclose(vstup);
  printf("Operace probehla uspesne.");
  return 0;
}

int Sifrovat_nebo_desifrovat()
{
  char znak = NactiZnak();

  if ( znak == 'd') {
    return 2;
  } else if (znak == 's') {
    return 1;
  }
  return 0;
}

void ClearBuffer()
{
  while ((getchar()) != '\n'); //mozna zbytecna samostatna funkce, ale pro moje zjednoduseni existuje
}
char NactiZnak()        //fce pro nacteni uzivatelova inputu
{
  char prectenyZnak;
  prectenyZnak = getchar();
  ClearBuffer();

  return tolower(prectenyZnak);     //uzivatel muze zadat jak velke 'A', tak male 'a'
}

int NactiCislo()
{
  int cislo;

  scanf("%d", &cislo);
  return cislo;
}

int Reset_souboru(char *jmeno_souboru)  //smazani obsahu souboru
{
  FILE *reset;
  reset = fopen(jmeno_souboru, "w");
  if (reset != NULL) {
  } else {
    printf("Nepodarilo se vymazat obsah souboru.");
    return -1;
  }
  fclose(reset);
  return 0;
}

int Sifrovani(int enkryptovavane, int e, int n) //fce pro sifrovani
{
  FILE *vystupza = fopen(jmeno_zasifr, "a");

  if (vystupza == NULL) {
    printf("Chyba pri zasifrovani do souboru.");
    return -1;
  }

  mpz_t zasifrovane;    //deklarace promenych gmp
  mpz_t enkryptovavane_z;
  mpz_t e_z;
  mpz_t n_z;

  mpz_init(zasifrovane);    //jejich inicializace
  mpz_init(enkryptovavane_z);
  mpz_init(e_z);
  mpz_init(n_z);

  mpz_set_ui(enkryptovavane_z,enkryptovavane);  //nastaveni jejich hodnoty
  mpz_set_ui(e_z,e);
  mpz_set_ui(n_z,n);

  mpz_powm(zasifrovane, enkryptovavane_z, e_z, n_z);    //zasifrovane = enkryptovane_z ^ e_z % n_z
  gmp_fprintf(vystupza, "%Zd%s",zasifrovane, delici_znak);  //vystup do souboru s delicim znakem

  fclose(vystupza);         //uzavreni souboru a vycisteni pameti od promennych
  mpz_clear(zasifrovane);
  mpz_clear(enkryptovavane_z);
  mpz_clear(e_z);
  mpz_clear(n_z);
  return 0;
}

int Desifrovani(int zasifrovane, int n, int d)  //desifrovani, je skoro stejne jako sifrovani
{
  FILE *vystupde = fopen(jmeno_desifr, "a");

  if (vystupde == NULL) {
    printf("Chyba pri desifrovani do souboru.");
    return -1;
  }
  mpz_t zasifrovane_z;
  mpz_t n_z;
  mpz_t desifrovane;
  mpz_t d_z;

  mpz_init(zasifrovane_z);
  mpz_init(n_z);
  mpz_init(desifrovane);
  mpz_init(d_z);

  mpz_set_ui(n_z,n);
  mpz_set_ui(d_z,d);
  mpz_set_ui(zasifrovane_z, zasifrovane);

  mpz_powm(desifrovane, zasifrovane_z, d_z, n_z);
  gmp_fprintf(vystupde, "%c", mpz_get_ui (desifrovane));

  fclose(vystupde);
  mpz_clear(zasifrovane_z);
  mpz_clear(d_z);
  mpz_clear(n_z);
  mpz_clear(desifrovane);
  return 0;
}

int Umocnovani_ctvercem(int x, int n)   //momentalne nepotrebna diky knihovne gmp
{
  if (n < 0) {
    return Umocnovani_ctvercem(1/x,-n);
  } else if (n == 0) {
    return 1;
  } else if (n == 1) {
    return x;
  } else if (n % 2 == 1) {
    return Umocnovani_ctvercem(x*x, n/2);
  } else if (n % 2 == 0) {
    return Umocnovani_ctvercem(x*x, (n - 1) / 2);
  }
  return 0;
}

int Eukliduv_alg(int a, int m)  //spolecne s Nejvetsi_delitel se jedna o rozsireny Eukliduv algoritmus pro nalezeni nejvetsiho spolecneho delitele
{                               //a vypocet kongruence zbytkovych trid, protoze musime najit cislo d, kde plati (d % euler) = 1

  int x;
  int y;
  int g;
  int d;

  g = Nejvetsi_delitel(a, m, &x, &y);   //nejdrive nalezneme nejvetsiho spolecneho delitele cisel 'a' a 'm' (konkretne 'e' a 'euler' v nasem pripade)
  if (g != 1) {
    printf("Reseni neexistuje.");
    return 0;
  } else {
    d = (x % m + m) % m;
    return d;
  }

  return 0;
}

int Nejvetsi_delitel(int a, int b, int* x, int* y)  //rekurzivni funkce pro nalezeni nejvetsiho spolecneho delitele, pointery 'x' a 'y' slouzi pro predani
{                                                   //Bezoutovych koeficientu
  int x1;
  int y1;
  int delitel;

  if (a == 0) {
    *x = 0;
    *y = 1;
    return b;
  }

  delitel = Nejvetsi_delitel(b % a, a, &x1, &y1);   //vzdy se prohodi 'a' a 'b', nove 'a' je rovno zbytku po deleni b/a, tak to pokracuje dokud 'a' = 0

  *x = y1 - (b / a) * x1;   //kdyz se 'a' = 0, tak x = 0 a y = 1, z toho se odzadu dopocitaji vysledne Bezoutovy koeficienty
  *y = x1;                  //pokud je predposledni 'a' = 2, 'b' = 4, tak 'x' = 1 - (4 / 2) * 0 = 1, 'y' = 0 atd...

  return delitel;
}

int ErasthetonovoSito( int hranice, int* p, int* q, int* e)     //funkce pro generaci nahodnych prvocisel pomoci erasthetonova sita
{
  int i;
  int j;
  int k;
  int sito[hranice+1];
  int poradip;
  int poradiq;
  int poradie;
  int* pole_prcisla;
  int velikost = ZAKLADNI_VELIKOST;

  poradip = rand()%PORADI_MAXIMALNI + PORADI_MINIMALNI; //vygeneruji poradi prvocisla p
  for (i = 0; i <= hranice; i++) {  //v poli cisel, kde cisla znaci index pole je vsechny dam rovny 1
    sito[i] = 1;    //kazde cislo je prvocislo, dokud nedokazi opak
  }

  for (i = 2; i < sqrt(hranice); i++) {     //nema smysl prochazet az do horni hranice, staci do jeji odmocniny, protoze x = a * b; 'a' a 'b' < sqrt(x)
    if (sito[i] == 1) {     //pokud je cislo prvocislo
      k = 0;
      for (j = k*i + i*i; j <= hranice; j = k*i + i*i) {    //odstranujeme nasobky prvocisla, zaciname na hodnote cisla na druhou, protoze predchozi nasobky
        sito[j] = 0;        //byly odstraneny v predchozich krocich u predchozich prvocisel
        k++;
      }
    }
  }
  poradiq = rand()%PORADI_MAXIMALNI + PORADI_MINIMALNI; //generace poradi prvocisla q
  pole_prcisla = (int*) malloc(ZAKLADNI_VELIKOST*sizeof(int));  //alokace pameti pro dynamicke pole prvocisel
  if (pole_prcisla == NULL) {  //nepodarilo se alokovat pamet
    return(-1);
  }
  i = 2;
  k = 0;
  while (i != hranice) {    //prepis prvocisel z pole sito[] neni nutny, ale usnadni nam to praci, jelikoz prvocisla vybirame na zaklade jejich poradi
    if (sito[i] == 1) {     //je to jako kdyz si nekdo v duchu pocita a jiny rekne stop a to cislo, na ktero zrovna myslel pocitajici je brane jako nahodne
      *(pole_prcisla + k) = i;
      k++;
      if (k == velikost) {
        pole_prcisla = (int*) realloc(pole_prcisla,(k+ZAKLADNI_VELIKOST)*sizeof(int));  //pokud pole nestaci, tak se alokuje vice pameti
        if (pole_prcisla == NULL) {
          return(-1);
        }
      }
    }
    i++;
  }
  poradie = rand()%PORADI_MAXIMALNI + PORADI_MINIMALNI;     //poradi pro e
  *p = *(pole_prcisla + (poradip % (k - 1)));       //ulozime hodnoty do pointeru, aby se vratili do funkce main()
  *q = *(pole_prcisla + (poradiq % (k - 1)));
  *e = *(pole_prcisla + ((poradie + i) % (k - 1)));

  free(pole_prcisla);   //uvolnime pamet
  return 0;
}


