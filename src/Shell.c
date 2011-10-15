/*
 ============================================================================
 Name        : Shell.c
 Author      : Zachary Cook, Stephanie Gloin, Darryl Barnhart
 Version     : 0.01
 Copyright   : Your copyright notice
 Description : Our Linux Shell Project for Operating Systems
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//The size of text buffers
#define BUFSIZE 1000
//The prompt text
#define PROMPT "==>"
//The deliminators that separate arguments in shell commands
#define DELIMINATORS " \n\t"

extern char** environ; //the system links this pointer to the environment variables

//Check if the token count is between min and max, writing an error message to stdout
//on failure.
//Return 1 on success, 0 otherwise
int tokenCheck(int count, int min, int max) {
	if (count>=min && count<=max) return 1;
	fputs("Invalid number of arguments\n", stdout);
	return 0;
}

int main(int argc, char* argv[]) {
	fputs("Starting Shell\n",stdout); //notify the user that the shell has started
	fflush(stdout); //ensure the start message is written before continuing

	//IO buffers
	char input[BUFSIZE], output[BUFSIZE];
	//System command buffers. Commands that are to be executed by the underlying system
	//are written to this buffer, and executed at the end of each iteration
	char commands[BUFSIZE];
	//Argument buffer. The input commands are tokenized and the tokens stored in this array.
	char* args[BUFSIZE];
	char baseDirectory[BUFSIZE];
	getcwd(baseDirectory, BUFSIZE);

	//The file stream the shell reads input from. This can be stdin or some other stream.
	FILE * inputStream;
	//A flag that indicates if a prompt should be printed to stdout. Typically only enabled
	//when input is coming from stdin (i.e. the user).
	//Set to 1 to enable, 0 to disable.
	int emitPrompt;

	if(argc==2){ //If there is an extra argument, assume it's the location of a batch file
		inputStream=fopen(argv[1],"r");
		if(inputStream==NULL) {	//Ensure the file is valid, and terminate with an error otherwise
			fputs("Invalid file", stdout);
			fflush(stdout);
			return EXIT_SUCCESS;
		}
		emitPrompt=0; //no prompt
	} else { //Otherwise use stdin for input
		inputStream=stdin;
		emitPrompt=1; //show the prompt
	}

	//Set the "parent" environment variable to the path of the shell executable (this path is always
	//the first argument in argv as passed to the main function).
	setenv("parent", argv[0], 1);

	//run shell forever until told to exit
	while(1){
		int tokenCount; //The number of tokens in the current input

		fflush(stdout); //flush the output buffer

		//clear buffers
		strcpy(input,"");
		strcpy(output,"");
		strcpy(commands, "");

		//clear the argument array
		int j;
		for(j=0; args[j]!=NULL; j++){
			args[j]=NULL;
		}

		if(emitPrompt) { //emit the prompt if requested
			//put in the cwd with the prompt
			char cwd[BUFSIZE];
			getcwd(cwd,BUFSIZE);
			fputs(cwd,stdout);
			fputs(PROMPT,stdout);
		}

		if(feof(inputStream))
			break;	//if the input stream is finished, exit the loop
		if(fgets(input,BUFSIZE,inputStream)==NULL)
			break;	//if reading from the input fails, exit the loop

		tokenCount=0; //reset the token count
		//Tokenize the users input
		char* token;
		for(token=strtok(input, DELIMINATORS); token!=NULL; token=strtok(NULL, DELIMINATORS)) {
			if(strlen(token)!=0) { //ignore empty tokens
				args[tokenCount]=token; //add the token to the token array
				tokenCount++;
			}
		}

		//Checks on args to determine the command given
		if(strcmp(args[0], "cd")==0) { //Current Directory
			if(!tokenCheck(tokenCount, 1, 2)) continue; //ensure there's only up to 1 extra argument
			if(tokenCount==1) { //when no arguments, print the working directory
				getcwd(output, BUFSIZE);
				fputs(strcat(output, "\n"), stdout);
			} else { //when an argument given, change the working directory to that directory
				if(chdir(args[1])==-1) {
					//print an error if the directory doesn't exist
					fputs("path doesn't exist\n", stdout);
				}
			}
		} else if(strcmp(args[0],"clr")==0){ //Clear the screen
			if(!tokenCheck(tokenCount, 1, 1)) continue; //ensure there's no extra arguments
			strcat(commands,"clear"); //add the clear system command
		}else if(strcmp(args[0],"quit")==0){ //Quit the shell
			if(!tokenCheck(tokenCount, 1, 1)) continue; //ensure there's no extra arguments
			break; //quit the loop
		}else if(strcmp(args[0], "environ")==0) { //List the environment variables
			if(!tokenCheck(tokenCount, 1, 1)) continue; //ensure there's no extra arguments
			//loop through the environment variables and print them to stdout
			char** current;
			for(current=environ; (*current)!=NULL; current++) {
				fputs(*current, stdout);
				fputs("\n", stdout);
			}
		}else if(strcmp(args[0],"pause")==0){ //Pause the shell and wait for user input
			if(!tokenCheck(tokenCount, 1, 1)) continue; //ensure there's no extra arguments
			//wait for enter, by taking any input up to it, and not using it.
			fputs("PAUSED, press enter to continue",stdout);
			char temp2[BUFSIZE];
			fgets(temp2,BUFSIZE,stdin); //wait for user input. Since it's the user, we use stdin
		}else if(strcmp(args[0], "dir")==0) {
			if(!tokenCheck(tokenCount, 1, 2)) continue; //ensure there's only up to 1 extra arguments
			if(tokenCount==1) { //if no arguments, list the contents of the working directory
				strcat(commands, "ls -la");
			} else { //if a directory is specified, list its contents
				strcat(commands,"ls -la ");
				strcat(commands,args[1]);
			}
		}else if(strcmp(args[0],"echo")==0){ //print all arguments to stdout
			//loop over tokens and output the everything after echo to the screen
			int j;
			for(j=1; j<tokenCount; j++){
				fputs(args[j],stdout);
				fputs(" ",stdout);
			}
			fputs("\n",stdout); //don't forget the newline
		}else if(strcmp(args[0],"help")==0){
			if(!tokenCheck(tokenCount, 1, 1)) continue;
			strcat(commands,"more ");
			strcat(commands, baseDirectory);
			strcat(commands, "/readme");
		}else{ //Otherwise, assume the command is a system executiable, and try to execute it
			pid_t pid=fork(); //Fork this process
			if(pid==0){
				//child
				execvp(args[0],args);
				//child should exit after executing outside program
				return EXIT_SUCCESS;
			}else if(pid<0){
				//error
				return 0;
			}else{
				//parent waits for the child to complete
				wait(0);
			}
		}
		//execute any system commands specified
		if(strlen(commands)!=0) {
			system(commands);

			//uncomment to output system commands for debugging purposes
			//fputs(commands, stdout);
			//fputs("\n", stdout);
		}
	}

	//notify that the shell is closing
	fputs("Shell Terminated\n\r", stdout);
	fflush(stdout); //ensure all output is emitted before terminating
	fclose(inputStream); //close the input stream

	return EXIT_SUCCESS; //done
}
