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
			return f'{self.token} {self.condition} {self.change}: {self.block}'
		else:
			return f'{self.token} {self.condition}: {self.block}'
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
					
					start = self.tokens[self.index]
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
							end = self.tokens[self.index]
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
						change = self.tokens[self.index]
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
				change = Token(TT_INT, val = 1)

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
			else:
				return ErrorNode(InvalidSyntaxError(
					"Expected '{' after condition", self.tokens[self.index].pos_start
				))
		else:
			return ErrorNode(InvalidSyntaxError(
				"Expected 'for', 'while'", self.tokens[self.index].pos_start
			))

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
			return ErrorNode("Expected 'if' statement", self.tokens[self.index].start_pos)
		elif self.index != TT_EOF and self.tokens[self.index].type == TT_IF:
			return self.make_conditional_statement()
		
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_WHILE, TT_FOR]:
			return self.make_loop()
		
		if self.index != TT_EOF and self.tokens[self.index].type == TT_NOT:
			token = self.tokens[self.index]
			self.next()
			new_val = self.value()
			return UnOpNode(token, new_val)

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
					new_Node = VarNode(self.tokens[self.index])
			else:
				new_Node = UnOpNode(self.tokens[start_index], self.tokens[self.index])
			self.next()

			if self.index != TT_EOF and self.tokens[self.index].type == TT_POWER:
				new_Node = self.get_operation(self.value, [TT_POWER], left = new_Node)
			return new_Node
		print(self.index)
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
				#print(op_token)
				right_expression = self.expression()
				#print(right_expression)
				return VarAssignNode(left_token, op_token, right_expression)
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
		#print(self.tokens, self.tokens[self.index])
		# if self.index != TT_EOF and self.tokens[self.index].type == TT_IDENTIFIER:
		# 	left_token = self.tokens[self.index]
		# 	#print(left_token)
		# 	self.next()

		# 	valid_var_assign_ops = [TT_EQUATION, TT_INCREMENT, TT_DECREMENT, TT_PRODUCT_INCREMENT, TT_PRODUCT_DECREMENT]
		# 	if self.index != TT_EOF and self.tokens[self.index].type in valid_var_assign_ops:
		# 		op_token = self.tokens[self.index]
		# 		self.next()
		# 		#print(op_token)
		# 		right_expression = self.addsub()
		# 		#print(right_expression)
		# 		return VarAssignNode(left_token, op_token, right_expression)
		# 	else:
		# 		left_expression = VarNode(left_token)
		# 		#print("3", self.tokens[self.index])
		# 		if self.index != TT_EOF and self.tokens[self.index].type in [TT_PLUS, TT_MINUS]:
		# 			return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS], left = left_expression)
		# 		elif self.index != TT_EOF and self.tokens[self.index].type in [TT_MUL, TT_DIV]:
		# 			#print("1")
		# 			return self.get_operation(self.value, [TT_MUL, TT_DIV], left = left_expression)
		# 		elif self.index != TT_EOF and self.tokens[self.index].type == TT_POWER:
		# 			left_expression = self.get_operation(self.value, [TT_POWER], left = left_expression)
		# 			left_expression = self.get_operation(self.value, [TT_MUL, TT_DIV], left = left_expression)
		# 			return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS], left = left_expression)
		# 		elif self.index == TT_EOF:
		# 			return left_expression
		# 		else:
		# 			error = InvalidSyntaxError(
		# 				"Expected '*', '/', '+', '-' or '^'",
		# 				self.tokens[self.index].pos_start
		# 			)

		# 			return ErrorNode(error)

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