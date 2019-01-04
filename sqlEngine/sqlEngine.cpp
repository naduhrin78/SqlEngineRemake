// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#define BUFFER_SIZE 1024
#pragma warning(disable: 4996)

#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "utils.h"
#include "UniPret.h"
#include "UniversalScheme.h"
#include "query.h"

//to access join scheme, type View e.g select * from View
//p.s View contains only coherent records for meaningful queries

int main()
{

	char* yolo = (char*)malloc(sizeof(char)* 100);

	while (strcmp(gets(yolo), "exit"))
	{
		UnilineInterpret(trimString(yolo));
	}
	return 0;
}