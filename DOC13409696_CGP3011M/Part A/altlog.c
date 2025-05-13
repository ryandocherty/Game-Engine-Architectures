#include "altlog.h"
#include <stdarg.h>

//set up a file
FILE *fp = NULL;

//Begin log and set up file 
void beginLog()
{
#ifdef _DEBUG
	fp = fopen("altlog.txt", "a");

	//error check
	if (fp == NULL)
	{
		printf("Error: Couldn't open file\n");
		exit(1);
	}
#endif
}

void stopLog()
{
	fclose(fp);
}

void selection()
{
#ifdef _DEBUG

	int userInput;

	//Allow user to select options 
	printf("AltLog Initiated\n");
	printf("Press 0 to turn logging off\n");
	printf("Press 1 to log to file");
	printf("Press 2 to log to console\n");
	printf("Press 3 to log to both\n");

	//basic error checking
	//doesn't check for none-numeric values however 
	userInput = scanf("%d", userInput);
	if (userInput < 0 || userInput > 3)
	{
		printf("Invalid Input");
	}

	else
	{
		printf("%d,", userInput);
	}

	logCatagory = userInput;

#endif 

}

void logger(LogMode logLevel, const char *message, ...)
{

#ifdef _DEBUG //this will stop logging while in release 

	va_list args;
	va_start(args, message); //start variadic arguments 
	const char *text = "";

	switch (logLevel)
	{
	case 0: //all
		if (logCatagory == console || both)
		{
			printf("%s", "(All)");
			if (logCatagory == txtfile || both)
			{
				vfprintf(fp, message, args);
			}

	case 1: //performanceInfo
		if (logCatagory == console || both)
		{
			printf("%s", "(Performance)");
			if (logCatagory == txtfile || both)
			{
				vfprintf(fp, message, args);
			}
		}

	case 2: //debugInfo
		if (logCatagory == console || both)
		{
			printf("%s", "(Debug)");
			if (logCatagory == txtfile || both)
			{
				vfprintf(fp, message, args);
			}
		}

	case 3: //errorInfo 
		if (logCatagory == console || both)
		{
			printf("%s", "(Error)");
			if (logCatagory == txtfile || both)
			{
				vfprintf(fp, message, args);

			}
		}
		}
		va_end(args); //end of variadic arguments 
	}
#endif
};




void loggerConsole(char *message)
{
	printf(message);
}