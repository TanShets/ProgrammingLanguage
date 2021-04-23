from Parser import *
from symbols import *
from Lexer import *
from Error import *
from Context import *

class Value:
    operators = {
        TT_PLUS: 'addition', TT_MINUS: 'subtraction',
        TT_MUL: 'multiplication', TT_DIV: 'division',
        TT_POWER: 'exponent', TT_EQUALS: 'equals',
        TT_GREAT: 'greater', TT_GREAT_EQ: 'greater_e',
        TT_LESS: 'lesser', TT_LESS_EQ: 'lesser_e',
        TT_NOT_EQUALS: 'not_equals', TT_AND: 'And',
        TT_OR: 'Or'
    }

    decision = {
        True: 1, False: 0
    }

    def __str__(self):
        if self.pos is not None:
            if self.isBoolean:
                return f'{T_BACKWARD_BOOLEAN_TRACK[self.num]}'
            else:
                if type(self.num) is not dict:
                    return f'{self.num}'
                else:
                    word = '['
                    keys = list(self.num.keys())
                    for i in range(len(keys)):
                        #print(type(keys[i]).__name__)
                        if type(keys[i]) is StringValue:
                            word += '\''
                            word += keys[i].__str__()
                            word += '\': '
                        
                        if type(self.num[keys[i]]) is StringValue:
                            word += '\''
                            word += self.num[keys[i]].__str__()
                            word += '\''
                        else:
                            word += str(self.num[keys[i]])
                        
                        if i != len(keys) - 1:
                            word += ', '
                    word += ']'
                    return word
        else:
            return f''
    
    def __repr__(self):
        return self.__str__()

    def copy(self):
        new_val = Value()
        new_val.error = self.error
        new_val.error = self.error
        new_val.isBoolean = self.isBoolean
        new_val.isNull = self.isNull
        new_val.num = self.num
        new_val.pos = self.pos
        new_val.context = self.context
        return new_val

    def __init__(self, num_token = None, parent_context = None):
        #print(type(num_token).__name__)
        self.error = None
        self.isBoolean = False
        self.isNull = False
        if num_token is not None:
            #print("1")
            #print(num_token.type)
            if num_token.type == TT_IDENTIFIER:
                #print("2")
                if parent_context is not None:
                    #print("3")
                    temp_val = parent_context.var_table.get(num_token.val)
                    #print("Var", temp_val.isNull)
                    self.num = temp_val.num
                    self.error = temp_val.error
                    self.isBoolean = temp_val.isBoolean
                    self.isNull = temp_val.isNull
                    #print(type(parent_context.var_table.get(num_token.val)).__name__)
                else:
                    self.num = None
                
                if self.num is None:
                    #print("4")
                    self.num = 0
                    self.error = NullValueError(
                                    f'\'{num_token.val}\' has not been initialized', 
                                    num_token.pos_start, parent_context
                                )
            elif num_token.type in [TT_TRUE, TT_FALSE]:
                self.num = T_KEYWORDS_VALS[num_token.type]
                self.isBoolean = True
            elif num_token.type == TT_NULL:
                #print("Made it here")
                self.num = num_token.val
                self.isNull = True
            else:
                self.num = num_token.val
            self.pos = num_token.pos_start
            if self.pos is not None:
                self.context = Context(self.pos.filename, self.pos, parent_context)
            else:
                self.context = parent_context
        else:
            self.num = 0
            self.pos = None
            self.context = parent_context
    
    def operation(self, val, op):
        condition1 = type(val) is Value
        condition2 = type(val.num) in [int, float] and type(self.num) in [int, float]
        condition3 = type(val.num) == type(self.num)
        #print((condition1 and condition2) or condition3)
        if (condition1 and condition2) or condition3:
            #print(True)
            method_name = Value.operators.get(op.type, '')
            #print(method_name)
            if method_name != '':
                #print(self.isNull, val.isNull)
                # print(method_name)
                if (self.isNull or val.isNull) and method_name not in ['equals', 'not_equals']:
                    self.error = NullValueError("Invalid operation with null type", val.pos, self.context)
                    return
                #print("SHS")
                #self.isBoolean = False
                method = getattr(self, method_name)
                #print(False)
                method(val)
            else:
                self.error = FunctionCallError("No such operation found", self.pos, self.context)
        else:
            self.error = InvalidTypeError("Mismatched types", val.pos, self.context)
    
    def raise_error(self, val):
        if (self.isNull or val.isNull):
            self.error = NullValueError("Invalid operation with null type", val.pos, self.context)
            return True
        elif (type(self.num) == str and type(val.num) != str) or (type(self.num) != str and type(val.num) == str):
            self.error = InvalidTypeError("Mismatched value types", val.pos, self.context)
            return True
        return False
    
    def addition(self, val):
        #print("Shamac")
        if self.raise_error(val):
            #print("shs")
            return
        self.num += val.num
        self.isBoolean = False
        self.context = val.context
    
    def subtraction(self, val):
        if self.raise_error(val):
            return
        self.num -= val.num
        self.isBoolean = False
        self.context = val.context
    
    def multiplication(self, val):
        if self.raise_error(val):
            return
        self.num *= val.num
        self.isBoolean = False
        self.context = val.context
    
    def division(self, val):
        if self.raise_error(val):
            return
        
        if val.num == 0:
            #print(val.context)
            self.error = DivisionByZeroError(val.pos, self.context)
            self.num = None
        else:
            self.num /= val.num
            self.isBoolean = False
            self.context = val.context
    
    def exponent(self, val):
        if self.raise_error(val):
            return
        self.num = self.num ** val.num
        self.isBoolean = False
        self.context = val.context
    
    def greater(self, val):
        if self.raise_error(val):
            return
        self.isBoolean = True
        self.num = Value.decision[self.num > val.num]
    
    def lesser(self, val):
        if self.raise_error(val):
            return
        self.isBoolean = True
        self.num = Value.decision[self.num < val.num]
    
    def equals(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num == val.num]
    
    def greater_e(self, val):
        if self.raise_error(val):
            return
        self.isBoolean = True
        self.num = Value.decision[self.num >= val.num]
    
    def And(self, val):
        self.num = Value.decision[type(val) is Value and ((self.num != 0 and val.num != 0) and (not self.isNull and not val.isNull))]
        self.isBoolean = True
    
    def Or(self, val):
        # print(1, type(val) is Value)
        # print(2, self.num != 0 or val.num != 0)
        # print(3, self.isNull and val.isNull)
        self.num = Value.decision[type(val) is Value and ((self.num != 0 and not self.isNull) or (val.num != 0 and not val.isNull))]
        # print("Num", self.num)
        self.isBoolean = True
    
    def lesser_e(self, val):
        if self.raise_error(val):
            return
        self.isBoolean = True
        self.num = Value.decision[self.num <= val.num]
    
    def not_equals(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num != val.num]

class StringValue(Value):
    def __init__(self, string_token = None, parent_context = None):
        if string_token is not None and string_token.type == TT_STRING:
            super().__init__(string_token, parent_context)
        else:
            self.num = None
        
    def operation(self, val, op):
        if type(val) is StringValue or (type(val) is Value and type(val.num) is str):
            #print(True)
            method_name = StringValue.operators.get(op.type, '')

            if method_name != '':
                method = getattr(self, method_name)
                #print(False)
                method(val)
            else:
                self.error = FunctionCallError("Method not found", self.pos)
        else:
            self.error = InvalidTypeError("Mismatched types", val.pos, self.context)
    
    def multiplication(self, val):
        if type(val) is Value:
            for i in range(val.num):
                self.num += self.num
    
    def addition(self, val):
        if type(val) is StringValue:
            super().addition(val)
        else:
            self.error = InvalidTypeError('Cannot add non-string to string', val.pos, self.context)
    
    def subtraction(self, val):
        self.error = InvalidTypeError('Cannot subtract from string', val.pos, self.context)
    
    def division(self, val):
        self.error = InvalidTypeError('Cannot divide with string', val.pos, self.context)
    
    def exponent(self, val):
        self.error = InvalidTypeError("Cannot exponentially raise string", val.pos, self.context)
    
    def And(self, val):
        self.num = StringValue.decision[type(val) is StringValue and self.num != "" and val.num != ""]
        self.isBoolean = True

    def Or(self, val):
        self.num = Value.decision[type(val) is StringValue and (self.num != "" or val.num != "")]
        self.isBoolean = True
        
class ArrayValue(Value):
    def __init__(self, array_token = None, parent_context = None, node = None):
        if array_token is not None and type(array_token) is dict:
            #print(type(array_token))
            self.pos = None
            super().__init__(node.token, parent_context)
            self.values = array_token
            self.num = dict()
            for i in array_token:
                self.num[i] = array_token[i]
        else:
            self.num = None
            self.pos = None
    
    def __str__(self):
        word = '['
        keys = list(self.num.keys())
        for i in range(len(keys)):
            #print(type(keys[i]).__name__)
            if type(keys[i]) is StringValue:
                word += '\''
                word += keys[i].__str__()
                word += '\': '
            
            if type(self.num[keys[i]]) is StringValue:
                word += '\''
                word += self.num[keys[i]].__str__()
                word += '\''
            else:
                word += str(self.num[keys[i]])
            
            if i != len(keys) - 1:
                word += ', '
        word += ']'

        return word
        
    def operation(self, val, op):
        if type(val) is ArrayValue:
            #print(True)
            method_name = ArrayValue.operators.get(op.type, '')

            if method_name != '':
                method = getattr(self, method_name)
                #print(False)
                method(val)
            else:
                self.error = FunctionCallError("No such operation", self.pos)
        else:
            self.error = InvalidTypeError("Mismatched value types", val.pos)
    
    def multiplication(self, val):
        if type(val) is Value:
            for i in range(val.num):
                self.num += self.num
    
    def addition(self, val):
        if type(val) is ArrayValue:
            super().addition(val)
        else:
            self.error = InvalidTypeError('Cannot add non-array to array', val.pos, self.context)
    
    def subtraction(self, val):
        self.error = InvalidTypeError('Cannot subtract from array', val.pos, self.context)
    
    def division(self, val):
        self.error = InvalidTypeError('Cannot divide with array', val.pos, self.context)
    
    def exponent(self, val):
        self.error = InvalidTypeError("Cannot exponentially raise array", val.pos, self.context)
    
    def And(self, val):
        self.num = StringValue.decision[type(val) is StringValue and self.num != [] and val.num != []]
        self.isBoolean = True

    def Or(self, val):
        self.num = Value.decision[type(val) is StringValue and (self.num != [] or val.num != [])]
        self.isBoolean = True
    
    def __repr__(self):
        return f'{self.num}'