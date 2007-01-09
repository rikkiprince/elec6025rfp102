#include "print.h"

void printn(int n, char *str)
{
	int i=0;
	int l=strlen(str);
	for(i=0; i<n && i<l; i++, str++)
	{
		putchar(*str);
	}
}

void repeat(int n, char c)
{
	int i=0;

	for(i=0; i<n; i++)
		putchar(c);
}

void print(char *str)
{
	int w = 76;
	int l = strlen(str);

	if(l>(w-4))
	{
		printf("= ");
		printn(w-4, str);
		printf(" =\n");
	}
	else
	{
		int a = (w - l)/2;

		repeat(a, '=');
		printf(" %s ", str);
		repeat(a, '=');
		putchar('\n');
	}
}
