/**
 * dup_demo.cpp
 * Demo of pipe, fork, dup, execlp coordination.
 * 
 * Thurman Gillespy
 * 4/8/18
 */

#include <sys/types.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

static const int BUFFERSIZE = 10000;
static const int RD = 0;
static const int WR = 1;

using namespace std;

int main(int argc, char *argv[])
{
   int fd1[2] = {-1, -1};
   int pid;
   char buf1[BUFFERSIZE];
   char buf2[BUFFERSIZE];

   if ( (pipe(fd1) == -1) )
   {
      cerr << "pipe failed" << endl;
      exit(EXIT_FAILURE);
   }

   // have child process get a long list (ls -l) of all the files
   // in current directory, pipe results back to parent process
   pid = fork();
   switch (pid)
   {
      case (-1):
         cerr << "fork failed" << endl;
         exit(EXIT_FAILURE);
         break;
      case (0):         // child (forked) process
         close(1);      // close stdout
         dup(fd1[WR]);   // child fd[WR] now mapped to stdout
         // close(fd1[RD]);
         // close(fd1[WR]);
         execlp("ls", "ls", "-al", NULL);
         exit(EXIT_FAILURE);// only executes if execlp faild
         break;
      default: // parent
         close(fd1[WR]);
         int bytes;
         wait(&pid); // wait for child
         bytes = read(fd1[RD], buf1, BUFFERSIZE - 1);
         // close(fd1[RD]);
         cout << "All files in this directory: " << endl;
         cout << buf1 << endl;
   }

   // have child process just show us the .cpp files (grep .cpp)
   if ((pipe(fd1) == -1))
   {
      cerr << "pipe failed" << endl;
      exit(EXIT_FAILURE);
   }
   int fd2[2] = {-1, -1};
   if ((pipe(fd2) == -1))
   {
      cerr << "pipe failed" << endl;
      exit(EXIT_FAILURE);
   }

   
   pid = fork();
   switch (pid)
   {
   case (-1):
      cerr << "fork failed" << endl;
      exit(EXIT_FAILURE);
      break;
   case (0):       // child (forked) process
      close(0);    // close stdin
      dup(fd1[RD]); // child fd[RD] now mapped to stdin
      close(1);    // close stdout
      dup(fd2[WR]); // child fd2[WR] now mapped to stdout
      close(fd1[RD]);
      close(fd1[WR]);
      close(fd2[RD]);
      close(fd2[WR]);
      execlp("grep", "grep",  "sour", NULL);
      exit(EXIT_FAILURE); // only executes if execlp faild
      break;
   default: // parent
      close(fd1[RD]);
      close(fd2[WR]);
      int bytes;
      //  send data to child, then read the output
      bytes = write(fd1[WR], buf1, strlen(buf1));
      close(fd1[WR]);
      bytes = read(fd2[RD], buf2, BUFFERSIZE - 1);
      close(fd1[RD]);
      cout << "All the .cpp files: " << endl;
      cout << buf2 << endl;
   }

   return(EXIT_SUCCESS);
}
