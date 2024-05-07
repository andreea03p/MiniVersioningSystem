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
#define ISOLATED_DIR "ISOLATED" 
#define PATH_TO_VERIFY_SCRIPT "/home/user/SO/PROIECT/verify_malicious.sh"
#define PATH_TO_MOVE_SCRIPT "/home/user/SO/PROIECT/move_file.sh"
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
void writeSnapshot(const char *snapshot_path, const FileInfo *info) 
{
    int snapshot = open(snapshot_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (snapshot == -1) 
    {
        perror("Error opening snapshot file to write in\n");
        return;
    }

    char buffer[2048];

    int len = snprintf(buffer, sizeof(buffer),
                       "Mode: %o\nSize: %ld\nModification Time: %ld\nAccess Time: %ld\nParent Folder: %s\nInode: %lu\nFilename: %s\nLink: %s\n",
                       info->st_mode, info->st_size, (long)info->mtime, (long)info->atime,
                       info->parent_folder, (unsigned long)info->st_ino, info->filename, info->linkTo);

    if (write(snapshot, buffer, len) == -1) 
    {
        perror("Error writing in snapshot file\n");
        close(snapshot);
        return;
    }

    close(snapshot);
}

// Read info from snapshot
int readSnapshot(const char *snapshot_path, FileInfo *info) 
{
    int snapshot = open(snapshot_path, O_RDONLY);
    if (snapshot == -1) 
    {
        perror("Error opening snapshot file to read from\n");
        return 0;
    }

    char buffer[2048];
    ssize_t bytes_read = read(snapshot, buffer, sizeof(buffer));
    if (bytes_read == -1) 
    {
        perror("Error reading from snapshot file\n");
        close(snapshot);
        return 0;
    }

    sscanf(buffer,
           "Mode: %o\nSize: %ld\nModification Time: %ld\nAccess Time: %ld\nParent Folder: %s\nInode: %lu\nFilename: %s\nLink: %s\n",
           &info->st_mode, &info->st_size, (long *)&info->mtime, (long *)&info->atime,
           info->parent_folder, (unsigned long *)&info->st_ino, info->filename, info->linkTo);

    close(snapshot);
    return 1;
}


void pathToSaveFilename(const char *path, char *safe_filename)
{
    for (int i = 0; i < strlen(path); ++i)
    {
        if (path[i] == '/')
        {
            safe_filename[i] = '_';
        }
        else
        {
            safe_filename[i] = path[i];
        }
    }
    safe_filename[strlen(path)] = '\0';
}


// Compare previous vs current version of snapshot
int comparePrevVsCurr(const FileInfo *currentInfo, const FileInfo *previousInfo, FILE *comp_file) 
{
    int counter = 0;

    fprintf(comp_file, "%ld - %ld\n", previousInfo->st_ino, currentInfo->st_ino);
    if (currentInfo->atime != previousInfo->atime && currentInfo->st_ino == previousInfo->st_ino) 
    {
        fprintf(comp_file, "Folder: %s, File: %s was accessed in the meantime\n", currentInfo->parent_folder, currentInfo->filename);
        counter++;
    }

    if (currentInfo->mtime != previousInfo->mtime)
    {
        fprintf(comp_file, "Folder: %s, File: %s was modified in the meantime\n", currentInfo->parent_folder, currentInfo->filename);
        counter++;
    }

    if (currentInfo->st_size < previousInfo->st_size) 
    {
        fprintf(comp_file, "Folder: %s, File: %s - Some data was removed\n", currentInfo->parent_folder, currentInfo->filename);
        counter++;
    }
    else if (currentInfo->st_size > previousInfo->st_size) 
    {
        fprintf(comp_file, "Folder: %s, File: %s - Some data was added\n", currentInfo->parent_folder, currentInfo->filename);
        counter++;
    } 
    else 
    {
        fprintf(comp_file, "Folder: %s, File: %s - Same amount of data\n", currentInfo->parent_folder, currentInfo->filename);
        counter++;
    }

    if (currentInfo->st_mode != previousInfo->st_mode) 
    {
        fprintf(comp_file, "File: %s; permissions were changed from %s TO %s\n", currentInfo->filename, mode_to_symbolic(previousInfo->st_mode), mode_to_symbolic(currentInfo->st_mode));
    }

    fprintf(comp_file, "\n");
    return counter;
}


// if file is renamed/moved it prints and prev snapshot is deleted
int searchOverwriteSS(ino_t inode, const char *newFilename, const char *prevName, FILE *comp_file, FileInfo *myPreviousInfo, FileInfo *myCurrentInfo) 
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
        
        if (readSnapshot(snapshotPath, myPreviousInfo)) 
        {
            if (inode == myPreviousInfo->st_ino && !isSymbolicLink(snapshotPath)) 
            {
                fprintf(comp_file, "Snapshot with the same inode(%ld) already exists for file: %s (previous file name: %s)\n\n", inode, newFilename, prevName);
                if(strcmp(newFilename, prevName) == 0)
                {
                    fprintf(comp_file, "---File %s was moved from %s to %s\n\n", newFilename, myPreviousInfo->parent_folder, myCurrentInfo->parent_folder);
                    char ss_name[PATH_LENGTH] = PATH_TO_SSDIR;
                    char aux[PATH_LENGTH] = "";
                    pathToSaveFilename(myPreviousInfo->parent_folder, aux);
                    strcat(ss_name, aux);
                    ss_name[strlen(ss_name) - 2] = '\0';
                    strcat(ss_name, newFilename);
                    strcat(ss_name, ".ss");

                    if (remove(ss_name) == 0) 
                    {
                        printf("File '%s' deleted successfully.\n", ss_name);
                    } 
                    else 
                    {
                        perror("Error deleting file");
                    }
                }
                else
                {
                    char ss_name[PATH_LENGTH] = PATH_TO_SSDIR;
                    char aux[PATH_LENGTH] = "";
                    pathToSaveFilename(myPreviousInfo->parent_folder, aux);
                    strcat(ss_name, aux);
                    ss_name[strlen(ss_name) - 2] = '\0';
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
                }
                return 1;
            }
        }
    }
    return 0;
    closedir(dir);
}


// PIPES processes and script
void checkPermissions(const char *permissions, const char *file_path, int *dangerous_files_count) 
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

        if (pid == 0) 
        {
            // Child process - run verification script
            close(pipefd[0]); // Close unused read end
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to write end of the pipe
            close(pipefd[1]);

            execl(PATH_TO_VERIFY_SCRIPT, "verify_malicious.sh", file_path, NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } 
        else 
        {
            // Parent process - read output from the script
            close(pipefd[1]); // Close unused write end

            char buffer[1024];
            ssize_t bytes_read;
            int total_dangerous_files = 0;

            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) 
            {
                buffer[bytes_read] = '\0';
                if (strcmp(buffer, "SAFE\n") == 0) 
                {
                    printf("SAFE\n");
                } 
                else 
                {
                    printf("%s\n", buffer);

                    pid_t move_pid = fork();
                    if (move_pid == -1) 
                    {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                    if (move_pid == 0) 
                    {
                        // Move the dangerous file to the isolated directory
                        execl(PATH_TO_MOVE_SCRIPT, "move_file.sh", file_path, PATH_TO_ISOLATEDIR, NULL);
                        perror("execl");
                        exit(EXIT_FAILURE);
                    } 
                    else 
                    {
                        wait(NULL); // Wait for the child process to finish moving the file
                        total_dangerous_files++;
                    }
                }
            }

            if (bytes_read == -1) 
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            close(pipefd[0]);
            wait(NULL); // Wait for the child process to finish
            
            (*dangerous_files_count) += total_dangerous_files;
        }
    }
}



// Process the directory and call the subdirectories
void processDirectory(const char *dir_name, const char *snapshots_dir, const char *isolate_dir, int *dangerous_counter, FILE *info_file, FILE *comp_file)
{
    DIR *dir;
    struct dirent *file;

    FileInfo myCurrentInfo;
    FileInfo myPreviousInfo;
    struct stat file_stat;
    
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
        
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
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
            char safe_filename[PATH_LENGTH];
            pathToSaveFilename(path, safe_filename);
            if (snprintf(snapshot_path, PATH_LENGTH, "%s/%s.ss", snapshots_dir, safe_filename) >= PATH_LENGTH)
            {
                fprintf(stderr, "Error: File path too long.\n");
                continue;
            }
            
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

            int ct = 0;
            checkPermissions(mode_to_symbolic(myCurrentInfo.st_mode), path, &ct);
            (*dangerous_counter) += ct;
            
            int snapshotStatus = readSnapshot(snapshot_path, &myPreviousInfo);
            if (snapshotStatus == 0) 
            {
                if(searchOverwriteSS(file_stat.st_ino, myCurrentInfo.filename, myPreviousInfo.filename, comp_file, &myPreviousInfo, &myCurrentInfo) == 0)
                {
                    fprintf(comp_file, "file %s added\n\n", myCurrentInfo.filename);
                }
                writeSnapshot(snapshot_path, &myCurrentInfo);
            } 
            else 
            {
                if(comparePrevVsCurr(&myCurrentInfo, &myPreviousInfo, comp_file) > 0)
                {
                    writeSnapshot(snapshot_path, &myCurrentInfo);
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

            // Create formatted path for sym links
            char safe_filename[PATH_LENGTH];
            pathToSaveFilename(path, safe_filename);
            char snapshot_path[PATH_LENGTH];
            if (snprintf(snapshot_path, PATH_LENGTH, "%s/%s.ss", snapshots_dir, safe_filename) >= PATH_LENGTH)
            {
                fprintf(stderr, "Error: File path too long.\n");
                continue;
            }

            
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
            
            writeSnapshot(snapshot_path, &myCurrentInfo);
        }

        // if (isHardLink(path)) 
        // {
        //     printf("%s is a hard link\n", path);
        // }

        if (isDirectory(path)) 
        {
            //printf("%d\n", *dangerous_counter);
            processDirectory(path, snapshots_dir, isolate_dir, dangerous_counter, info_file, comp_file);
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
        if (strcmp(snapshot_file->d_name, ".") == 0 || strcmp(snapshot_file->d_name, "..") == 0) 
        {
            continue;
        }

        char f_name[256];
    
        char *last_underscore = strrchr(snapshot_file->d_name, '_');
        if (last_underscore == NULL) 
        {
            f_name[0] = '\0';
        } 
        else 
        {
            size_t length_after_underscore = strlen(last_underscore + 1);
            if (length_after_underscore <= 3) 
            {
                f_name[0] = '\0';
            } 
            else 
            {
                snprintf(f_name, sizeof(f_name), "%s", last_underscore + 1);
                f_name[length_after_underscore - 3] = '\0';
            }
        }

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

    FILE *comp_file = fopen(comp_out, "w");
    if(comp_file == NULL) 
    {
        perror("Error opening file to write comparisons\n");
        fclose(comp_file);
        exit(EXIT_FAILURE);
    }

    FILE *info_file = fopen(info_out, "w");
    if (info_file == NULL) 
    {
        perror("Error opening output info file\n");
        fclose(info_file);
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

            int dangerous_counter = 0;
            processDirectory(argv[i], snapshots_dir, isolate_dir, &dangerous_counter, info_file, comp_file);
            printf("Snapshot for %s created successfully.\n", argv[i]);
            exit(dangerous_counter); // Return the count of dangerous files as the exit status
        }
    }

    // Wait for all child processes to terminate
    int status;
    pid_t pid;
    while ((pid = wait(&status)) != -1) 
    {
        printf("Process with PID %d ended with code %d and %d dangerous files.\n", pid, WEXITSTATUS(status), WEXITSTATUS(status));
    }

    //Check if there are deleted files by snapshot name
    searchForDeletedFilesInSnapshotsDir(snapshots_dir, argc, argv);
    
    fclose(comp_file);
    fclose(info_file);

    return 0;
}