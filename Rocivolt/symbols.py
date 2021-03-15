TT_EOF = -1
TT_PLUS = 0
TT_MINUS = 1
TT_MUL = 2
TT_DIV = 3
TT_LPARA = 4
TT_RPARA = 5

T_OPERATOR = {
	'+': TT_PLUS, '-': TT_MINUS, '*': TT_MUL,
	'/': TT_DIV, '(': TT_LPARA, ')': TT_RPARA
}

T_OPERATOR_INVERSE = {
	TT_PLUS: '+', TT_MINUS: '-', TT_MUL: '*',
	TT_DIV: '/', TT_LPARA: '(', TT_RPARA: ')'
}

T_OPERATOR_KEYS = '+-*/()'

TT_INT = 6
TT_FLOAT = 7
TT_ALPHA = 8

T_DIGITS = "0123456789"

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