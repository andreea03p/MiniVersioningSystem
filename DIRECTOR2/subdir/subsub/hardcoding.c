#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/*
Se considera o aplicatie de management al unei companii de transporturi care detine un LinkedList de autobuze. 
Fiecare autobuz are un ArrayList de soferi, fiecare sofer fiind caracterizat prin nume si salar. 
Compania de transporturi si autobuzele contin de asemenea un nume unic. 

Se cere implementarea urmatoarelor metode:

adauga un sofer in interiorul autobuzului
afiseaza informatii despre autobuz (nume autobuz si informatii despre soferi)
adauga un autobuz in lista de autobuze
elimina un autobuz din lista de autobuze
cauta un autobuz dupa nume si adauga soferul in ArrayList-ul acestuia
afiseaza lista de autobuze cu informatii despre fiecare autobuz (nume autobuz si informatii despre soferi)
*/

#define MAX_SIZE 100

typedef struct Driver
{
    char *nume;
    float salary;

}Driver;

typedef struct Bus
{
    char *nume;
    Driver **busesArray;
    int busCount;

    struct Bus *next;

}Bus;

typedef struct Company
{
    char *name;
    Bus *listBus;

}Company;


Bus *createList()
{
    Bus *lista = malloc(MAX_SIZE * sizeof(Bus));
    if(lista == NULL)
    {
        perror("eraore alocare lista autobuze\n");
        exit(-1);
    }

    lista->busesArray = malloc(MAX_SIZE * sizeof(Driver*));
    if(lista->busesArray == NULL)
    {
        perror("eroare alocare array soferi\n");
        exit(-1);
    }

    lista->busesArray[0]->nume = NULL;
    lista->busesArray[0]->salary = 0;

    lista->nume = NULL;
    lista->busCount = 0;
    lista->next = NULL;

    return lista;
}

Bus *addDriver(Driver *sofer, Bus *autobuz)
{
    printf("nume sofer: ");
    scanf("%s", sofer->nume);
    printf("salar sofer: ");
    scanf("%f", &sofer->salary);

    autobuz->busesArray[autobuz->busCount] = sofer;
    //autobuz->busCount++;

    return autobuz;
}

Bus *addBusInList(Bus *newBus, Bus *root)
{
    Bus *p = root;
    while(p->next != NULL)
    {
        p=p->next;
    }

    p->next = newBus;
    return root;
}


int main()
{
    Bus *root = createList();




    return 0;
}