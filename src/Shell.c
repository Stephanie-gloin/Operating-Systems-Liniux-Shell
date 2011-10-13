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

#define BUFSIZE 1000
#define PROMPT "==>"
#define DELIMINATORS " \n\t"

int main(void) {
	//Create Buffers for IO
	char input[BUFSIZE], output[BUFSIZE];
	char commands[BUFSIZE];
	char* args[BUFSIZE];

	//run shell forever until told to exit
	while(1){
		int tokenCount;
		char* token;

		//clear buffers
		strcpy(input,"");
		strcpy(output,"");
		strcpy(commands, "");

		//Put prompt to user and get their input
		fputs(PROMPT,stdout);
		fgets(input,BUFSIZE,stdin);

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
		if(tokenCount==1 && strcmp(args[0], "cd")==0) {
			fputs(strcat(getcwd(output, BUFSIZE), "\n"), stdout);
		} else if(tokenCount==2 && strcmp(args[0], "cd")==0) {
			if(chdir(args[1])==-1) {
				fputs("path doesn't exist\n", stdout);
			}
		} else if(tokenCount==1&&strcmp(args[0],"clr")==0){
			strcat(commands,"clear");
		}else if(tokenCount==1&&strcmp(args[0],"quit")==0){
			break;
		}else if(tokenCount==1&&strcmp(args[0],"pause")==0){
			//wait for enter, by taking any input up to it, and not using it.
			fputs("PAUSED, press enter to continue",stdout);
			char temp2[BUFSIZE];
			fgets(temp2,BUFSIZE,stdin);
		}else if(tokenCount==1 && strcmp(args[0], "dir")==0) {
			strcat(commands, "ls -la");
		}else if(tokenCount==2&&strcmp(args[0],"dir")==0){
			strcat(commands,"ls -la ");
			strcat(commands,args[1]);
		}else if(tokenCount>1&&strcmp(args[0],"echo")==0){
			//loop over tokens and output the everything after echo to the screen
			int j;
			for(j=1; j<tokenCount; j++){
				fputs(args[j],stdout);
				fputs(" ",stdout);
			}
			fputs("\n",stdout);
		}
		//execute the commands specified
		if(strlen(commands)!=0) {
			system(commands);
			//fputs(commands, stdout);
			//fputs("\n", stdout);
		}

		int j;
		for(j=0; args[j]!=NULL; j++){
			args[j]=NULL;
		}
	}
	fputs("Shell Terminated\n\r",stdout);
	fflush(stdout);
	return EXIT_SUCCESS;
}
