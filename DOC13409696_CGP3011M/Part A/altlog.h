#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef __altlog_h__
#define __altlog_h__ 

#define none		0
#define txtfile		1
#define console		2
#define both		3



//enums will help with tagging 
typedef enum
{
	
	debugInfo = 0,
	errorInfo = 1,
	PerformanceInfo = 2,
	positionInfo = 3 

} LogMode; 
LogMode logInfo; 

//methods 
void selection(); 
void beginLog(); 
void endLog(); 
void logger(char *message);
void loggerConsole(char *message); 
int logCatagory;

#endif

