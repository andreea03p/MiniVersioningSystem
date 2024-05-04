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
#define PATH_TO_SCRIPT "/home/user/SO/PROIECT/verify_malicious.sh"
#define PATH_TO_SSDIR "/home/user/SO/PROIECT/SNAPSHOTS/"
#define PATH_TO_ISOLATEDIR "/home/user/SO/PROIECT/ISOLATED/"

typedef struct 
{
    mode_t st_mode;           // file type and permissions
    off_t st_size;            // total size in bytes
    time_t mtime;             // time of last modification
    time_t atime;             // time of last access
    char parent_folder[256];  // parent folders
    ino_t st_ino;             // i node
    char filename[256];       // file name
    char linkTo[256];         // string to print the link

} FileInfo;

FileInfo myCurrentInfo;
FileInfo myPreviousInfo;
struct stat file_stat;
FILE *comp_file;
FILE *info_file;


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
        perror("error getting file information symlink");
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


// Permissions string
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
void writeFileInfoToFile(const FileInfo *info, FILE *file)
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
    fprintf(file, "%s", info->linkTo);
    fprintf(file, "\n\n");
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



// if file is renamed it prints and prev snapshot is deleted
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

                char ss_name[100] = PATH_TO_SSDIR;
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


// PIPES processes and script
void checkPermissions(const char *permissions, const char *file_path) 
{
    if (strcmp(permissions, "---------") == 0) 
    {
        int pipefd[2];
        if (pipe(pipefd) == -1) 
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        
        if (pid == -1) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) // child 
        {
            close(pipefd[0]);
            
            // Redirect stdout to the write end of the pipe
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) 
            {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(pipefd[1]);
            
            execl(PATH_TO_SCRIPT, "verify_malicious.sh", file_path, PATH_TO_ISOLATEDIR, NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } 
        else // parent
        {
            close(pipefd[1]);

            // Read from the pipe and print the script's output
            char buffer[1024];
            ssize_t bytes_read;
            int found_dangerous_files = 0;
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) 
            {
                // Print the output received from the script
                write(STDOUT_FILENO, buffer, bytes_read);

                // Check if potentially dangerous file was found
                if (strstr(buffer, "DANGEROUS") != NULL) 
                {
                    found_dangerous_files++;
                }
            }
            if (bytes_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            // Wait for the child process to finish
            wait(NULL);

            close(pipefd[0]);

            printf("Child Process %d terminated with PID %d and %d potentially dangerous file(s).\n", getpid(), pid, found_dangerous_files);
        }
    }
}


// Process the directory and call the subdirectories
void parseDir(const char *dir_name, const char *snapshots_dir, const char *isolate_dir) 
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
                perror("error getting info stat\n");
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
            strcpy(myCurrentInfo.linkTo, "NO LINK");
            
            writeFileInfoToFile(&myCurrentInfo, info_file);

            checkPermissions(mode_to_symbolic(myCurrentInfo.st_mode), path);
            
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
            char linkInfo[1000] = "";

            ssize_t len = readlink(path, target, sizeof(target) - 1);
            if (len != -1) 
            {
                target[len] = '\0';
                strcat(linkInfo, "The symbolic link ");
                strcat(linkInfo, path);
                strcat(linkInfo, " points to ");
                strcat(linkInfo, target);
                strcat(linkInfo, "\n");
            } 
            else 
            {
                perror("readlink");
                exit(EXIT_FAILURE);
            }

            // Create formatted path for .sym
            char snapshot_path[PATH_LENGTH];
            snprintf(snapshot_path, PATH_LENGTH, "%s/%s.sym", snapshots_dir, file->d_name);
            
            if (lstat(path, &file_stat) == -1) 
            {
                perror("error getting info lstat\n");
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
            strcpy(myCurrentInfo.linkTo, linkInfo);
            
            writeFileInfoToFile(&myCurrentInfo, info_file);
            
            writeSnapshot(snapshot_path);
        }

        // if (isHardLink(path)) 
        // {
        //     printf("%s is a hard link\n", path);
        // }

        if (isDirectory(path)) 
        {
            parseDir(path, snapshots_dir, isolate_dir);
        }
    }

    closedir(dir);
}


// // Process each directory in a new process
// void process_directory(const char *input_dir, const char *snapshots_dir, const char *isolate_dir) 
// {
//     // Create a child process
//     pid_t pid = fork();
//     if(pid > 0)
//     {
//         printf("this is the PARENT process with id: %d\n", getppid());
//     }
//     else if (pid == 0) 
//     {
//         printf("this is the PARENT process with id: %d\n", getppid());
//         printf("this is the CHILD process with id: %d\n", getpid());
//         if (isDirectory(input_dir) && isDirectory(snapshots_dir)) 
//         {
//             parseDir(input_dir, snapshots_dir, isolate_dir);
//             printf("Snapshot for Directory %s created successfully.\n", input_dir);
//             exit(EXIT_SUCCESS);
//         } 
//         else 
//         {
//             perror("invalid in/out directories\n");
//             exit(EXIT_FAILURE);
//         }
//     } 
//     else
//     {
//         perror("fork");
//     }
// }


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
            printf("\nFile '%s' exists in directory '%s'\n", filename, dir_name);
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


void searchForDeletedFilesInSnapshotsDir(const char *snapshots_dir, int nrArg, char *args[])
{
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
        if (strcmp(snapshot_file->d_name, ".") == 0 || strcmp(snapshot_file->d_name, "..") == 0 || strstr(snapshot_file->d_name, ".sym")) 
        {
            continue;
        }

        char f_name[100] = "";
        strncpy(f_name, snapshot_file->d_name, strlen(snapshot_file->d_name) - 3);
        f_name[strlen(snapshot_file->d_name) - 3] = '\0';

        // strncpy(f_name, snapshot_file->d_name, strrchr(snapshot_file->d_name, '_') - snapshot_file->d_name);
        // f_name[strrchr(snapshot_file->d_name, '_') - snapshot_file->d_name] = '\0';

        ok = 0;
        for (int i = 5; i < nrArg; i++) 
        {
            const char *input_directory = args[i];
            if (isDirectory(input_directory)) 
            {
                checkDeleted(input_directory, f_name, &ok);
            }
        }
        
        if(ok == 0)
        {
           printf("\nFile '%s' does not exist anymore\n", f_name);     
        }
    }

    closedir(ss_dir);
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
    const char *isolate_dir = argv[4];

    for (int i = 5; i < argc; i++) 
    {
        pid_t pid = fork();
        if (pid == -1) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) 
        { 
            printf("parent process id: %d\n", getppid());
            printf("child process id: %d\n", getpid());
            parseDir(argv[i], snapshots_dir, isolate_dir);
            printf("Snapshot for %s created successfully.\n", argv[i]);
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to terminate
    int status;
    pid_t pid;
    while ((pid = wait(&status)) != -1) 
    {
        printf("Process with PID %d ended with code %d\n", pid, WEXITSTATUS(status));
    }

    //Check if there are deleted files by snapshot name
    searchForDeletedFilesInSnapshotsDir(snapshots_dir, argc, argv);
    
    fclose(comp_file);
    fclose(info_file);

    return 0;
}