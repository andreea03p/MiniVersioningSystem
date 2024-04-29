#include <stdio.h>
#include <stdlib.h>
#include <string.h>

risk

typedef struct number
{
    int value;
    int ok;

} number;

number *citire(number *array, int n)
{
    array = calloc(n, sizeof(number));
    if (array == NULL)
    {
        perror("eroare alocare array\n");
        exit(-1);
    }

    for (int i = 0; i < n; ++i)
    {
        scanf("%d", &array[i].value);
        array[i].ok = 1;
    }

    return array;
}

int main()
{
    int n;
    scanf("%d", &n);

    number *array = NULL;
    array = citire(array, n);

    int ct = 0;
    int current;
    for (int i = 0; i < n; ++i)
    {
        if (array[i].ok != 0)
        {
            current = array[i].value;
            array[i].ok = 0;
            for (int j = i + 1; j < n; ++j)
            {
                if (array[j].value > current && array[j].ok != 0)
                {
                    array[j].ok = 0;
                    current = array[j].value;
                }
            }
            ct++;
        }
        else
        {
            continue;
        }
    }

    printf("%d\n", ct);

    free(array);
}


