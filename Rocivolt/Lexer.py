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
	
	def make_identifier(self):
		identifier = ""
		pos_start = self.pos.deepcopy()
		while self.pos.index != TT_EOF and self.line[self.pos.index] in T_ALPHANUMERICS + '_':
			identifier += self.line[self.pos.index]
			self.move()
		#print(list(T_KEYWORDS.keys()))
		if identifier in T_KEYWORDS.keys():
			return Token(T_KEYWORDS.get(identifier), val = identifier, pos_start = pos_start, pos_end = self.pos)
		return Token(TT_IDENTIFIER, val = identifier, pos_start = pos_start, pos_end = self.pos)
	
	def make_operator(self):
		temp_char = self.line[self.pos.index]
		self.move()
		if self.pos.index != TT_EOF:
			if self.line[self.pos.index] == '=':
				temp_char += self.line[self.pos.index]
				self.move()
			
			if T_OPERATOR.get(temp_char, None) is not None:
				return Token(T_OPERATOR[temp_char], pos_start = self.pos), None
			else:
				return None, CharError(f'Expected \'=\' ahead of \'{temp_char}\'', self.pos)
		return None, EoFError(f'Unexpected End of file with \'{temp_char}\'', self.pos)

	def make_tokens(self):
		tokens = []
		error = None

		while self.pos.index != TT_EOF and error is None:
			if self.line[self.pos.index] in T_DIGITS:
				tokens.append(self.make_number())
			elif self.line[self.pos.index] in T_ALPHABETS:
				tokens.append(self.make_identifier())
			else:
				if self.line[self.pos.index] in T_OPERATOR_KEYS:
					token, error = self.make_operator()
					if error is None and token is not None:
						tokens.append(token)
				elif self.line[self.pos.index] in T_BLOCK_KEYS:
					tokens.append(Token(T_BLOCK[self.line[self.pos.index]], pos_start = self.pos))
					self.move()
				elif self.line[self.pos.index] not in " \t":
					#print("Character =", self.line[self.pos.index])
					#start_pos = self.pos.deepcopy()
					#self.move()
					error = CharError(
						"'"+self.line[self.pos.index]+"'" + " not accepted character in shell",
						self.pos
					)
					break
				else:
					self.move()

		return tokens, error