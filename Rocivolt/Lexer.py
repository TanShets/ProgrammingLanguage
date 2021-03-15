from symbols import *
from Error import *

class Token:
	def __init__(self, type_, val = None, pos_start = None, pos_end = None):
		self.type = type_
		self.val = val

		self.pos_start = pos_start
		self.pos_end = pos_end

	def __repr__(self):
		if self.val: 
			return "["+str(self.type) + " : " + str(self.val)+"]"
		return "[" + str(self.type) + "]"
	
	def copy(self, isNegative = False):
		new_val = self.val
		if isNegative:
			new_val *= -1
		return Token(self.type, new_val, self.pos_start, self.pos_end)

class Lexer:
	def __init__(self, line, filename):
		self.line = line
		self.filename = filename
		self.pos = Position(0, 0, 0, filename, line)

	def move(self):
		self.pos.move()

	def make_number(self):
		isFloat = False
		num = ""
		pos_start = self.pos.deepcopy()
		while self.pos.index != TT_EOF and self.line[self.pos.index] in T_DIGITS + '.':
			if self.line[self.pos.index] == '.':
				if isFloat:
					break
				else:
					isFloat = True
			num += self.line[self.pos.index]
			self.move()
		if isFloat:
			return Token(TT_FLOAT, val = float(num), pos_start = pos_start, pos_end = self.pos)
		else:
			return Token(TT_INT, val = int(num), pos_start = pos_start, pos_end = self.pos)


	def make_tokens(self):
		tokens = []
		error = None

		while self.pos.index != TT_EOF:
			#print(self.line[self.pos.index])
			if self.line[self.pos.index] in T_DIGITS:
				tokens.append(self.make_number())
			else:
				if self.line[self.pos.index] in T_OPERATOR_KEYS:
					tokens.append(Token(T_OPERATOR[self.line[self.pos.index]], pos_start = self.pos))
				elif self.line[self.pos.index] not in " \t":
					#print("Character =", self.line[self.pos.index])
					#start_pos = self.pos.deepcopy()
					#self.move()
					error = CharError(
						"'"+self.line[self.pos.index]+"'" + " not accepted character in shell",
						self.pos
					)
					break
				self.move()

		return tokens, error