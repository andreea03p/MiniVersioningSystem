#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

// added 

// pb 1
/*
typedef struct{
    float nota;
    char nume[20];
}student;


void citire(student *array, int size)
{
    for(int i = 0; i < size; ++i)
    {
        scanf("%f", &array[i].nota);
        getchar();
        fgets(array[i].nume, 20, stdin);
    }
} 

void afisare(student *array, int size)
{
    for(int i = 0; i < size; ++i)
        printf("%g %s", array[i].nota, array[i].nume);
}

int compare(const void *a, const void *b)
{
    const student *p = (const student*)a;
    const student *q = (const student*)b;
    
    int sign = (p->nota) - (q->nota);
        
    if(sign == 0)
        return strcmp(p->nume, q->nume);
    else
        return sign;
}


int main(void)
{
    int n;
    scanf("%d", &n);

    student *rez = (student*)malloc(n*sizeof(student));
    if(rez == NULL)
    {
        perror("Eroare alocare\n");
        exit(-1);
    }

    citire(rez, n);
    qsort(rez, n, sizeof(student), compare);
    afisare(rez, n);

    free(rez);

    return 0;
}
*/



// pb 2
/*
int* arrayAlloc(int n)
{
    int *v = (int*)malloc(n*sizeof(int));
    if(v == NULL)
    {
        perror("Eroare alocare\n");
        exit(-1);
    } 
    return v;
}

void citire(int *v, int n)
{
    for(int i = 0; i < n; ++i)
        scanf("%d", &v[i]);
}

void afisare(int *v, int n)
{
    for(int i = 0; i < n; ++i)
        printf("%d ", v[i]);
    printf("\n");
}


int cond_neg(int a)
{
    if(a<0)
        return 0;
    else
        return 1;
}

void test_cond(int *v, int *n, int(*cond_neg)(int))
{
    int s = *n;

    for(int i = 0; i < s; ++i)
        if(cond_neg(v[i]) == 0)
        {
            for(int j = i; j < s-1; ++j)
                v[j] = v[j+1];
            s -= 1;
            
            if(s != 0)
            {
                int *new_v = (int*)realloc(v, s * sizeof(int));
                if (new_v == NULL)
                {
                    perror("Eroare alocare\n");
                    exit(-1);
                }
                v = new_v;
            }
            i--;
        } 
    *n = s;     
}


int main(void)
{
    int n;
    scanf("%d", &n);

    int *array = arrayAlloc(n);

    citire(array, n);
    test_cond(array, &n, cond_neg);
    afisare(array, n);

    free(array);

    return 0;

}
*/


// pb 3
/*
int compare(const void *a, const void* b)
{
    const float *p = (const float*)a;
    const float *q = (const float*)b;

    return *p - *q;
}

int main(void)
{
    int n;
    scanf("%d", &n);

    if(n<0 || n>10)
    {
        perror("n invalid\n");
        exit(-1);
    }

    float *v = (float*) malloc(n*sizeof(float));
    if(v == NULL)
    {
        printf("eroare alocare memorie\n");
        exit(-1);
    }

    for(int i = 0; i < n; i++)
        scanf("%f", &v[i]);

    qsort(v, n, sizeof(float), compare);

    for(int i = 0; i < n; i++)
        printf("%g ", v[i]);
    printf("\n");

    float x;
    scanf("%f", &x);

    float *result = (float*) bsearch(&x, v, n, sizeof(float), compare);

    if (result == NULL)
    {
        printf("%g not found\n", x);
    }
    else
    {
        printf("%g found at position %ld\n", x, result - v);
    }

    free(v);

    return 0;
}
*/


// pb 4
/*
double tabelare(double x)
{
    return cos(x);
}

void functionT(double a, double b, int n, double(*tabelare)(double))
{
    double r = (b-a)/n;

    for(double i = a; i <= b; i = i+r)
        printf("f(%g) = %g \n", i, tabelare(i));
}

int main(void)
{
    functionT(-1, 1, 10, tabelare);

    return 0;
}
*/



// sub 2.1
/*
double f1(double x)
{
    return (x*x*x+4)/(x*x+5);
}

double f2(double x)
{
    return x+10;
}

double f3(double x)
{
    return 2*x*x-1;
}

double** tabelare(double a, double b, double p, double(*fct)(double))
{
    int size = (b-a)/p + 1;
    double **rez = malloc(size * sizeof(double*));
    if(rez == NULL)
    {
        perror("Eroare alocare m\n");
        exit(-1);
    }
    for(int i = 0; i < size; i++)
    {
        rez[i] = malloc(2*sizeof(double));
        if(rez[i] == NULL)
        {
            perror("Eroare alocare r\n");
            exit(-1);
        }
        rez[i][0] = a+i*p;
        rez[i][1] = fct(a+i*p);
    }
    return rez;
}

void freeMatrix(double **mat, double n)
{
    for(int i = 0; i < n; i++)
        free(mat[i]);
    free(mat);
}

void afisare(double** matrix, double a, double b, double p)
{
    printf("x  |  f(x)\n");
    printf("---------\n");

    for(int i = 0; i < (b-a)/p+1; i++)
    {
        printf("%g  |  %g\n", matrix[i][0], matrix[i][1]);
    }
}

int main(void)
{
    double a, b, p;
    scanf("%lf%lf%lf", &a, &b, &p);

    double **mat1 = tabelare(a, b, p, f1);
    double **mat2 = tabelare(a, b, p, f2);
    double **mat3 = tabelare(a, b, p, f3);

    printf("functia 1 \n");
    afisare(mat1, a, b, p);
    printf("\n");
    printf("functia 2 \n");
    afisare(mat2, a, b, p);
    printf("\n");
    printf("functia 3 \n");
    afisare(mat3, a, b, p);
    printf("\n");

    double n = (b-a)/p+1; 
    freeMatrix(mat1, n);
    freeMatrix(mat2, n);
    freeMatrix(mat3, n);

    return 0;
}
*/


// sub 2.2

int fct1(char *s)
{
    return strlen(s);
}

int fct2(char *s)
{
    char voc[]="aeiouAEIOU";
    int ct = 0;

    for(int i = 0; i < strlen(s); ++i)
        if(strchr(voc, s[i]))
            ct++;

    return ct;
}

int fct3(char *s)
{
    int ct = 0;

    for(int i = 0; i < strlen(s); ++i)
        if(isupper(s[i]))
            ct++;

    return ct;
}

int fct4(char *s)
{
    int a = (int)s[0];
    int b = (int) s[strlen(s)-1];

    return abs(a-b);
}

typedef int (*FuncPtr)(); 


int main(int argc, char *argv[])
{
    FuncPtr farray[10];

    farray[0] = &fct1;
    farray[1] = &fct2;
    farray[2] = &fct3;
    farray[3] = &fct4;


    for(int i = 1; i < 4; i++)
    {
        printf("%d \n", (*farray[0])(argv[i]));
        printf("%d \n", (*farray[1])(argv[i]));
        printf("%d \n", (*farray[2])(argv[i]));
        printf("%d \n", (*farray[3])(argv[i]));
        printf("\n");
    }

    return 0;
}

