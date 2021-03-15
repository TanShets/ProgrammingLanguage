from Lexer import *
from symbols import *

class Node:
	def __init__(self, token):
		self.token = token

	def __repr__(self):
		return f'{self.token}'

class NumNode(Node):
	def __init__(self, token):
		super().__init__(token)

class UnOpNode(Node):
	def __init__(self, token, num_token):
		super().__init__(token)
		self.num_token = num_token
	
	def __repr__(self):
		return f'({self.token}, {self.num_token})'
	
	def get_in_NumNode(self):
		if self.token == '-':
			new_token = self.token.copy(True)
			return NumNode(new_token)
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

	def value(self):
		if self.index != TT_EOF and self.tokens[self.index].type == TT_LPARA:
			self.next()
			new_Node = self.addsub()

			if type(new_Node) is ErrorNode:
				return new_Node
			elif self.index == TT_EOF or self.tokens[self.index].type != TT_RPARA:
				return ErrorNode(InvalidSyntaxError(
					"Expected ')'", self.tokens[self.index].pos_start
				))
			self.next()
			return new_Node
		
		if self.index != TT_EOF and self.tokens[self.index].type in [TT_PLUS, TT_MINUS]:
			start_index = self.index
			self.next()
		else:
			start_index = TT_EOF

		if self.index != TT_EOF and self.tokens[self.index].type in [TT_FLOAT, TT_INT]:
			if start_index == TT_EOF:
				new_Node = NumNode(self.tokens[self.index])
			else:
				new_Node = UnOpNode(self.tokens[start_index], self.tokens[self.index])
			self.next()
			return new_Node
		
		error = InvalidSyntaxError(
			"Not a numerical value", 
			self.tokens[self.index].pos_start
		)

		return ErrorNode(error)

	def addsub(self):
		return self.get_operation(self.prodquo, [TT_PLUS, TT_MINUS])

	def prodquo(self):
		return self.get_operation(self.value, [TT_MUL, TT_DIV])

	def get_operation(self, function, operators):
		left_expression = function()
		#print(self.tokens)
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
		result = self.addsub()

		if type(result) is ErrorNode:
			return result
		elif self.index != TT_EOF:
			return ErrorNode(InvalidSyntaxError(
				"Not an operator",
				self.tokens[self.index].pos_start
			))
		return result