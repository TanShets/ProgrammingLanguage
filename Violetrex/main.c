#include "value.h"

int main()
{
	char line[SIZE];
	int line_no = 1, col_no = 1;
	Context* context = construct_Context();
	int t_size, curr_size = 0, curr_index;
	Token** tokens;
	Node* node;
	Value* val;
	while(1){
		fgets(line, SIZE, stdin);
		//printf("%d\n", strlen(line));
		line[strlen(line) - 1] = '\0';
		//printf("%d\n", strlen(line));
		curr_size = 0;
		
		tokens = Lexer(line, &curr_size, &t_size, &line_no, &col_no);
		// print_token_array(tokens, curr_size);
		if((*tokens)->type == TT_ERROR)
		{
			print_token(*tokens);
			return 0;
		}
		//printf("Flag4 %d\n", t_size);
		// if(tokens != NULL)
		// print_token_array(tokens, curr_size);
		// else
		// 	printf("\nIt was null lol\n");
		//printf("\n\n\n\n");
		curr_index = 0;
		node = Parser(tokens, curr_size, &curr_index, 0);
		printf("\n");
		if(node->nodeType == ERROR_NODE){
			printNode(node, 0);
			return 0;
		}
		// printNode(node, 0);
		val = viewNode(node, context);
		printValue(val);
		line_no++;
		col_no = 1;
	}
	return 0;
}