#include "Parser.h"

int main()
{
	char line[SIZE];
	int line_no = 1, col_no = 1;
	fgets(line, SIZE, stdin);
	//printf("%d\n", strlen(line));
	line[strlen(line) - 1] = '\0';
	//printf("%d\n", strlen(line));
	int t_size, curr_size = 0;
	
	Token** tokens = Lexer(line, &curr_size, &t_size, &line_no, &col_no);
	//printf("Flag4 %d\n", t_size);
	// if(tokens != NULL)
	// 	print_token_array(tokens, curr_size);
	// else
	// 	printf("\nIt was null lol\n");
	//printf("\n\n\n\n");
	int curr_index = 0;
	Node* node = Parser(tokens, curr_size, &curr_index);
	printf("\n");
	printNode(node, 0);
	return 0;
}