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
#include<sys/wait.h>


#define PATH_LENGTH 1000
#define comp_out "comparisons.txt"
#define info_out "info.txt"
#define SNAPSHOT_DIR "SNAPSHOTS" 

typedef struct 
{
    mode_t st_mode;           // file type and permissions
    off_t st_size;            // total size in bytes
    time_t mtime;             // time of last modification
    time_t atime;             // time of last access
    char parent_folder[256];  // parent folders
    ino_t st_ino;             // i node
    char filename[256];       // file name

} FileInfo;

FileInfo myCurrentInfo;
FileInfo myPreviousInfo;
struct stat file_stat;
FILE *comp_file;
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


int checkPermissions(const char *permissions)
{
    if(strcmp(permissions, "---------"))
    {
        return 1;
    }
    return 0;
}


// Print info in a txt file
void printFileInfoToFile(const FileInfo *info, FILE *file)
{
    fprintf(file, "Inode: %ld\n", info->st_ino);
    fprintf(file, "Filename: %s\n", info->filename);
    fprintf(file, "Parent Folder: %s\n", info->parent_folder);
    char *permissions = mode_to_symbolic(info->st_mode);
    fprintf(file, "File Permissions: %s\n", permissions);
    free(permissions);
    fprintf(file, "Size: %ld bytes\n", info->st_size);
    fprintf(file, "Last Modification Time: %s", ctime(&info->mtime));
    fprintf(file, "Last Access Time: %s", ctime(&info->atime));
    fprintf(file, "\n\n");
}


int isDirectory(const char *path) 
{
    struct stat statbuf;
    return stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode);
}

int isRegularFile(const char *path) 
{
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}


int isSymbolicLink(const char *path) 
{
    struct stat statbuf;
    if (lstat(path, &statbuf) == -1) 
    {
        perror("Error getting file information");
        exit(EXIT_FAILURE);
    }
    
    return S_ISLNK(statbuf.st_mode);
}

int isHardLink(const char *path) 
{
    struct stat statbuf;
    if (lstat(path, &statbuf) != 0) 
    {
        perror("Unable to get file information");
        exit(EXIT_FAILURE);
    }
    return (statbuf.st_nlink > 1) ? 1 : 0;
}


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
        
        if (readSnapshot(snapshotPath) && !isSymbolicLink(snapshotPath)) 
        {
            if (inode == myPreviousInfo.st_ino && !isSymbolicLink(snapshotPath)) 
            {
                fprintf(comp_file, "Snapshot with the same inode(%ld) already exists for file: %s (previous file name: %s)\n\n", inode, newFilename, prevName);

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


// Compare previous vs current version of snapshot
int comparePrevVsCurr(const char *path)
{
    int counter = 0;

    fprintf(comp_file, "%ld - %ld\n",myPreviousInfo.st_ino, myCurrentInfo.st_ino);
    if(myCurrentInfo.atime != myPreviousInfo.atime && myCurrentInfo.st_ino == myPreviousInfo.st_ino)
    {
        fprintf(comp_file, "Folder: %s, File: %s was accessed in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if(myCurrentInfo.mtime != myPreviousInfo.mtime)
    {
        fprintf(comp_file, "Folder: %s, File: %s was modified in the meantime\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if (myCurrentInfo.st_size < myPreviousInfo.st_size)
    {
        fprintf(comp_file, "Folder: %s, File: %s - Some data was removed\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }
    else if (myCurrentInfo.st_size > myPreviousInfo.st_size)
    {
        fprintf(comp_file, "Folder: %s, File: %s - Some data was added\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }
    else
    {
        fprintf(comp_file, "Folder: %s, File: %s - Same amount of data\n", myCurrentInfo.parent_folder, myCurrentInfo.filename);
        counter++;
    }

    if(strcmp(myCurrentInfo.parent_folder, myPreviousInfo.parent_folder) != 0 && myCurrentInfo.st_ino == myPreviousInfo.st_ino)
    {
        fprintf(comp_file, "File: %s was moved from %s TO %s\n", myCurrentInfo.filename, myPreviousInfo.parent_folder, myCurrentInfo.parent_folder);
        counter++;
    }

    if(myCurrentInfo.st_mode != myPreviousInfo.st_mode)
    {
        fprintf(comp_file, "File: %s; permissions were changed from %s TO %s\n", myCurrentInfo.filename, mode_to_symbolic(myPreviousInfo.st_mode), mode_to_symbolic(myCurrentInfo.st_mode));
    }

    fprintf(comp_file, "\n");

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
        
        if (isRegularFile(path) && !isSymbolicLink(path)) 
        {
            // Create formatted path for .ss
            char snapshot_path[PATH_LENGTH];
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
            myCurrentInfo.mtime = file_stat.st_mtime;
            myCurrentInfo.atime = file_stat.st_atime;
            strcpy(myCurrentInfo.parent_folder, parent);
            strcpy(myCurrentInfo.filename, file->d_name);
            
            printFileInfoToFile(&myCurrentInfo, info_file);

            printf("\n%d\n", checkPermissions(mode_to_symbolic(myCurrentInfo.st_mode)));
            
            int snapshotStatus = readSnapshot(snapshot_path);
            if (snapshotStatus == 0) 
            {
                if(searchOverwriteSS(file_stat.st_ino, myCurrentInfo.filename, myPreviousInfo.filename) == 0)
                {
                    fprintf(comp_file, "file %s added\n\n", myCurrentInfo.filename);
                }
                writeSnapshot(snapshot_path);
            } 
            else 
            {
                if(comparePrevVsCurr(path) > 0)
                {
                    writeSnapshot(snapshot_path);
                }
            }
        }

        if (isSymbolicLink(path)) 
        {
            char target[PATH_LENGTH];

            ssize_t len = readlink(path, target, sizeof(target) - 1);
            if (len != -1) 
            {
                target[len] = '\0';
                fprintf(info_file, "The symbolic link %s points to: %s\n\n\n", path, target);
            } 
            else 
            {
                perror("readlink");
                exit(EXIT_FAILURE);
            }
        }

        // if (isHardLink(path)) 
        // {
        //     printf("%s is a hard link\n", path);
        // }

        if (isDirectory(path)) 
        {
            parseDir(path, snapshots_dir);
        }
    }

    closedir(dir);
}


void checkDeleted(const char *dir_name, const char *filename, int *ok) 
{
    DIR *dir = opendir(dir_name);
    if (dir == NULL) 
    {
        perror("Error opening directory");
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char path[PATH_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        if (strcmp(entry->d_name, filename) == 0 && isRegularFile(path)) 
        {
            printf("File '%s' exists in directory '%s'\n", filename, dir_name);
            *ok = 1;
            return;
        }

        if (isDirectory(path)) 
        {
            checkDeleted(path, filename, ok);
        }
    }
    closedir(dir);
}


void process_directory(const char *input_dir, const char *snapshots_dir) 
{
    // Create a child process
    pid_t pid = fork();
    if (pid == 0) 
    {
        if (isDirectory(input_dir) && isDirectory(snapshots_dir)) 
        {
            //execlp("./p", "parseDir", input_directory, snapshots_dir, NULL);
            //perror("error exec");
            //exit(EXIT_FAILURE);
            parseDir(input_dir, snapshots_dir);
            printf("Snapshot for Directory %s created successfully.\n", input_dir);
            exit(EXIT_SUCCESS);
        } 
        else 
        {
            perror("invalid in/out directories\n");
            exit(EXIT_FAILURE);
        }
    } 
    else if (pid < 0) 
    {
        perror("fork");
    }
}

int main(int argc, char *argv[]) 
{
    if (argc < 6 || argc > 15) 
    {
        perror("Usage: ./program -o <output_directory> <input_directories>\n");
        exit(EXIT_FAILURE);
    }

    comp_file = fopen(comp_out, "w");
    if(comp_file == NULL) 
    {
        perror("Error opening file to write comparisons\n");
        exit(EXIT_FAILURE);
    }

    info_file = fopen(info_out, "w");
    if (info_file == NULL) 
    {
        perror("Error opening output info file\n");
        return 1;
    }

    const char *snapshots_dir = argv[2];
    const char *isolated_dir = argv[4];

    int child_count = 0;
    for (int i = 5; i < argc; i++) 
    {
        const char *input_dir= argv[i];
        process_directory(input_dir, snapshots_dir);
        child_count++;
    }

    int status;
    pid_t child_pid;
    for (int i = 0; i < child_count; i++) 
    {
        child_pid = wait(&status);
        if (WIFEXITED(status)) 
        {
            printf("Child Process %d terminated with PID %d and exit code %d.\n", i + 1, child_pid, WEXITSTATUS(status));
        } 
        else 
        {
            printf("Child Process %d terminated with PID %d and errors.\n", i + 1, child_pid);
        }
    }


    //Check if there are deleted files by snapshots name
    printf("\n\n");
    int ok = 0;
    DIR *ss_dir = opendir(snapshots_dir);
    if (ss_dir == NULL) 
    {
        perror("Error opening snapshots folder");
        exit(EXIT_FAILURE);
    }

    struct dirent *snapshot_file;
    while ((snapshot_file = readdir(ss_dir)) != NULL) 
    {
        if (strcmp(snapshot_file->d_name, ".") == 0 || strcmp(snapshot_file->d_name, "..") == 0) 
        {
            continue;
        }

        char f_name[100] = "";
        strncpy(f_name, snapshot_file->d_name, strlen(snapshot_file->d_name) - 3);
        f_name[strlen(snapshot_file->d_name) - 3] = '\0';
        printf("%s\n", f_name);

        ok = 0;
        for (int i = 3; i < argc; i++) 
        {
            const char *input_directory = argv[i];
            if (isDirectory(input_directory)) 
            {
                checkDeleted(input_directory, f_name, &ok);
            }
        }
        
        if(ok == 0)
        {
           printf("File '%s' does not exist\n", f_name);     
        }
    }

    closedir(ss_dir);
    fclose(comp_file);
    fclose(info_file);

    return 0;
}