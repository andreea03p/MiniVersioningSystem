#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>

#define PATH_LENGTH 100

int isDirectory(const char *path) 
{
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
   {
       return 0;
   }
   return S_ISDIR(statbuf.st_mode);
}

int isRegularFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    
    return S_ISREG(path_stat.st_mode);
}

void parseDir(const char *dir_name, int nrOfFiles) 
{
    DIR *dir;
    struct dirent *file;
    struct stat file_stat;
    
    dir = opendir(dir_name);
    if (dir == NULL) 
    {
        perror("can not open folder\n");
        exit(EXIT_FAILURE);
    }
    
    while ((file = readdir(dir)) != NULL) 
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) 
        {
            continue;
        }
        
        nrOfFiles++;
        printf("File %3d: %s\n", nrOfFiles, file->d_name);
                     
        char path[PATH_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", dir_name, file->d_name);
        
        if (isDirectory(fullPath)) 
        {
            parseDir(fullPath, 0);
        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) 
{
    if (argc != 2)
    {
        perror("Usage: ./program <input_directory>\n");
        exit(EXIT_FAILURE);
    }

    char *myDir = argv[1];

    if(isDirectory(myDir))
    {
     	parseDir(myDir, 0);
    }

    return 0;
}

