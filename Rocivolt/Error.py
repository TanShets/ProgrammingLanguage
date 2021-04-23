from symbols import *
# from Context import *

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

class EoFError(Error):
	def __init__(self, body, pos):
		super().__init__("EOFError", body, pos)

class DivisionByZeroError(Error):
	def __init__(self, pos, context = None):
		self.context = context
		super().__init__("DivisionByZeroError", "Cannot divide value by 0", pos)
	
	def __repr__(self):
		sentence = ""
		if self.context is None:
			temp_context = context
			#sentence = ""
			#print(True)
			while temp_context is not None:
				sentence += f'{temp_context}'
				temp_context = temp_context.parent
		sentence += super().__repr__()
		return sentence

class NullValueError(Error):
	def __init__(self, body, pos, context):
		self.context = context
		super().__init__("NullValueError", body, pos)
	
	def __repr__(self):
		sentence = ""
		if self.context is not None:
			temp_context = context
			#sentence = ""
			while temp_context is not None:
				sentence += f'{temp_context}'
				temp_context = temp_context.parent
		sentence += super().__repr__()
		return sentence

class RunTimeError(Error):
	def __init__(self, body, pos):
		super().__init__("RunTimeError", body, pos)

class FunctionDefinitionError(Error):
	def __init__(self, body, pos):
		super().__init__("FunctionDefinitionError", body, pos)

class FunctionCallError(Error):
	def __init__(self, body, pos):
		super().__init__("FunctionCallError", body, pos)

class InvalidTypeError(Error):
	def __init__(self, body, pos, context = None):
		self.context = context
		super().__init__("InvalidTypeError", body, pos)
	
	def __repr__(self):
		sentence = ""
		if self.context is None:
			temp_context = context
			#sentence = ""
			#print(True)
			while temp_context is not None:
				sentence += f'{temp_context}'
				temp_context = temp_context.parent
		sentence += super().__repr__()
		return sentence

class InvalidStringError(Error):
	def __init__(self, body, pos, context = None):
		self.context = context
		super().__init__("InvalidStringError", body, pos)