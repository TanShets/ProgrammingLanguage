#include <stdio.h>
#include "symbols.h"

int add(int a, int b)
{
	return a + b;
}

int main(){
	char c = 'a';
	int count = 0;
	printf("%s", INT_TO_STR(0));
	while((c = getchar()) != EOF)
		if(c != '\n') printf("%c%d", c, count++);
	return 0;
}