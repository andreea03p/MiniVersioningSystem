// depozit

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Palet
{
    char cod[8];
    struct Palet *next;

}Palet;

typedef struct Depozit
{
    char nume[20];
    struct Palet *paletList;
    struct Depozit *next;

}Depozit;


Depozit* createDepozit(Depozit* urm, char nume[])
{
    Depozit *nou = calloc(1, sizeof(Depozit));
    if(nou == NULL)
    {
        perror("eroare alcoare depozit nou!\n");
        exit(-1);
    }

    strcpy(nou->nume, nume);
    nou->next = urm;
    nou->paletList = NULL;

    return nou;
}

Depozit* endAddDepoToList(Depozit* depo, Depozit *lista)
{
    if(lista == NULL)
    {
        return depo;
    }
    else
    {
        Depozit *p;
        for(p = lista; p->next != NULL; p = p->next);
        p->next = depo;
        return lista;
    }
}

Palet* createPalet(Palet* urm, char cod[])
{
    Palet *nou = calloc(1, sizeof(Palet));
    if(nou == NULL)
    {
        perror("eroare alcoare palet nou!\n");
        exit(-1);
    }

    strcpy(nou->cod, cod);
    nou->next = urm;

    return nou;
}

Depozit* endAddPaletToDepozit(Palet* palet, Depozit *depo)
{
    Palet *newPalet = createPalet(NULL, palet->cod);

    if (depo->paletList == NULL)
    {
        depo->paletList = newPalet;
    }
    else
    {
        Palet *p;
        for (p = depo->paletList; p->next != NULL; p = p->next);
        p->next = newPalet;
    }

    return depo;
}

void deleteByCode(Depozit *lista, char codDelete[])
{
    Depozit *p;
    for (p = lista; p != NULL; p = p->next)
    {
        Palet *prev = NULL;
        Palet *q = p->paletList;

        while (q != NULL)
        {
            if (strcmp(q->cod, codDelete) == 0)
            {
                Palet *temp = q;
                if (prev == NULL)
                {
                    p->paletList = q->next;
                    q = p->paletList;
                }
                else
                {
                    prev->next = q->next;
                    q = prev->next;
                }
                free(temp);
            }
            else
            {
                prev = q;
                q = q->next;
            }
        }
    }
}

void displayList(Depozit *lista)
{
    Depozit *p;
    for (p = lista; p != NULL; p = p->next)
    {
        printf("Nume Depozit: %s\n", p->nume);
        
        Palet *q;
        for (q = p->paletList; q != NULL; q = q->next)
        {
            printf("Cod palet: %s\n", q->cod);
        }
        
        printf("\n");
    }
}

void freeList(Depozit *lista)
{
    Depozit *p;
    while (lista != NULL)
    {
        p = lista->next;
        if (lista->paletList != NULL)
        {
            Palet *q;
            while (lista->paletList != NULL)
            {
                q = lista->paletList->next;
                free(lista->paletList);
                lista->paletList = q;
            }
        }
        free(lista);
        lista = p;
    }
}


int main()
{
    Depozit *myLista = NULL;

    Depozit* d1 = createDepozit(NULL, "AGRO");
    Depozit* d2 = createDepozit(NULL, "UPT");
    Depozit* d3 = createDepozit(NULL, "UVT");

    Palet* p1 = createPalet(NULL, "1111");
    Palet* p2 = createPalet(NULL, "2222");
    Palet* p3 = createPalet(NULL, "3333");
    Palet* p4 = createPalet(NULL, "4444");
    Palet* p5 = createPalet(NULL, "5555");
    Palet* p6 = createPalet(NULL, "6666");
    Palet* p7 = createPalet(NULL, "7777");

    d1 = endAddPaletToDepozit(p1, d1);
    d1 = endAddPaletToDepozit(p2, d1);
    d1 = endAddPaletToDepozit(p3, d1);
    d1 = endAddPaletToDepozit(p6, d1);
    d1 = endAddPaletToDepozit(p7, d1);

    d2 = endAddPaletToDepozit(p1, d2);
    d2 = endAddPaletToDepozit(p2, d2);
    d2 = endAddPaletToDepozit(p3, d2);

    d3 = endAddPaletToDepozit(p5, d3);
    d3 = endAddPaletToDepozit(p6, d3);
    d3 = endAddPaletToDepozit(p4, d3);
    d3 = endAddPaletToDepozit(p3, d3);


    myLista = endAddDepoToList(d1, myLista);
    myLista = endAddDepoToList(d2, myLista);
    myLista = endAddDepoToList(d3, myLista);

    displayList(myLista);

    deleteByCode(myLista, "1111");
    displayList(myLista);

    freeList(myLista);

    return 0;
}