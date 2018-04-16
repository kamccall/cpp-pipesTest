/**
 * dup_demo2.cpp
 * Demo of pipe, fork, dup, execlp coordination.
 * Open a new shell, send multiple commands on multiple lines
 * get the output back to the parent process.
 * 
 * Thurman Gillespy
 * 4/8/18
 */

#include <sys/types.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

static const int BUFFERSIZE = 10000;
static const int RD = 0;
static const int WR = 1;

using namespace std;

int main(int argc, char *argv[])
{
   int fd[2] = {-1, -1};
   int fd2[2] = {-1, -1};
   int pid;
   char buf1[BUFFERSIZE];
   char buf2[BUFFERSIZE];

   if ( (pipe(fd) == -1) )
   {
      cerr << "pipe failed" << endl;
      exit(EXIT_FAILURE);
   }

   if ((pipe(fd) == -1))
   {
      cerr << "pipe failed" << endl;
      exit(EXIT_FAILURE);
   }

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
      dup(fd[RD]); // child fd[RD] now mapped to stdin
      close(1);    // close stdout
      dup(fd2[WR]); // child fd2[WR] now mapped to stdout
      close(fd[RD]);
      close(fd[WR]);
      close(fd2[RD]);
      close(fd2[WR]);
      execlp("bash", "bash",  "-s", NULL);
      exit(EXIT_FAILURE); // only executes if execlp faild
      break;
   default: // parent
      close(fd[RD]);
      close(fd2[WR]);
      int bytes;
      // send data to child, then read the output
      // this is very inefficient, just demoing how to send mutltiple commands,
      // on multiple lines
      strcpy(buf1, "ls -l | egrep '\\.cpp' | wc -l > tmp.txt\n" // store output in tmp file
                  "cat tmp.txt | tr -d ' '\n"   // read file, pile to tr to elim whitepaces
                  "rm -f tmp.txt"); // remove the tmp file
      bytes = write(fd[WR], buf1, strlen(buf1));
      close(fd[WR]);
      bytes = read(fd2[RD], buf2, BUFFERSIZE - 1);
      close(fd[RD]);
      cout << "number of .cpp files in this directory: " << buf2 << endl;
   }

   return(EXIT_SUCCESS);
}