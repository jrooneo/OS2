#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileHandler.h"

FILE *fileHandlerRead(){
	FILE *fp;
	char *filename = "producerFile.txt";
	fp = fopen(filename, "r");
	if(fp == NULL){
		perror("Could not open file");
	}
	return fp;
}

FILE *fileHandlerWrite(){
	FILE *fp;
	char *filename = "consumerFile.log";
	fp = fopen(filename, "a");
	if(fp == NULL){
		perror("Could not open file");
	}
	return fp;
}
