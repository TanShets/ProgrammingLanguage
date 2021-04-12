TT_EOF = -1
TT_NULL = 'null'
TT_PLUS = 0
TT_MINUS = 1
TT_MUL = 2
TT_DIV = 3
TT_LPARA = 4
TT_RPARA = 5
TT_INT = 6
TT_FLOAT = 7
TT_ALPHA = 8
TT_POWER = 9

TT_IDENTIFIER = 10
TT_EQUATION = 11

TT_LESS = 12
TT_LESS_EQ = 13
TT_GREAT = 14
TT_GREAT_EQ = 15
TT_EQUALS = 16
TT_NOT_EQUALS = 17

TT_INCREMENT = 18
TT_DECREMENT = 19
TT_PRODUCT_INCREMENT = 20
TT_PRODUCT_DECREMENT = 21 #Basically division

TT_AND = 22
TT_OR = 23
TT_NOT = 24
TT_TRUE = 25
TT_FALSE = 26

TT_IF = 27
TT_ELSEIF = 28
TT_ELIF = 29
TT_ELSE = 30
TT_START_BLOCK = 31
TT_END_BLOCK = 32

TT_FOR = 33
TT_CHANGE = 34
TT_WHILE = 35
TT_IN = 36
TT_TO = 37

TT_FUNCTION = 38
TT_COMMA = 39

TT_STRING = 40
TT_ESCAPE_CHARACTER = 41

T_OPERATOR = {
	'+': TT_PLUS, '-': TT_MINUS, '*': TT_MUL,
	'/': TT_DIV, '^': TT_POWER, '=': TT_EQUATION, 
	'<': TT_LESS, '<=': TT_LESS_EQ, '>': TT_GREAT, 
	'>=': TT_GREAT_EQ, '==': TT_EQUALS, '!=': TT_NOT_EQUALS, 
	'+=': TT_INCREMENT, '-=': TT_DECREMENT, '*=': TT_PRODUCT_INCREMENT, 
	'/=': TT_PRODUCT_DECREMENT, ',': TT_COMMA
}

T_BLOCK = {
	'{': TT_START_BLOCK, '}': TT_END_BLOCK,
	'(': TT_LPARA, ')': TT_RPARA
}

T_OPERATOR_INVERSE = {
	TT_PLUS: '+', TT_MINUS: '-', TT_MUL: '*',
	TT_DIV: '/', TT_LPARA: '(', TT_RPARA: ')',
	TT_POWER: '^', TT_EQUATION: '='
}

T_KEYWORDS = {
	'and': TT_AND, 'or': TT_OR, 'not': TT_NOT,
	'true': TT_TRUE, 'false': TT_FALSE, 'if': TT_IF,
	'elseif': TT_ELSEIF, 'elif': TT_ELIF, 'else': TT_ELSE,
	'for': TT_FOR, 'change': TT_CHANGE, 'while': TT_WHILE,
	'in': TT_IN, 'to': TT_TO, 'function': TT_FUNCTION,
	'fn': TT_FUNCTION
}

T_KEYWORDS_VALS = {
	TT_TRUE: 1, TT_FALSE: 0
}

T_BACKWARD_BOOLEAN_TRACK = ['false', 'true']

T_OPERATOR_KEYS = '+-*/^=><!,'
T_BLOCK_KEYS = '}{()'
T_STRING_KEYS = '"\''

T_DIGITS = "0123456789"
T_ALPHABETS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'
T_ALPHANUMERICS = T_DIGITS + T_ALPHABETS

class Position:
	def __init__(self, line_no, col_no, index, filename, file_text):
		self.line_no = line_no
		self.col_no = col_no
		self.index = index
		self.filename = filename
		self.file_text = file_text

	def move(self):
		if self.index < len(self.file_text) - 1 and self.index >= 0:
			self.index += 1
			if self.file_text[self.index] == '\n':
				self.line_no += 1
				self.col_no = 0
				self.index += 1
			else:
				self.col_no += 1
		else:
			self.index = TT_EOF

	def deepcopy(self):
		return Position(self.line_no, self.col_no, self.index, self.filename, self.file_text)