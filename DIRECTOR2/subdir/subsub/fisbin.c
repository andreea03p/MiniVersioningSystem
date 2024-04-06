#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


/*
void throwError(char *errorMessage)
{
    fprintf(stderr, "%s:%s\n", errorMessage, strerror(errno));
    // perror(errorMessage);
    exit(errno);
}

void *allocateMemory(size_t memory)
{
    void *p = malloc(memory);
    if (NULL == p)
    {
        throwError("Error at allocating memory");
    }
    return p;
}

void throwErrorFromFile(char *errorMessage, char *fileName)
{
    char tmp[1001];
    snprintf(tmp, 1000, "%s %s", errorMessage, fileName);
    throwError(tmp);
}

FILE *openFile(char *fileName, char *openType)
{
    FILE *file;
    file = fopen(fileName, openType);

    if (NULL == file)
    {
        throwErrorFromFile("Error at opening file: ", fileName);
    }

    return file;
}

void closeFile(FILE *file, char *fileName)
{
    if (fclose(file))
    {
        throwErrorFromFile("Error at closing file: ", fileName);
    }
}

void readFromFile(char *fileName)
{
    FILE *file = openFile(fileName, "r");

    static const int MAX_BUFFER_LENGTH = 1000;
    char buffer[MAX_BUFFER_LENGTH];

    int firstNumber;
    errno = 0;
    if (fscanf(file, "%d\n", &firstNumber) != 1)
    {
        throwErrorFromFile("Error at reading from file ", fileName);
    }

    printf("%d\n", firstNumber);

    while (fgets(buffer, MAX_BUFFER_LENGTH, file))
    {
        printf("%s", buffer);
    }

    if(!feof(file))
    {
        throwErrorFromFile("EOF not reachead: ", fileName);
    }

    if (errno)
    {
        throwErrorFromFile("Errror reading from file: ", fileName);
    }

    closeFile(file, fileName);
}

void writeToFile(char *fileName)
{
    FILE *file = openFile(fileName, "w");
    char *dummyText[] = {"This is a sentence", "This is the second one", "Third one"};

    errno = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (fprintf(file, "%s\n", dummyText[i]) != strlen(dummyText[i]) + 1)
        {
            throwErrorFromFile("Error at writing to file: ", fileName);
        }
    }

    if (errno)
    {
        throwErrorFromFile("Error at writing to file: ", fileName);
    }

    closeFile(file, fileName);
}

void writeToBinaryFile(char *fileName, char *openType, void *elements, size_t elementSize, size_t numberOfElements)
{
    FILE *file = openFile(fileName, openType);

    errno = 0;
    if (fwrite(elements, elementSize, numberOfElements, file) != numberOfElements)
    {
        throwErrorFromFile("Error at writing to binary file: ", fileName);
    }

    if (errno)
    {
        throwErrorFromFile("Error at writing to binary file: ", fileName);
    }

    closeFile(file, fileName);
}

void copy(char *sourceFilePath, char *destinationFilePath, size_t elementSize)
{
    FILE *sourceFile = openFile(sourceFilePath, "rb");
    size_t numberOfElementsRead = 1;
    void *element = allocateMemory(elementSize * numberOfElementsRead);

    errno = 0;

    while (fread(element, elementSize, numberOfElementsRead, sourceFile) == numberOfElementsRead)
    {
        writeToBinaryFile(destinationFilePath, "ab", element, elementSize, numberOfElementsRead);
    }

    if (!feof(sourceFile))
    {
        throwErrorFromFile("EOF not reached ", sourceFilePath);
    }

    if (errno)
    {
        throwErrorFromFile("Error at writing to binary file: ", destinationFilePath);
    }

    free(element);

    closeFile(sourceFile, sourceFilePath);
}

void basicTesting()
{
    int n = 0x1177AAEE;
    writeToBinaryFile("1.dat", "wb", &n, sizeof(int), 1);
    writeToBinaryFile("1.dat", "a+", &n, sizeof(int), 1);
    copy("1.dat", "2.dat", sizeof(int));
}

int main()
{
    printf("Hello World!\n");
    readFromFile("file_in.txt");
    writeToFile("file_out.txt");
    basicTesting();

    return 0;
}
*/



// apl 5.3
/*
int** citire(int m, int n)
{
    int **a = malloc(m*sizeof(int*));
    if(a == NULL)
    {
        fprintf(stderr, "eroare alocare matrice\n");
        exit(-1);
    }
    for(int i = 0; i < m; ++i)
    {
        a[i] = malloc(n*sizeof(int));
        if(a[i] == NULL)
        {
            fprintf(stderr, "eroare alocare linie\n");
            exit(-1);
        }
    }
    
    printf("introduceti elem matricei:\n");
    for(int i = 0; i < m; ++i)
    {
        for(int j = 0; j < n; ++j)
            scanf("%d", &a[i][j]);
    }

    return a;
}

void freeMatrix(int **a, int m)
{
    for(int i = 0; i < m; ++i)
        free(a[i]);
    free(a);
}

void writeToBinaryFile(char *filename, int **a, int m, int n)
{
    FILE *f = fopen(filename, "wb");
    if(f == NULL)
    {
        fprintf(stderr, "eroare deschidere fisier binar\n");
        exit(-1);
    }

    fwrite(&m, sizeof(int), 1, f);
    fwrite(&n, sizeof(int), 1, f);

    for (int i = 0; i < m; i++) 
    {
        fwrite(a[i], sizeof(int), n, f);
    }

    fclose(f);
}

void readFromBinaryFile(char *filename)
{
    FILE *fb = fopen(filename, "rb");
    if(fb == NULL)
    {
        fprintf(stderr, "eroare deschidere fisier binar\n");
        exit(-1);
    }

    int mb, nb;
    fread(&mb, sizeof(int), 1, fb);
    fread(&nb, sizeof(int), 1, fb);

    int **b = malloc(mb*sizeof(int*));
    if(b == NULL)
    {
        fprintf(stderr, "eroare alocare matrice binar\n");
        exit(-1);
    }

    for(int l = 0; l < mb; ++l)
    {
        b[l] = malloc(nb*sizeof(int));
        if(b[l] == NULL)
        {
            fprintf(stderr, "eroare alocare linie binar\n");
            exit(-1);
        }
    }

    for(int l = 0; l < mb; ++l)
    {
        for(int c = 0; c < nb; ++c)
        {
            fread(&b[l][c], sizeof(int), 1, fb);
        }
    }

    printf("Matricea citita din fisierul binar:\n");
    for (int i = 0; i < mb; i++) 
    {
        for (int j = 0; j < nb; j++) 
        {
            printf("%d ", b[i][j]);
        }
        printf("\n");
    }

    freeMatrix(b, mb);
    fclose(fb);
}


int main(void)
{
    int m, n;
    printf("introduceti nr linii & coloane: ");
    scanf("%d%d", &m, &n);

    int **a = citire(m,n);

    writeToBinaryFile("binar.dat", a, m, n);

    readFromBinaryFile("binar.dat");

    freeMatrix(a, m);

    return 0;
}
*/


// apl 5.6
/*
void work(char *filename1, char *filename2)
{
    FILE *f1 = fopen(filename1, "rb");
    if(f1 == NULL)
    {
        fprintf(stderr, "eroare deschidere fisier 1\n");
        exit(-1);
    }

    FILE *f2 = fopen(filename2, "rb");
    if(f2 == NULL)
    {
        fprintf(stderr, "eroare deschidere fisier 2\n");
        exit(-1);
    }

    unsigned char byte1, byte2;
    long offset = 0;
    long filesize1, filesize2;

    fseek(f1, 0, SEEK_END);
    filesize1 = ftell(f1);
    fseek(f1, 0, SEEK_SET);

    fseek(f2, 0, SEEK_END);
    filesize2 = ftell(f2);
    fseek(f2, 0, SEEK_SET);

    while(offset < filesize1 && offset < filesize2)
    {
        fread(&byte1, sizeof(unsigned char), 1, f1);
        fread(&byte2, sizeof(unsigned char), 1, f2);

        if(byte1 != byte2)
        {
            printf("Offset: %ld, File1: 0x%02X, File2: 0x%02X\n", offset, byte1, byte2);
        }
        offset++;
    }

    while (offset < filesize1) 
    {
        fread(&byte1, sizeof(unsigned char), 1, f1);
        printf("Offset: %ld, File1: 0x%02X, File2: --\n", offset, byte1);
        offset++;
    }

    while (offset < filesize2) 
    {
        fread(&byte2, sizeof(unsigned char), 1, f2);
        printf("Offset: %ld, File1: --, File2: 0x%02X\n", offset, byte2);
        offset++;
    }

    fclose(f1);
    fclose(f2);
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        perror("invalid args\n");
        exit(EXIT_FAILURE);
    }

    work(argv[1], argv[2]);

    return 0;
}
*/


// apl 5.5
/*
void hexDump(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
    {
        fprintf(stderr, "eroare deschidere fisier\n");
        exit(-1);
    }

    long long offset = 0;
    long long filesize;
    char bytes[16];

    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    while(offset < filesize)
    {
        fread(bytes, sizeof(char), 16, f);

        printf("%08llx ", offset);

        for(int i = 0; i < 16; i++)
        {
            if(offset + i < filesize)
                printf("%02x ", (unsigned char)bytes[i]);
            else
                printf("   ");
        }

        printf(" ");

        for(int i = 0; i < 16; i++)
        {
            if(offset + i < filesize)
            {
                unsigned char c = bytes[i];
                if(c >= 32 && c <= 255)
                    printf("%c", c);
                else
                    printf(".");
            }
        }

        printf("\n");
        offset += 16;
    }

    fclose(f);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        perror("invalid args\n");
        exit(-1);
    }

    hexDump(argv[1]);

    return 0;
}
*/


/// apl 5.2

int main()
{
    FILE *f = fopen("one.bin", "rb");
    if(f == NULL)
    {
        perror("eroare dechdiere INPUT file\n");
        exit(-1);
    }

    FILE *ff = fopen("two.bin", "wb");
    if(f == NULL)
    {
        perror("eroare dechdiere OUTPUT file\n");
        exit(-1);
    }

    int bytes_read;
    char buffer[4096];

    while ((bytes_read = fread(buffer, 1, 4096, f)) > 0) 
    {
        fwrite(buffer, 1, bytes_read, ff);
    }

    fclose(f);
    fclose(ff);

    return 0;
}