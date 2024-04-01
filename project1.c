#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <limits.h> 
#include <time.h>


#define PATH_LENGTH 1000


typedef struct FileInfo
{
    mode_t mode; // type of file
    off_t st_size; // size in bytes
    __time_t st_mtime; // last modif
    __time_t st_atime; // last access
    char parent_folder[PATH_LENGTH]; // parent folder
    mode_t permissions; // permissions
    char file_name[20]; // file name
    
} FileInfo;

FileInfo myCurrentInfo;
FileInfo myPreviousInfo;
struct stat file_stat;


// check if is a directory
int isDirectory(const char *path) 
{
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
   {
       return 0;
   }
   return S_ISDIR(statbuf.st_mode);
}


// check if it s a regular file (not symlinks or special)
int isRegularFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    
    return S_ISREG(path_stat.st_mode);
}


// write info in snapshot
void writeSnapshot(const char *snapshot_path)
{
    int snapshot = open(snapshot_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (snapshot < 0) 
    {
        perror("error at opening snapshot file to write in\n");
        exit(EXIT_FAILURE);
    }
    
    unsigned char buffer[sizeof(FileInfo)];
    memcpy(buffer, &myCurrentInfo, sizeof(FileInfo));
    
    if(write(snapshot, buffer, sizeof(FileInfo)) == -1)
    {
        perror("error writing in snapshot file\n");
        exit(EXIT_FAILURE);
    }
    
    close(snapshot);
}

// read info from snapshot
void readSnapshot(const char *snapshot_path)
{
    int snapshot = open(snapshot_path, O_RDONLY);
    if (snapshot < 0)
    {
        perror("error at opening snapshot file to read from\n");
        return;
    }

    if (read(snapshot, &myPreviousInfo, sizeof(FileInfo)) == -1)
    {
        perror("error reading from snapshot file\n");
        return;
    }

    close(snapshot);
}


// compare previous vs current version
void comparePrevVsCurr()
{
    if(myCurrentInfo.st_size < myPreviousInfo.st_size)
    {
        printf("folder: %s file: %s - some data was removed\n", myCurrentInfo.parent_folder, myCurrentInfo.file_name);
    }
    else if(myCurrentInfo.st_size > myPreviousInfo.st_size)
    {
        printf("folder: %s file: %s - some data was added\n", myCurrentInfo.parent_folder, myCurrentInfo.file_name);
    }
    else
    {
        printf("folder: %s file: %s - same amount of data\n", myCurrentInfo.parent_folder, myCurrentInfo.file_name);
    }

    if(strcmp(myCurrentInfo.file_name, myPreviousInfo.file_name) != 0)
    {
        printf("folder: %s file: %s was renamed to %s\n", myCurrentInfo.parent_folder, myPreviousInfo.file_name, myCurrentInfo.file_name);
    }
}


// process the directory and call the subdirs
void parseDir(const char *dir_name) 
{
    DIR *dir;
    struct dirent *file;
    
    dir = opendir(dir_name);
    if (dir == NULL) 
    {
        perror("can not open folder\n");
        exit(EXIT_FAILURE);
    }
    
    while ((file = readdir(dir)) != NULL) 
    {
    	// obtain path by name         
        char path[PATH_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", dir_name, file->d_name);
        
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 ||
    strstr(file->d_name, ".ss") != NULL)
        {
            continue;
        }
        
        if (isRegularFile(path)) 
        {
            char snapshot_path[PATH_LENGTH];
            // Generate unique filename with ".ss" extension
            snprintf(snapshot_path, PATH_LENGTH, "%s/%s.ss", dir_name, file->d_name);
            
            if (stat(path, &file_stat) == -1) 
            {
                perror("stat-error getting info\n");
                exit(EXIT_FAILURE);
            }
        
            // get parent folder
            char parent[200];
            snprintf(parent, sizeof(parent), "%s/..", dir_name);
                
            myCurrentInfo.mode = file_stat.st_mode;
            myCurrentInfo.st_size = file_stat.st_size;
            myCurrentInfo.st_mtime = file_stat.st_mtime;
            myCurrentInfo.st_atime = file_stat.st_atime;
            strcpy(myCurrentInfo.parent_folder, parent);
            myCurrentInfo.permissions = S_IRUSR | S_IWUSR | S_IXUSR;
            strcpy(myCurrentInfo.file_name, file->d_name);
            
            readSnapshot(snapshot_path);
            comparePrevVsCurr();
            writeSnapshot(snapshot_path);
        }
        
        if (isDirectory(path)) 
        {
            parseDir(path);
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
     	parseDir(myDir);
    }

    return 0;
}

