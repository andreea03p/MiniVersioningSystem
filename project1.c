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
#include <sys/types.h>


#define PATH_LENGTH 1000
#define file_out "comparisons.txt"

typedef struct 
{
    mode_t st_mode;           // file type and permissions
    off_t st_size;            // total size in bytes
    time_t st_mtime;          // time of last modification
    time_t st_atime;          // time of last access
    char parent_folder[256];  // parent folders
    ino_t st_ino;             // i node
    char filename[256];

} FileInfo;

FileInfo myCurrentInfo;
FileInfo myPreviousInfo;
struct stat file_stat;
FILE *f;


// Check if is a directory
int isDirectory(const char *path) 
{
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
   {
       return 0;
   }
   return S_ISDIR(statbuf.st_mode);
}

// Check if it's a regular file
int isRegularFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    
    return S_ISREG(path_stat.st_mode);
}


// Write info in snapshot
void writeSnapshot(const char *snapshot_path)
{
    int snapshot = open(snapshot_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (snapshot == -1) 
    {
        perror("Error opening snapshot file to write in\n");
        return;
    }
    
    unsigned char buffer[sizeof(FileInfo)];
    memcpy(buffer, &myCurrentInfo, sizeof(FileInfo));
    
    if (write(snapshot, buffer, sizeof(FileInfo)) == -1)
    {
        perror("Error writing in snapshot file\n");
        return;
    }
    
    close(snapshot);
}

// Read info from snapshot
int readSnapshot(const char *snapshot_path)
{
    int snapshot = open(snapshot_path, O_RDONLY);
    if (snapshot == -1)
    {
        perror("Error opening snapshot file to read from\n");
        return 0;
    }

    if (read(snapshot, &myPreviousInfo, sizeof(FileInfo)) == -1)
    {
        perror("Error reading from snapshot file\n");
        return 0;
    }

    close(snapshot);
    return 1;
}


// Compare previous vs current version
void comparePrevVsCurr(const char *path)
{
    if(myCurrentInfo.st_atime != myPreviousInfo.st_atime)
    {
        fprintf(f, "Folder: %s, File: %s was accessed in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
    }

    if(myCurrentInfo.st_mtime != myPreviousInfo.st_mtime)
    {
        fprintf(f, "Folder: %s, File: %s was modified in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
    }

    if (myCurrentInfo.st_size < myPreviousInfo.st_size)
    {
        fprintf(f, "Folder: %s, File: %s - Some data was removed\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
    }
    else if (myCurrentInfo.st_size > myPreviousInfo.st_size)
    {
        fprintf(f, "Folder: %s, File: %s - Some data was added\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
    }
    else
    {
        fprintf(f, "Folder: %s, File: %s - Same amount of data\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
    }

    /*if (myCurrentInfo.st_ino != myPreviousInfo.st_ino ||
        strcmp(myCurrentInfo.filename, myPreviousInfo.filename) != 0)
    {
        fprintf(f, "Folder: %s, File: %s was renamed to %s\n", myCurrentInfo.parent_folder, myPreviousInfo.filename, myCurrentInfo.filename);
    }*/

    fprintf(f, "\n");
}


// Process the directory and call the subdirectories
void parseDir(const char *dir_name, const char *snapshots_dir) 
{
    DIR *dir;
    struct dirent *file;
    
    dir = opendir(dir_name);
    if (dir == NULL) 
    {
        perror("error opening folder\n");
        exit(EXIT_FAILURE);
    }
    
    while ((file = readdir(dir)) != NULL) 
    {
    	// Obtain path by name         
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
            // create .ss file and put it in snapshot dir
            snprintf(snapshot_path, PATH_LENGTH, "%s/%s.ss", snapshots_dir, file->d_name);
            
            if (stat(path, &file_stat) == -1) 
            {
                perror("error getting info\n");
                exit(EXIT_FAILURE);
            }
        
            // Get parent folder
            char parent[PATH_LENGTH];
            snprintf(parent, sizeof(parent), "%s/..", dir_name);
                
            myCurrentInfo.st_ino = file_stat.st_ino;
            myCurrentInfo.st_mode = file_stat.st_mode;
            myCurrentInfo.st_size = file_stat.st_size;
            myCurrentInfo.st_mtime = file_stat.st_mtime;
            myCurrentInfo.st_atime = file_stat.st_atime;
            strcpy(myCurrentInfo.parent_folder, parent);
            myCurrentInfo.st_mode = S_IRUSR | S_IWUSR | S_IXUSR;
            strcpy(myCurrentInfo.filename, file->d_name);
            
            if (readSnapshot(snapshot_path)) 
            {
                comparePrevVsCurr(path);
                writeSnapshot(snapshot_path);
            } 
            else 
            {
                writeSnapshot(snapshot_path);
            }
        }
        
        if (isDirectory(path)) 
        {
            parseDir(path, snapshots_dir);
        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) 
{
    if (argc < 4 || argc > 13)
    {
        perror("Usage: ./program -o <output_directory> <input_directories>\n");
        exit(EXIT_FAILURE);
    }

    f = fopen(file_out, "w");
    if(f == NULL)
    {
        perror("error opening file to write comparisons\n");
        exit(EXIT_FAILURE);
    }

    const char *snapshots_dir = argv[2];

    for(int i = 3; i < argc; i++)
    {
        const char *input_directory = argv[i];
        if(isDirectory(input_directory) && isDirectory(snapshots_dir))
        {
            parseDir(input_directory, snapshots_dir);
        }
    }

    fclose(f);

    return 0;
}
