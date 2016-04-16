/* Authors: SAMUEL  Capotosto
            DANIEL Dusabimana
*/

#include <unistd.h>
#include <stdio.h>//standard input
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>
//.
#define size_B 2048 // find and replace. 
#define mode_copy 0644 // how you give a directory a file permission. 
//.
//.declaring Globals
void display_File(char* , struct stat *);
struct stat fileInfo;// this is stat structure 
struct dirent **fileList;// this is pointer to pointer dirent
char * fileExit;// pointer to a charactor 
char readin[size_B];// Array of charators..
int isDirectory(const char *);//Passing a string into the function which returns int
void oops(char *,  char *);//functions that take two strings that return nothing 
char course[size_B];// arrays of charactors
char myProg[size_B];
char filesList[size_B];
char currentDirectory[size_B];
//.char directPath[size_B];
char DPA[size_B];// Direct Path Associate
char dirPath[size_B];
char listFile[size_B];
void files_To_Submit();// fun that return and accpet nothing. 
static char *buf;//  a pointer to a charactor
static char FilesReadyTogo[size_B];//
//.static char rKey = rn;
void printFiles();
struct dirent ** fileList;
//.
int hiddenFiles(const struct dirent *);
int dirExists(char * path);
const char * getUserName();
void displayFiles();
void Files_To_Submit();
char *formatString(char *string, int len);
void copyFiles(char *, char *, char *);
int cp(const char *to, const char *from);
//.
//.
//.
int main()
{
    //---------------Number 1: read in 256 characters max----------------------
    //course name exists?
    printf("\nEnter Course Name : ");
    scanf("%s", readin);// this takes a string  and %d takes a number
    if(dirExists(readin) < 0) {
        printf("\nmySubmit - directory error : %s", readin);
        printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        //.exit(1);
    }
    //---------------Number 2: submit exists? --------------------------------------------------------
    strcat(readin, "/submit");
    if(dirExists(readin) < 0) {
        printf("\nmySubmit - directory error : %s", readin);
        printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        //.exit(2);
    }
    //.
    strcat(readin, "/");
    strcat(readin, getUserName());
    if(dirExists(readin) < 0) {
        printf("\nmySubmit - user submit directory not present : %s", readin);
        printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        printf("\nmySubmit - creating user submit directory : %s", readin);
        mkdir(readin, 0777);
    }
    //.
    char readinassign[256];
    //----------------Number 5: Get the name of the myProg and add a subdirectory-----------------
    printf("\nEnter the Assignment Name : ");
    scanf("%s", readinassign);
    strcpy(myProg, readinassign);
    strcat(readin, "/");
    strcat(readin, readinassign);
    if(dirExists(readin) < 0) {
        printf("\nmySubmit - assignment directory not present : %s", readin);
        printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        printf("\nmySubmit - creating assignment directory : %s", readin);
        mkdir(readin, 0777);
    }
    strcpy(DPA, readin);
    strcpy(dirPath, readin);
    buf = (char *)malloc(size_B * sizeof(char));
    //.
    displayFiles();
    files_To_Submit(readin);
    printFiles();
    //.
    free(buf);
    return(0);
}


//does the directory exist?
int dirExists(char * path)
{
    struct stat fileStat;
    char workingdir[size_B];
    strcpy(workingdir, "./");
    strcat(workingdir, path);
    return stat(workingdir, &fileStat);
}

//get the username
const char * getUserName()
{
    uid_t uid = geteuid();
    struct passwd * pw = getpwuid(uid);
    if(pw) {
        return pw->pw_name;
    }
    return "";
}


void displayFiles()
{
    //--------------Number 6: Display our list of files-----------------------------------
    //name, size, modification date and time
    int filesLength = scandir(".", &fileList, hiddenFiles, alphasort);
    printf("\n%9s %s %s %3s\n", "SIZE", "DATE", "MODIFIED", "NAME");
    int i;
    for(i = 0; i < filesLength; i++) {
        fileExit = fileList[i]->d_name;
        if(!(isDirectory(fileExit))) {
            if(stat(fileExit, &fileInfo) == -1) {
                printf("\nmySubmit - file list error : %s", readin);
                printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
            }
            else {
                display_File(fileExit, &fileInfo);
            }
        }
    }
}

//---------- Method to print out the information from the Filelist-----------------------//
void display_File(char * fileN, struct stat * eInfo)
{
    long file_size = (long)eInfo->st_size;
    char * info = 4 + ctime(&eInfo->st_mtime);
    printf("%9ld %.14s %s\n", file_size, info, fileN);
}


//----------- Number 7: Ask the user for the names of the files to submit-------------------------------
void files_To_Submit()
{
    int filesLength = 0;
    char readin[size_B];
    char * src = malloc(size_B);
    char * dest = malloc(size_B);
    char * copiedFiles = malloc(size_B);
    char cwd[size_B * sizeof(char)];
    //.
    getcwd(cwd, size_B * sizeof(char));
    filesLength = scandir(".", &fileList, hiddenFiles, alphasort);
    printf("\nWhich files would you like to submit ? : ");
    scanf("%s", readin);
    if(readin[0] == '*') {
      int i;
        for(i = 0; i < filesLength; i++) {
            copiedFiles = fileList[i]->d_name;
            if(!(isDirectory(copiedFiles))) {
                strcpy(src, cwd);
                strcat(src, "/");
                strcpy(dest, DPA);
                strcat(dest, "/");
                copyFiles(copiedFiles, dest, src);
                //.free(fileList[i]);
            }
        }
    }
    else {
        strcpy(listFile, readin);
        copiedFiles = strtok(listFile, " ");
        while(copiedFiles) {
            strcpy(src, currentDirectory);
            strcat(src, "/");
            strcpy(dest, DPA);
            copyFiles(copiedFiles, dest, src);
            copiedFiles = strtok(NULL, " ");
        }
    }
}

//------------------Number 8: Method to copy files -------------------------------
void copyFiles(char * myFiles, char * d, char * s)
{
  char dest[size_B];
  char src[size_B];

  strcpy(dest, d);
  strcat(dest, "/");
  strcat(dest, myFiles);
  strcpy(src, myFiles);

  printf("Source: %s Dest: %s", src, dest);

    cp(dest, src);
  /*  printf("%s", myFiles);
    char dest[size_B];
    char src[size_B];
    int inDirectory, outDirectory, nChars, wChars;
    //.
    strcpy(dest, d);
    strcat(dest, myFiles);
    strcpy(src, s);
    strcat(src, myFiles);
    //.printf("\ncopyFiles( %s, %s )", dest, src);
    //.
    if(isDirectory(myFiles) != 1) {
        if((inDirectory = open(src, O_RDONLY)) == -1) {
            printf("\nmySubmit - src file open error : %s", src);
            printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        }
        if((outDirectory = creat(dest, mode_copy)) == -1) {
            printf("\nmySubmit - dst file open error : %s", src);
            printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        }
        else {
            strcat(FilesReadyTogo, myFiles);
            strcat(FilesReadyTogo, " ");
        }
        while((nChars = (int)read(inDirectory, buf, size_B * size_B)) > 0) {
            wChars = 0;
            if((wChars = (int)(write(outDirectory, buf, nChars))) != nChars) {
                printf("\nmySubmit - dst file write error : %s", dest);
                printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
            }
        }
        if(nChars == -1) {
            printf("\nmySubmit - src file read error : %s", src);
            printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        }
        if(close(inDirectory) == 1 || close(outDirectory) == 1) {
            printf("\nmySubmit - file close error : [ %s ] & [ %s ] ", src, dest);
            printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
        }
    }
    else {
        printf("mySubmit will not copy only files, not directories,\" %s\" These are not included.\n", myFiles);
        }*/
}

//
int cp(const char *to, const char *from)
{
  //From and to buffers
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  //Open the from buffer as read only
  fd_from = open(from, O_RDONLY);
  if (fd_from < 0)
    return -1;

  //open the to buffer in create mode
  fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0)
    goto out_error;

  //While we can still read
  while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
      char *out_ptr = buf;
      ssize_t nwritten;

      //Write to the file from the read file
      do {
        nwritten = write(fd_to, out_ptr, nread);
        //If it was successful, decrement the amoune read by the amount written
        if (nwritten >= 0)
          {
            nread -= nwritten;
            out_ptr += nwritten;
          }
        else if (errno != EINTR)
          {
            goto out_error;
          }
      } while (nread > 0);
    }

  if (nread == 0)
    {
      if (close(fd_to) < 0)
        {
          fd_to = -1;
          goto out_error;
        }
      close(fd_from);

      return 0;
    }

  //Handle errors
 out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0)
    close(fd_to);

  errno = saved_errno;
  return -1;
}

//------------------------ printing Successfull copied files --------------------
void printFiles()
{
    int i, nRet;
    char * file;
    //.
    i = nRet = 0;
    printf("\nThe files submitted for \"%s\" are:\n, ", myProg);
    file = strtok(FilesReadyTogo, " ");
    while(file) {
        printf("%s\n", file);
        file = strtok(NULL, " ");
    }
    printf("\n");
    if((nRet = chdir(dirPath)) == -1) {
        printf("\nmySubmit - chdir error : %s", dirPath);
        printf("\nErrNum : %d : [ %s ]\n", errno, strerror(errno));
    }
    char cwd [size_B * sizeof(char)];
    getcwd(cwd, size_B * sizeof(char));
    printf("%s\n", cwd);
    displayFiles();
}

//-----------------Hide files function---------------------------------------
int hiddenFiles(const struct dirent * entry)
{
    if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
        return(0);
    }
    else {
        return(1);
    }
}

//------------------ Number
char * formatString(char *string, int len)
{
    char * newStr;
    newStr = malloc(size_B);
    strcat(newStr, string);
    long i;
    for(i = strlen(string); i < len; i++) {
        strcat(newStr, " ");
    }
    return newStr;
}

//---------------------Handling Errors---------------------------------
int isDirectory(const char * path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

//---------------------Handling Errors---------------------------------
void oops(char * strX, char * strY)
{
  printf("Oops : %s - %s", strX, strY);
}
