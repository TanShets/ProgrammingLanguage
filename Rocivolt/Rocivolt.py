from symbols import *
from Error import *
from Lexer import *
from Parser import *
from Interpreter import *
from Context import *

def run(filename, line, parent_context):
	lex = Lexer(line, filename)

	tokens, error = lex.make_tokens()
	#print(tokens)
	parser = Parser(tokens)

	result = parser.parse()
	#print(result)
	if type(result) in [LoopPrecursorNode, FunctionDefinitionPrecursorNode]:
		if filename == 'stdin':
			pass
		else:
			if result.hasStarted:
				return ErrorNode(
					InvalidSyntaxError(
						"Expected '}'", result.token.pos_start
					)
				)
			else:
				return ErrorNode(
					InvalidSyntaxError(
						"Expected '{'", result.token.pos_start
					)
				)
	# #print(result)
	if error is None and type(result) is not ErrorNode:
		interpreter = Interpreter(result, parent_context)
		#print(interpreter.view(result))
		result, error = interpreter.getResult()
	#print(type(result))
	return result, error
	#return tokens, error