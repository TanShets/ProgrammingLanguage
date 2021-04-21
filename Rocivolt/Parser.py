from Lexer import *
from symbols import *

class Node:
	def __init__(self, token):
		self.token = token

	def __repr__(self):
		return f'{self.token}'

class ForConditionNode(Node):
	def __init__(self, var, token, start, to, end):
		super().__init__(token)
		self.var = var
		self.start = start
		self.to = to
		self.end = end
		self.change = None
		self.hasStarted = False

class NumNode(Node):
	pass

class ConditionalNode(Node):
	def __init__(self, blocks, else_block = None):
		super().__init__(blocks[0])
		self.blocks = blocks
		self.else_block = else_block

class BooleanNode(Node):
	pass

class VarNode(Node):
	pass

class VarAssignNode(Node):
	def __init__(self, var_token, token, expression):
		super().__init__(token)
		self.var_token = var_token
		self.expression = expression
	
	def __repr__(self):
		return f'[({self.var_token}, {self.token}, {self.expression})]'

class UnOpNode(Node):
	def __init__(self, token, num_token):
		super().__init__(token)
		self.num_token = num_token
	
	def __repr__(self):
		return f'({self.token}, {self.num_token})'
	
	def get_in_ValNode(self):
		if self.token == '-':
			new_token = self.token.copy(True)
			if new_token.type == TT_IDENTIFIER:
				return VarNode(new_token)
			else:
				return NumNode(new_token)
		else:
			if new_token.type == TT_IDENTIFIER:
				return VarNode(new_token)
			else:
				return NumNode(self.num_token)

class BinOpNode(Node):
	def __init__(self, left_token, token, right_token):
		self.left_token = left_token
		self.right_token = right_token
		super().__init__(token)

	def __repr__(self):
		return f'({self.left_token}, {self.token}, {self.right_token})'

class ErrorNode(Node):
	def __init__(self, error_token):
		super().__init__(error_token)

class LoopNode(Node):
	def __init__(self, token, condition, block, change = None):
		super().__init__(token)
		self.condition = condition
		if self.token.type == TT_FOR and type(self.condition) is ForConditionNode:
			self.condition.change = change
		self.block = block
	
	def __repr__(self):
		if self.token.type == TT_FOR:
			start = '{'
			end = '}'
			return f'{self.token} {self.condition} {self.condition.change} {start} {self.block} {end}'
		else:
			return f'{self.token} {self.condition} {start} {self.block} {end}'

class LoopPrecursorNode(Node):
	def __init__(self, token, condition, hasStarted, change = None):
		super().__init__(token)
		self.condition = condition
		self.change = change
		self.hasStarted = hasStarted

class StringNode(Node):
	pass

class ArrayNode(Node):
	def __init__(self, nodes, keys = None):
		#super().__init__(nodes[0].token)
		self.keys = keys
		self.nodes = nodes
	
	def __repr__(self):
		return f'{self.nodes}'

class FunctionDefinitionNode(Node):
	def __init__(self, function_token, name, parameters, body):
		super().__init__(function_token)
		self.name = name
		self.parameters = parameters
		self.no_of_parameters = len(parameters)
		self.body = body
	
	def __repr__(self):
		return f'<function: {self.name.val}>'

class FunctionDefinitionPrecursorNode(Node):
	def __init__(self, function_token, parameters, hasStarted):
		super().__init__(function_token)
		self.parameters = parameters
		self.hasStarted = hasStarted

class FunctionCallNode(Node):
	def __init__(self, name, parameters):
		super().__init__(name)
		self.name = name
		self.parameters = parameters
'''
value = float or int
prodquo = a * b or a / b
addsub = a + b or a - b

In an expression a - b * c, we look at c and b and a individually
Then look at b * c (prodquo) and then a - (term) -> addsub
'''

class Parser:
	def __init__(self, tokens):
		self.tokens = tokens
		self.n = len(tokens)
		self.index = 0

	def next(self):
		if self.index >= 0 and self.index < self.n - 1:
			self.index += 1
		else:
			self.index = TT_EOF
	
	def peek_next(self):
		if self.index >= 0 and self.index < self.n - 1:
			return self.index + 1
		else:
			return TT_EOF

	def make_conditional_statement(self):
		blocks = []
		is_if = False
		while self.index != TT_EOF and self.tokens[self.index].type in [TT_IF, TT_ELSEIF, TT_ELIF]:
			if is_if and self.tokens[self.index].type == TT_IF:
				return ConditionalNode(blocks)
			
			if (not is_if and self.tokens[self.index].type == TT_IF) or is_if:
				is_if = True
				if_token = self.tokens[self.index]
				self.next()
				condition = self.expression()
				if type(condition) is ErrorNode:
					return condition

				if self.index != TT_EOF and self.tokens[self.index].type == TT_START_BLOCK:
					self.next()
					body_token = self.expression()
					if type(body_token) is ErrorNode:
						return body_token

					if self.index != TT_EOF and self.tokens[self.index].type == TT_END_BLOCK:
						blocks.append({
							'condition': condition,
							'body': body_token
						})
						self.next()
					else:
						return InvalidSyntaxError("Expected '}'", self.tokens[self.index].pos_start)
				else:
					return InvalidSyntaxError("Expected '{'", self.tokens[self.index].pos_start)
			else:
				if not is_if:
					return InvalidSyntaxError(f'Expected \'if\'', self.tokens[self.index].pos_start)
				else:
					return InvalidSyntaxError(f'Expected \'elseif\', \'elif\', \'else\'', self.tokens[self.index].pos_start)
		
		else_block = None
		if self.index != TT_EOF and self.tokens[self.index].type == TT_ELSE:
			self.next()
			if self.tokens[self.index].type == TT_START_BLOCK:
				self.next()
				body_expression = self.expression()
				if type(body_expression) is ErrorNode:
					return body_expression
				elif self.tokens[self.index].type == TT_END_BLOCK:
					else_block = body_expression
					self.next()
				else:
					return InvalidSyntaxError("Expected '}'", self.tokens[self.index].pos_start)

		if len(blocks) == 0:
			return EoFError(f'Unexpected end of file and expecting \'if\'', self.tokens[self.index].pos_start)
		else:
			return ConditionalNode(blocks, else_block)
	
	def make_for_loop_condition(self):
		if self.index != TT_EOF and self.tokens[self.index].type == TT_IDENTIFIER:
			var = self.tokens[self.index]
			self.next()
			if self.index != TT_EOF and self.tokens[self.index].type in [TT_EQUATION, TT_IN]:
				eq = self.tokens[self.index]
				self.next()
				if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT, TT_MINUS]:
					if self.tokens[self.index].type == TT_MINUS:
						self.next()
						if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT]:
							self.tokens[self.index].val *= -1
						else:
							return ErrorNode(InvalidSyntaxError(
								"Expected numerical value", self.tokens[self.index].pos_start
							))
					
					start = NumNode(self.tokens[self.index])
					self.next()
					if self.index != TT_EOF and self.tokens[self.index].type == TT_TO:
						to = self.tokens[self.index]
						self.next()
						if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT, TT_MINUS]:
							if self.tokens[self.index].type == TT_MINUS:
								self.next()
								if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT]:
									self.tokens[self.index].val *= -1
								else:
									return ErrorNode(InvalidSyntaxError(
										"Expected numerical value", self.tokens[self.index].pos_start
									))
							end = NumNode(self.tokens[self.index])
							self.next()
							return ForConditionNode(var, eq, start, to, end)
						else:
							return ErrorNode(InvalidSyntaxError(
								"Expected numerical value", self.tokens[self.index].pos_start
							))
					else:
						return ErrorNode(InvalidSyntaxError(
							"Expected 'to'", self.tokens[self.index].pos_start
						))
				else:
					return ErrorNode(InvalidSyntaxError(
						"Expected numerical value", self.tokens[self.index].pos_start
					))
			else:
				return ErrorNode(InvalidSyntaxError(
					"Expected '='", self.tokens[self.index].pos_start
				))
		else:
			return ErrorNode(InvalidSyntaxError(
				"Expected an iterative variable", self.tokens[self.index].pos_start
			))

	def make_loop(self):
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_WHILE, TT_FOR]:
			loop_token = self.tokens[self.index]
			self.next()
			if loop_token.type == TT_WHILE:
				condition = self.expression()
			else:
				condition = self.make_for_loop_condition()
			
			if type(condition) is ErrorNode:
				return condition

			change = None
			if self.index != TT_EOF and loop_token.type == TT_FOR and self.tokens[self.index].type == TT_CHANGE:
				self.next()
				if self.index != TT_EOF and self.tokens[self.index].type == TT_EQUATION:
					self.next()
					if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT, TT_MINUS]:
						if self.tokens[self.index].type == TT_MINUS:
							self.next()
							if self.index != TT_EOF and self.tokens[self.index].type in [TT_INT, TT_FLOAT]:
								self.tokens[self.index].val *= -1
							else:
								return ErrorNode(InvalidSyntaxError(
									"Expected 'int', 'float' value", self.tokens[self.index].pos_start
								))
						change = NumNode(self.tokens[self.index])
						self.next()
					else:
						return ErrorNode(InvalidSyntaxError(
							"Expected 'int', 'float' value", self.tokens[self.index].pos_start
						))
				else:
					return ErrorNode(InvalidSyntaxError("Expected '='", self.tokens[self.index].pos_start))
			elif self.index != TT_EOF and self.tokens[self.index].type == TT_CHANGE:
				return ErrorNode(InvalidSyntaxError(
					"Unexpected term 'change' in a while loop", self.tokens[self.index].pos_start
				))
			elif loop_token.type == TT_FOR:
				change = NumNode(Token(TT_INT, val = 1))

			if self.index != TT_EOF and self.tokens[self.index].type == TT_START_BLOCK:
				self.next()
				block = []
				while self.index != TT_EOF and self.tokens[self.index].type != TT_END_BLOCK:
					line = self.expression()
					block.append(line)
				
				if self.index == TT_EOF:
					return ErrorNode(InvalidSyntaxError(
						"Expected '}'", self.tokens[self.index].start_pos
					))
				self.next()
				
				return LoopNode(loop_token, condition, block, change = change)
			elif self.index == TT_EOF:
				return LoopPrecursorNode(loop_token, condition, False, change = change)
			else:
				return ErrorNode(InvalidSyntaxError(
					"Expected '{' after condition", self.tokens[self.index].pos_start
				))
		else:
			return ErrorNode(InvalidSyntaxError(
				"Expected 'for', 'while'", self.tokens[self.index].pos_start
			))

	def make_function(self):
		if self.index != TT_EOF and self.tokens[self.index].type == TT_FUNCTION:
			function_token = self.tokens[self.index]
			self.next()
			if self.index != TT_EOF:
				if self.tokens[self.index].type == TT_IDENTIFIER:
					name = self.tokens[self.index]
					self.next()
				else:
					name = None
				
				if self.index != TT_EOF and self.tokens[self.index].type == TT_LPARA:
					self.next()
					parameters = []
					if self.index != TT_EOF and self.tokens[self.index].type in [TT_IDENTIFIER]:
						val_x = self.value()
						if type(val_x) is not ErrorNode:
							parameters.append(val_x)
							while self.index != TT_EOF and self.tokens[self.index].type == TT_COMMA:
								self.next()
								val_x = self.value()
								if self.index != TT_EOF and type(val_x) == VarNode:
									parameters.append(val_x)
								else:
									return val_x
						else:
							return val_x
							
					if self.index != TT_EOF and self.tokens[self.index].type == TT_RPARA:
						self.next()
						if self.index != TT_EOF and self.tokens[self.index].type == TT_START_BLOCK:
							body = []
							self.next()
							if self.index == TT_EOF:
								return FunctionDefinitionPrecursorNode(function_token, name, parameters, True)

							while self.index != TT_EOF and self.tokens[self.index].type != TT_END_BLOCK:
								expr = self.expression()
								if type(expr) is ErrorNode:
									return expr
								body.append(expr)
							
							if self.index != TT_EOF and self.tokens[self.index].type == TT_END_BLOCK:
								self.next()
								return FunctionDefinitionNode(function_token, name, parameters, body)
							else:
								return ErrorNode(InvalidSyntaxError(
									"Expected '}'", self.tokens[self.index].pos_start
								))
						elif self.index == TT_EOF:
							return FunctionDefinitionPrecursorNode(function_token, name, parameters, False)
						else:
							return ErrorNode(InvalidSyntaxError(
								"Expected '{'", self.tokens[self.index].pos_start
							))
					else:
						return ErrorNode(InvalidSyntaxError(
							"Expected ')'", self.tokens[self.index].pos_start
						))
				else:
					return ErrorNode(InvalidSyntaxError(
						"Expected '('", self.tokens[self.index].pos_start
					))
			else:
				return ErrorNode(InvalidSyntaxError(
					"Expected '(' or name of function", self.tokens[self.index].pos_start
				))
		else:
			return ErrorNode(InvalidSyntaxError(
				"Expected 'function' or 'fn'", self.tokens[self.index].pos_start
			))

	def make_function_call(self):
		if self.index != TT_EOF and self.tokens[self.index].type == TT_IDENTIFIER:
			name = self.tokens[self.index]
			self.next()
			if self.index != TT_EOF and self.tokens[self.index].type == TT_LPARA:
				self.next()
				parameters = []
				if self.index != TT_EOF and self.tokens[self.index].type in [TT_IDENTIFIER, TT_MINUS, TT_INT, TT_FLOAT]:
					temp_val = self.value()
					if type(temp_val) in [LoopNode, ConditionalNode, ErrorNode]:
						return ErrorNode(InvalidSyntaxError(
							"Expected numerical value or variable or boolean", 
							self.tokens[self.index].pos_start
						))
					
					parameters.append(temp_val)

					while self.index != TT_EOF and self.tokens[self.index].type == TT_COMMA:
						self.next()
						if self.index != TT_EOF and self.tokens[self.index].type in [TT_IDENTIFIER, TT_MINUS, TT_INT, TT_FLOAT]:
							temp_val = self.value()
							parameters.append(temp_val)
						else:
							return ErrorNode(InvalidSyntaxError(
								"Expected numerical value or variable or boolean",
								self.tokens[self.index].pos_start
							))
				#print(self.tokens[self.index], self.index == TT_EOF)
				if self.index != TT_EOF and self.tokens[self.index].type == TT_RPARA:
					return FunctionCallNode(name, parameters)
				else:
					return ErrorNode(InvalidSyntaxError(
						"Expected ')'", self.tokens[self.index].pos_start
					))
		else:
			return ErrorNode(InvalidSyntaxError(
				"Expected identifier", self.tokens[self.index].pos_start
			))

	def make_array(self):
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_ARRAY, TT_ARRAY_BLOCK_START]:
			temp_start = self.tokens[self.index]
			self.next()
			end_char_type = None
			if temp_start.type == TT_ARRAY:
				if self.tokens[self.index].type == TT_LPARA:
					end_char_type = TT_RPARA
					self.next()
				else:
					return ErrorNode(InvalidSyntaxError(
						"Expected '('", self.tokens[self.index].pos_start
					))
			else:
				end_char_type = TT_ARRAY_BLOCK_END
			
			node_list = dict()
			keys = []
			isContinued = True
			index_counter = 0
			while self.index != TT_EOF and self.tokens[self.index].type != end_char_type and isContinued:
				node = self.comparison_expression()
				#node_list.append(node)
				if self.index != TT_EOF and self.tokens[self.index].type == TT_ARRAY_KEY_VAL_SEPARATOR:
					key = node
					self.next()
					node = self.comparison_expression()
					if type(node) is not ErrorNode:
						keys.append(key)
						node_list[key] = node
					else:
						return node
				else:
					new_key = NumNode(Token(TT_INT, val = int(index_counter), pos_start = node.token.pos_start))
					node_list[new_key] = node
					keys.append(new_key)
					index_counter += 1

				if self.index != TT_EOF and self.tokens[self.index].type == TT_COMMA:
					self.next()
				else:
					isContinued = False
			
			if self.index == TT_EOF and self.tokens[self.index].type != end_char_type:
				val_char = {TT_RPARA: ')', TT_ARRAY_BLOCK_END: ']'}
				return ErrorNode(InvalidSyntaxError(
					f'Expected \'{val_char[end_char_type]}\'', self.tokens[self.index].pos_start
				))
			elif self.index != TT_EOF and isContinued:
				return ErrorNode(InvalidSyntaxError(
					"Expected another value before ending array", self.tokens[self.index].pos_start
				))
			else:
				self.next()
				return ArrayNode(node_list, keys = keys)
		else:
			return ErrorNode(InvalidSyntaxError("Expected 'array', '['", self.tokens[self.index].pos_start))

	def value(self):
		#print(self.tokens[self.index])
		if self.index != TT_EOF and self.tokens[self.index].type == TT_LPARA:
			self.next()
			new_Node = self.expression()
			if type(new_Node) is ErrorNode:
				return new_Node
			elif self.index == TT_EOF or self.tokens[self.index].type != TT_RPARA:
				return ErrorNode(InvalidSyntaxError(
					"Expected ')'", self.tokens[self.index].pos_start
				))
			self.next()
			return new_Node
		
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_ELIF, TT_ELSEIF, TT_ELSE]:
			return ErrorNode(InvalidSyntaxError("Expected 'if' statement", self.tokens[self.index].pos_start))
		elif self.index != TT_EOF and self.tokens[self.index].type == TT_IF:
			return self.make_conditional_statement()
		
		if self.index != TT_EOF and self.tokens[self.index].type == TT_FUNCTION:
			return self.make_function()
		
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_WHILE, TT_FOR]:
			return self.make_loop()
		
		if self.index != TT_EOF and self.tokens[self.index].type == TT_NOT:
			token = self.tokens[self.index]
			self.next()
			new_val = self.value()
			return UnOpNode(token, new_val)
		
		if self.index != TT_EOF and self.tokens[self.index].type == TT_STRING:
			token = self.tokens[self.index]
			self.next()
			return StringNode(token)
		
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_ARRAY, TT_ARRAY_BLOCK_START]:
			return self.make_array()

		if self.index != TT_EOF and self.tokens[self.index].type in [TT_PLUS, TT_MINUS]:
			start_index = self.index
			self.next()
		else:
			start_index = TT_EOF

		if self.index != TT_EOF and self.tokens[self.index].type in [TT_TRUE, TT_FALSE, TT_FLOAT, TT_INT, TT_IDENTIFIER]:
			if start_index == TT_EOF:
				if self.tokens[self.index].type in [TT_FLOAT, TT_INT]:
					new_Node = NumNode(self.tokens[self.index])
				elif self.tokens[self.index].type in [TT_TRUE, TT_FALSE]:
					new_Node = BooleanNode(self.tokens[self.index])
				else:
					#new_Node = VarNode(self.tokens[self.index])
					temp_index = self.peek_next()
					if temp_index != TT_EOF and self.tokens[temp_index].type == TT_LPARA:
						new_Node = self.make_function_call()
					else:
						new_Node = VarNode(self.tokens[self.index])
			else:
				new_Node = UnOpNode(self.tokens[start_index], self.tokens[self.index])
			
			if type(new_Node) is ErrorNode:
				self.next()
				return new_Node
			self.next()

			if self.index != TT_EOF and self.tokens[self.index].type == TT_POWER:
				new_Node = self.get_operation(self.value, [TT_POWER], left = new_Node)
			return new_Node
		#print(self.index)
		error = InvalidSyntaxError(
			"Not a numerical value", 
			self.tokens[self.index].pos_start
		)

		return ErrorNode(error)

	def expression(self):
		if self.index != TT_EOF and self.tokens[self.index].type == TT_IDENTIFIER:
			left_token = self.tokens[self.index]
			#print(left_token)
			temp_index = self.peek_next()

			valid_var_assign_ops = [TT_EQUATION, TT_INCREMENT, TT_DECREMENT, TT_PRODUCT_INCREMENT, TT_PRODUCT_DECREMENT]
			if temp_index != TT_EOF and self.tokens[temp_index].type in valid_var_assign_ops:
				self.next()
				op_token = self.tokens[self.index]
				self.next()
				right_expression = self.expression()
				#print(type(right_expression).__name__)
				if type(right_expression) is not FunctionDefinitionNode:
					return VarAssignNode(left_token, op_token, right_expression)
				elif type(right_expression) is FunctionDefinitionNode and op_token.type == TT_EQUATION:
					right_expression.name = left_token
					return right_expression
				elif type(right_expression) is ErrorNode:
					return right_expression
				else:
					return ErrorNode(InvalidSyntaxError(
						f'Unexpected \'{T_OPERATOR_INVERSE[op_token.type]}\' instead of \'=\'',
						self.tokens[self.index].pos_start
					))
			# else:
			# 	left_expression = VarNode(left_token)
			# 	#print("3", self.tokens[self.index])
			# 	if self.index != TT_EOF and self.tokens[self.index].type in [TT_PLUS, TT_MINUS]:
			# 		return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS], left = left_expression)
			# 	elif self.index != TT_EOF and self.tokens[self.index].type in [TT_MUL, TT_DIV]:
			# 		#print("1")
			# 		return self.get_operation(self.value, [TT_MUL, TT_DIV], left = left_expression)
			# 	elif self.index != TT_EOF and self.tokens[self.index].type == TT_POWER:
			# 		left_expression = self.get_operation(self.value, [TT_POWER], left = left_expression)
			# 		left_expression = self.get_operation(self.value, [TT_MUL, TT_DIV], left = left_expression)
			# 		return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS], left = left_expression)
			# 	elif self.index == TT_EOF:
			# 		return left_expression
			# 	else:
			# 		error = InvalidSyntaxError(
			# 			"Expected '*', '/', '+', '-' or '^'",
			# 			self.tokens[self.index].pos_start
			# 		)

			# 		return ErrorNode(error)
		return self.get_operation(self.comparison_expression, [TT_AND, TT_OR])

	def comparison_expression(self):
		return self.get_operation(self.addsub, [TT_LESS, TT_LESS_EQ, TT_GREAT, TT_GREAT_EQ, TT_EQUALS, TT_NOT_EQUALS])

	def addsub(self):
		return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS])

	def prodquo(self):
		return self.get_operation(self.value, [TT_MUL, TT_DIV])

	def get_operation(self, function, operators, left = None):
		if left is None:
			left_expression = function()
		else:
			left_expression = left
		#print(self.tokens)
		#print("2", left_expression)
		if type(left_expression) is ErrorNode:
			return left_expression

		op_token = None
		while self.index != TT_EOF and self.tokens[self.index].type in operators:
			op_token = self.tokens[self.index]
			self.next()
			#print(op_token)
			right_expression = function()
			#print(right_expression)
			if type(right_expression) is ErrorNode:
				return right_expression

			left_expression = BinOpNode(left_expression, op_token, right_expression)
		
		if op_token != None and type(op_token) is ErrorNode:
			return op_token

		return left_expression

	def parse(self):
		result = self.expression()

		if type(result) is ErrorNode:
			return result
		elif self.index != TT_EOF:
			return ErrorNode(InvalidSyntaxError(
				"Not an operator",
				self.tokens[self.index].pos_start
			))
		return result