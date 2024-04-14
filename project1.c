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
#include <stdarg.h>

#define PATH_LENGTH 1000
#define file_out "comparisons.txt"
#define SNAPSHOT_DIR "SNAPSHOTS" 

typedef struct 
{
    mode_t st_mode;           // file type and permissions
    off_t st_size;            // total size in bytes
    time_t st_mtime;          // time of last modification
    time_t st_atime;          // time of last access
    char parent_folder[256];  // parent folders
    ino_t st_ino;             // i node
    char filename[256];       // file name

} FileInfo;

FileInfo myCurrentInfo;
FileInfo myPreviousInfo;
struct stat file_stat;
FILE *f;
FILE *info_file;


char* mode_to_symbolic(mode_t mode) 
{
    char* symbolic = (char*)malloc(10 * sizeof(char));
    if (symbolic == NULL) 
    {
        return NULL;
    }

    // Owner
    symbolic[0] = (mode & S_IRUSR) ? 'r' : '-';
    symbolic[1] = (mode & S_IWUSR) ? 'w' : '-';
    symbolic[2] = (mode & S_IXUSR) ? 'x' : '-';
    // Group
    symbolic[3] = (mode & S_IRGRP) ? 'r' : '-';
    symbolic[4] = (mode & S_IWGRP) ? 'w' : '-';
    symbolic[5] = (mode & S_IXGRP) ? 'x' : '-';
    // Others
    symbolic[6] = (mode & S_IROTH) ? 'r' : '-';
    symbolic[7] = (mode & S_IWOTH) ? 'w' : '-';
    symbolic[8] = (mode & S_IXOTH) ? 'x' : '-';
    symbolic[9] = '\0';
    
    return symbolic;
}

// Print info in a txt file
void printFileInfoToFile(const FileInfo *info, FILE *file)
{
    fprintf(file, "Inode: %ld\n", info->st_ino);
    char *permissions = mode_to_symbolic(info->st_mode);
    fprintf(file, "File Permissions: %s\n", permissions);
    free(permissions);
    fprintf(file, "Size: %ld bytes\n", info->st_size);
    fprintf(file, "Last Modification Time: %s", ctime(&info->st_mtime));
    fprintf(file, "Last Access Time: %s", ctime(&info->st_atime));
    fprintf(file, "Parent Folder: %s\n", info->parent_folder);
    fprintf(file, "Filename: %s\n", info->filename);
    fprintf(file, "\n\n");
}


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

// int isSymbolicLink(const char *path) 
// {
//     struct stat statbuf;
//     if (lstat(path, &statbuf) == -1) 
//     {
//         perror("Error getting file information");
//         exit(EXIT_FAILURE);
//     }
    
//     return S_ISLNK(statbuf.st_mode);
// }

// int isHardLink(const char *path) 
// {
//     struct stat statbuf;
//     if (lstat(path, &statbuf) != 0) 
//     {
//         perror("Unable to get file information");
//         exit(EXIT_FAILURE);
//     }
//     return (statbuf.st_nlink > 1) ? 1 : 0;
// }


// Write info in snapshot
void writeSnapshot(const char *snapshot_path)
{
    int snapshot = open(snapshot_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
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

int searchOverwriteSS(ino_t inode, const char *newFilename, const char *prevName) 
{
    DIR *dir;
    struct dirent *ss_file;

    dir = opendir(SNAPSHOT_DIR);
    if (dir == NULL) 
    {
        perror("error opening SS folder\n");
        exit(EXIT_FAILURE);
    }

    while ((ss_file = readdir(dir)) != NULL) 
    {
        if (strcmp(ss_file->d_name, ".") == 0 || strcmp(ss_file->d_name, "..") == 0)
        {
            continue;
        }
        char snapshotPath[PATH_LENGTH];
        snprintf(snapshotPath, sizeof(snapshotPath), "%s/%s", SNAPSHOT_DIR, ss_file->d_name);
        
        if (readSnapshot(snapshotPath)) 
        {
            if (inode == myPreviousInfo.st_ino) 
            {
                fprintf(f, "Snapshot with the same inode(%ld) already exists for file: %s (prev: %s)\n\n", inode, newFilename, prevName);

                char ss_name[100] = "/home/user/SO/PROIECT/SNAPSHOTS/";
                strcat(ss_name, prevName);
                strcat(ss_name, ".ss");
                if (remove(ss_name) == 0) 
                {
                    printf("File '%s' deleted successfully.\n", ss_name);
                } 
                else 
                {
                    perror("Error deleting file");
                }

                return 1;
            }
        }
    }
    return 0;
    closedir(dir);
}


// Compare previous vs current version
int comparePrevVsCurr(const char *path)
{
    int counter = 0;

    fprintf(f, "%ld - %ld\n",myPreviousInfo.st_ino, myCurrentInfo.st_ino);
    if(myCurrentInfo.st_atime != myPreviousInfo.st_atime)
    {
        fprintf(f, "Folder: %s, File: %s was accessed in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if(myCurrentInfo.st_mtime != myPreviousInfo.st_mtime)
    {
        fprintf(f, "Folder: %s, File: %s was modified in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if (myCurrentInfo.st_size < myPreviousInfo.st_size)
    {
        fprintf(f, "Folder: %s, File: %s - Some data was removed\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }
    else if (myCurrentInfo.st_size > myPreviousInfo.st_size)
    {
        fprintf(f, "Folder: %s, File: %s - Some data was added\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }
    else
    {
        fprintf(f, "Folder: %s, File: %s - Same amount of data\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if(strcmp(myCurrentInfo.parent_folder, myPreviousInfo.parent_folder) != 0)
    {
        fprintf(f, "File: %s was moved from %s TO %s\n", myCurrentInfo.filename, myPreviousInfo.parent_folder, myCurrentInfo.parent_folder);
        counter++;
    }

    if(myCurrentInfo.st_mode != myPreviousInfo.st_mode)
    {
        fprintf(f, "File: %s; permissions were changed from %s TO %s\n", myCurrentInfo.filename, mode_to_symbolic(myPreviousInfo.st_mode), mode_to_symbolic(myCurrentInfo.st_mode));
    }

    fprintf(f, "\n");

    return counter;
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
    	// Obtain path         
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
            // create .ss buffer
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
            strcpy(myCurrentInfo.filename, file->d_name);
            
            printFileInfoToFile(&myCurrentInfo, info_file);
            
            int snapshotStatus = readSnapshot(snapshot_path);
            if (snapshotStatus == 0) 
            {
                if(searchOverwriteSS(file_stat.st_ino, myCurrentInfo.filename, myPreviousInfo.filename) != 0)
                {
                    writeSnapshot(snapshot_path);
                }
                else
                {
                    fprintf(f, "file %s added", myCurrentInfo.filename);
                    writeSnapshot(snapshot_path);
                }
            } 
            else 
            {
                if(comparePrevVsCurr(path) > 0)
                {
                    writeSnapshot(snapshot_path);
                }
            }
        }

        if (isDirectory(path)) 
        {
            parseDir(path, snapshots_dir);
        }

        // if (isSymbolicLink(path)) 
        // {
        //     printf("%s is a symbolic link\n", path);
        // }

        // if (isHardLink(path)) 
        // {
        //     printf("%s is a hard link\n", path);
        // }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) 
{
    if (argc < 4 || argc > 12)
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
    
    info_file = fopen("info.txt", "w");
    if (info_file == NULL)
    {
        perror("Error opening output info file\n");
        return 1;
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

    fclose(info_file);
    fclose(f);

    return 0;
}
