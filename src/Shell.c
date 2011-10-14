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

#define BUFSIZE 1000
#define PROMPT "==>"
#define DELIMINATORS " \n\t"

extern char** environ;

FILE * inputStream;
int emitPrompt;

int tokenCheck(int count, int min, int max) {
	if (count>=min && count<=max) return 1;
	fputs("Invalid number of arguments\n", stdout);
	return 0;
}

int main(int argc, char* argv[]) {
	fputs("Starting Shell\n",stdout);
	fflush(stdout);
	//Create Buffers for IO
	char input[BUFSIZE], output[BUFSIZE];
	char commands[BUFSIZE];
	char* args[BUFSIZE];

	if(argc==2){
		inputStream=fopen(argv[1],"r");
		emitPrompt=0;
	} else {
		inputStream=stdin;
		emitPrompt=1;
	}

	setenv("parent", argv[0], 1);

	//run shell forever until told to exit
	while(1){
		int tokenCount;
		char* token;

		fflush(stdout);

		//clear buffers
		strcpy(input,"");
		strcpy(output,"");
		strcpy(commands, "");

		if(emitPrompt) {
			fputs(PROMPT,stdout);
		}

		if(feof(inputStream))
			break;
		if(fgets(input,BUFSIZE,inputStream)==NULL)
			break;

		//Tokenize the users input
		tokenCount=0;
		//Loop and store until no more tokens
		for(token=strtok(input, DELIMINATORS); token!=NULL; token=strtok(NULL, DELIMINATORS)) {
			if(strlen(token)!=0) {
				args[tokenCount]=token;
				tokenCount++;
			}
		}

		//Do some checks on args to determine what to put in command buffer
		if(strcmp(args[0], "cd")==0) {
			if(!tokenCheck(tokenCount, 2, 1)) continue;
			if(tokenCount==1) {
				getcwd(output, BUFSIZE);
				fputs(strcat(output, "\n"), stdout);
			} else {
				if(chdir(args[1])==-1) {
					fputs("path doesn't exist\n", stdout);
				}
			}
		} else if(strcmp(args[0],"clr")==0){
			if(!tokenCheck(tokenCount, 1, 1)) continue;
			strcat(commands,"clear");
		}else if(strcmp(args[0],"quit")==0){
			if(!tokenCheck(tokenCount, 1, 1)) continue;
			break;
		}else if(strcmp(args[0], "environ")==0) {
			if(!tokenCheck(tokenCount, 1, 1)) continue;
			char** current;
			for(current=environ; (*current)!=NULL; current++) {
				fputs(*current, stdout);
				fputs("\n", stdout);
			}
		}else if(strcmp(args[0],"pause")==0){
			if(!tokenCheck(tokenCount, 1, 1)) continue;
			//wait for enter, by taking any input up to it, and not using it.
			fputs("PAUSED, press enter to continue",stdout);
			char temp2[BUFSIZE];
			fgets(temp2,BUFSIZE,stdin);
		}else if(strcmp(args[0], "dir")==0) {
			if(!tokenCheck(tokenCount, 1, 2)) continue;
			if(tokenCount==1) {
				strcat(commands, "ls -la");
			} else {
				strcat(commands,"ls -la ");
				strcat(commands,args[1]);
			}
		}else if(strcmp(args[0],"echo")==0){
			//loop over tokens and output the everything after echo to the screen
			int j;
			for(j=1; j<tokenCount; j++){
				fputs(args[j],stdout);
				fputs(" ",stdout);
			}
			fputs("\n",stdout);
		}else{
			pid_t pid=fork();
			if(pid==0){
				//child
				execvp(args[0],args);
				//child should exit after executing outside program
				return EXIT_SUCCESS;
			}else if(pid<0){
				//error
				return 0;
			}else{
				//parent
				wait(0);
			}
		}
		//execute the commands specified
		if(strlen(commands)!=0) {
			system(commands);

			//uncomment to output system commands for debugging purposes
			//fputs(commands, stdout);
			//fputs("\n", stdout);
		}

		int j;
		for(j=0; args[j]!=NULL; j++){
			args[j]=NULL;
		}
	}

	fputs("Shell Terminated\n\r", stdout);
	fflush(stdout);
	fclose(inputStream);

	return EXIT_SUCCESS;
}
