#include "value.h"
#include "Interpreter.h"

#define DEFAULT_NO_OF_NODES 5

int main(int argc, char** argv)
{
    char FILE_LINES[SIZE];
    int filesize = SIZE;
    int current_file_size = 0;
	int line_no = 1, col_no = 1;
    char* filename = argv[1];
    char* line = FILE_LINES;
    if(strcmp(&filename[strlen(filename) - 4], ".vrx") != 0)
    {
        printf("File format error: Not a .vrx file\n");
        return 0;
    }
    FILE* fp = fopen(filename, "r");
    char c;
    while((c = fgetc(fp)) != EOF){
        line[current_file_size] = c;
        current_file_size++;
        if(current_file_size == filesize)
            expand_file_data(&line, &filesize);
    }

	Context* context = construct_Context();
	int t_size, curr_size = 0, curr_index;
	Token** tokens;
	Node* node;
	Node** nodes;
	int no_of_nodes, max_no_of_nodes;
	Value* val;
    //printf("%d\n", strlen(line));
    //printf("%d\n", strlen(line));
    curr_size = 0;
    
    tokens = Lexer(line, &curr_size, &t_size, &line_no, &col_no);
    // print_token_array(tokens, curr_size);
    if((*tokens)->type == TT_ERROR)
    {
        print_token(*tokens);
        return 0;
    }
    // printf("Flag4 %d\n", t_size);
    // if(tokens != NULL)
    // print_token_array(tokens, curr_size);
    // return 0;
    // else
    // 	printf("\nIt was null lol\n");
    //printf("\n\n\n\n");
    curr_index = 0;
    no_of_nodes = 0, max_no_of_nodes = DEFAULT_NO_OF_NODES;
    nodes = (Node**)calloc(max_no_of_nodes, sizeof(Node*));
    while(curr_index < curr_size && curr_index != TT_EOF){
        node = Parser(tokens, curr_size, &curr_index, 0);
        nodes[no_of_nodes] = node;
        no_of_nodes++;
        if(no_of_nodes == max_no_of_nodes)
            expand_block(&nodes, &max_no_of_nodes);
        
        if(node->nodeType == ERROR_NODE){
            printNode(node, 0);
            return 0;
        }
        // printNode(node, 0);
    }
    // val = viewNode(node, context);
    // printValue(val);
    int isNode[] = {0, 0, 0};
    //Index 0 is for loop node and index 1 is for function node
    Interpreter* interpreter = Interpret(nodes, no_of_nodes, context, isNode);
    // printf("Success\n");
    Value** values = interpreter->values;
    int no_of_values = interpreter->no_of_values;
    if(no_of_values == 1 && (*values)->valType == TT_ERROR)
        printValue(*values);
    // printValues(interpreter);
    line_no++;
    col_no = 1;
	// return 0;
}