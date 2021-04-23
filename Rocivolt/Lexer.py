from symbols import *
from Error import *

class Token:
	def __init__(self, type_, val = None, pos_start = None, pos_end = None, proceeding_char = None):
		self.type = type_
		self.val = val

		self.pos_start = pos_start
		self.pos_end = pos_end

		self.proceeding_char = proceeding_char

	def __repr__(self):
		if self.val is not None: 
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
		self.pos = Position(1, 0, 0, filename, line)
		#print(self.pos)

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
			return Token(TT_FLOAT, val = float(num), pos_start = pos_start.deepcopy(), pos_end = self.pos.deepcopy())
		else:
			return Token(TT_INT, val = int(num), pos_start = pos_start.deepcopy(), pos_end = self.pos.deepcopy())
	
	def make_identifier(self):
		identifier = ""
		pos_start = self.pos.deepcopy()
		while self.pos.index != TT_EOF and self.line[self.pos.index] in T_ALPHANUMERICS + '_':
			#print(self.line[self.pos.index])
			identifier += self.line[self.pos.index]
			self.move()
		#print(list(T_KEYWORDS.keys()))
		if identifier in T_KEYWORDS.keys():
			proceeding_char = None
			if identifier == 'return':
				if self.pos.index == TT_EOF:
					proceeding_char = '\n'
				else:
					proceeding_char = self.line[self.pos.index]
			
			#print(pos_start, self.pos)
			return Token(
				T_KEYWORDS.get(identifier), val = identifier, pos_start = pos_start.deepcopy(), 
				pos_end = self.pos.deepcopy(), proceeding_char = proceeding_char
				)
		#print(pos_start, self.pos)
		return Token(TT_IDENTIFIER, val = identifier, pos_start = pos_start.deepcopy(), pos_end = self.pos.deepcopy())
	
	def make_operator(self):
		temp_char = self.line[self.pos.index]
		self.move()
		if self.pos.index != TT_EOF:
			if self.line[self.pos.index] == '=':
				temp_char += self.line[self.pos.index]
				self.move()
			
			if T_OPERATOR.get(temp_char, None) is not None:
				return Token(T_OPERATOR[temp_char], pos_start = self.pos.deepcopy()), None
			else:
				return None, CharError(f'Expected \'=\' ahead of \'{temp_char}\'', self.pos)
		return None, EoFError(f'Unexpected End of file with \'{temp_char}\'', self.pos)
	
	def make_string(self):
		temp_val = self.line[self.pos.index]
		self.move()
		hasEscape = False
		ended = False
		word = ""
		qualified_escape = {
			temp_val: temp_val, 'n': '\n', 't': '\t', "\\": "\\"
		}
		qualified_escape_keys = temp_val + "nt\\"
		start_pos = self.pos.deepcopy()
		while self.pos.index != TT_EOF and (self.line[self.pos.index] != temp_val or hasEscape):
			valx = self.line[self.pos.index]
			if hasEscape:
				if valx in qualified_escape_keys:
					word += qualified_escape[valx]
					hasEscape = False
				else:
					return None, CharError(f'Expected \'\\n\', \'\\t\', {temp_val}, instead got \'{valx}\'', self.pos)
			else:
				if valx == "\\":
					hasEscape = True
				else:
					word += valx
			self.move()
		
		if self.pos.index == TT_EOF and self.line[self.pos.index] != temp_val:
			return None, EoFError(f'Unexpected end of file with \'{self.line[self.pos.index]}\'', self.pos)
		
		self.move()
		return Token(TT_STRING, val = word, pos_start = start_pos.deepcopy(), pos_end = self.pos.deepcopy()), None

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
					tokens.append(Token(T_BLOCK[self.line[self.pos.index]], pos_start = self.pos.deepcopy()))
					self.move()
				elif self.line[self.pos.index] in T_STRING_KEYS:
					token, error = self.make_string()
					if error is None and token is not None:
						tokens.append(token)
				elif self.line[self.pos.index] not in [' ', '\t', '\n']:
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