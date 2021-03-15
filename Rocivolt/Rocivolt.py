from symbols import *
from Error import *
from Lexer import *
from Parser import *
from Interpreter import *

def run(filename, line):
	lex = Lexer(line, filename)

	tokens, error = lex.make_tokens()

	parser = Parser(tokens)

	result = parser.parse()
	
	if error is None and type(result) is not ErrorNode:
		interpreter = Interpreter(result)
		#print(interpreter.view(result))
		result, error = interpreter.getResult()
	#print(type(result))
	return result, error