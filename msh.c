// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12     // Mav shell only supports eleven arguments

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  // History and pid array max size
  int hn = 15;
  // Index variable for history and pid array 
  int n = 0;
  // Declare an array to store history and pids 
  char *hist[hn];
  int pidarr[hn];

  // Empty both arrays
  for( int i = 0; i < hn; i++ )
  {
    hist[i] = NULL;
    pidarr[i] = 0;
  }

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    // New prompt printed if no string input is entered 
    if (command_string[0] == '\n') {
      continue;
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      token[i] = NULL;
    }

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;                                         

    // If command_string[0] == '!'
    // then declare an integer index
    // set index to atoi|( &command_string[1] )
    // strncpy from hist[index] to command_string
    if(command_string[0] == '!')
    {
      int exIndex = atoi(&command_string[1]);
      strcpy(command_string, hist[exIndex]);
    }

    char *working_string  = strdup( command_string );

    // Store entire input string in history
    // move history up so last 15 inputs are always displayed
    hist[n] = strdup(command_string);
    if( n == 15)
    {
      n = n - 1;
      for(int i = 0; i < n; i++)
      {
        hist[i] = hist[i+1];
      }
      hist[n] = strdup(command_string);
    } 

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
      token_count++;
    }
    // Store pid in array
    pid_t pid;
    pidarr[n] = pid;

    // Implement built-ins
    // fork pid
    // Call execvp to handle other commands
    if(strcmp(token[0], "q") == 0)
    {
      exit(0);
    }
    else if (strcmp(token[0], "quit") == 0)
    {
      exit(0);
    }
    else if (strcmp(token[0], "exit") == 0) 
    {
      exit(0);
    }
    else if (strcmp(token[0], "cd") == 0) 
    {
      chdir(token[1]);
    }
    else if (strcmp(token[0], "history") == 0)
    {
      if (token[1] && strcmp(token[1], "-p") == 0)
      {
        for(int i = 0; i < hn; i++)
          {
            if(hist[i] != NULL) 
            {
              printf("%d: %s", i, hist[i]);
              printf("Command %d has pid %d\n", i, pidarr[i]);
            }
          }
      }
      else
      {
          for(int i = 0; i < hn; i++)
          {
            if(hist[i] != NULL) 
            {
              printf("%d: %s", i, hist[i]);
            }
          }
      }
    }
    else
    {
      pid = fork();
      if(pid == 0)
      {
        int ret = execvp( token[0], &token[0] );  
        if( ret == -1 )
        {
          printf("%s: Command not found.\n", token[0]);
          return 0;
        }
      }
      else 
      {
        int status;
        wait( & status );
      }
    }

    // Increment inndex variable 
    n++;

    // Cleanup allocated memory
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }

    free( head_ptr );

  }

  free( command_string );

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}