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
	# for i in tokens:
	# 	print(i, i.pos_start, i.pos_end)
	parser = Parser(tokens)

	result, index = parser.parse()
	#print(type(result).__name__)
	results = [result]
	while index != TT_EOF and type(result) is not ErrorNode:
		result, index = parser.parse()
		#print(result)
		#print(tokens[index])
		results.append(result)
	
	if type(result) is ErrorNode:
		#print(result)
		error = result
	# if type(result) in [LoopPrecursorNode, FunctionDefinitionPrecursorNode]:
	# 	if filename == 'stdin':
	# 		body = []
	# 		if result.hasStarted:
	# 			print("{")
	# 		else:
	# 			pass
	# 	else:
	# 		if result.hasStarted:
	# 			return ErrorNode(
	# 				InvalidSyntaxError(
	# 					"Expected '}'", result.token.pos_start
	# 				)
	# 			)
	# 		else:
	# 			return ErrorNode(
	# 				InvalidSyntaxError(
	# 					"Expected '{'", result.token.pos_start
	# 				)
	# 			)
	# #print(result)
	true_result = []
	for result in results:
		#print(result)
		if error is None and type(result) is not ErrorNode:
			interpreter = Interpreter(result, parent_context)
			#print(interpreter.view(result))
			interpreted_result, error = interpreter.getResult()
			#print(interpreted_result)
			true_result.append(interpreted_result)
		else:
			true_result = result
			break
	#print(type(result))
	return true_result, error
	#return tokens, error