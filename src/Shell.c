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

int main(void) {
	//Create Buffers for IO
	char input[BUFSIZE];
	char commands[BUFSIZE];
	char* args[BUFSIZE];

	//run shell forever until told to exit
	while(1){
		//Put prompt to user and get their input
		fputs(PROMPT,stdout);
		fgets(input,BUFSIZE,stdin);

		//Tokenize the users input

		int i=0;
		//Loop and store until no more tokens
		while(i<1000){
			char* temp;

			if(i==0){
				temp=strtok(input," \n\t");	//give initial array to tokenize and get first token
			}else{
				temp=strtok(NULL," \n\t");	//give NULL to take tokens from same input starting after previous token
			}

			if(temp==NULL){	//no more tokens
				break;
			}else{//add tokens to args
				args[i]=temp;
				i++;
			}
		}

		//Do some checks on args to determine what to put in command buffer
		if(strcmp(args[0],"clr")==0){
			strcat(commands,"clear");
		}
		else if(strcmp(args[0],"quit")==0){
			break;
		}
		//execute the commands specified
		system(commands);

		//clear buffers
		strcpy(commands,"");
		strcpy(input,"");
		int j=0;
		while(args[j]!=NULL){
			strcpy(args[j],"");
			j++;
		}
	}
	fputs("Shell Terminated\n\r",stdout);
	fflush(stdout);
	return EXIT_SUCCESS;
}
