from symbols import *
from Context import *

class Error:
	def __init__(self, name, body, pos):
		self.name = name
		self.body = body
		self.pos = pos

	def __str__(self):
		'''
		statement = self.name + ": in " + self.start.filename + ", " + self.end.line_no
		statement += ", " + self.end.col_no + ": "
		return statement + self.body
		'''
		return f'{self.name}: in {self.pos.filename}, line {self.pos.line_no}, col {self.pos.col_no}: {self.body}\n'

	def __repr__(self):
		return self.__str__()

class CharError(Error):
	def __init__(self, body, pos):
		super().__init__("CharError", body, pos)

class InvalidSyntaxError(Error):
	def __init__(self, body, pos):
		super().__init__("InvalidSyntaxError", body, pos)

class DivisionByZeroError(Error):
	def __init__(self, pos, context = None):
		self.context = context
		super().__init__("DivisionByZeroError", "Cannot divide value by 0", pos)
	
	def __repr__(self):
		if self.context is None:
			return super().__repr__()
		else:
			temp_context = context
			sentence = ""
			print(True)
			while temp_context is not None:
				sentence += f'{temp_context}'
			sentence += super().__repr__()