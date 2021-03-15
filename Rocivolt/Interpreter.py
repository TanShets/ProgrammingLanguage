from Parser import *
from symbols import *
from Lexer import *
from Error import *

class Value:
    operators = {
        TT_PLUS: 'addition',
        TT_MINUS: 'subtraction',
        TT_MUL: 'multiplication',
        TT_DIV: 'division'
    }

    def __str__(self):
        return f'{self.num}'
    
    def __repr__(self):
        return f'{self.num}'

    def __init__(self, num_token = None):
        #print(type(num_token).__name__)
        if num_token is not None:
            self.num = num_token.val
            self.pos = num_token.pos_start
        else:
            self.num = 0
            self.pos = None
        self.error = None
    
    def operation(self, val, op):
        if type(val) is Value:
            #print(True)
            method_name = Value.operators.get(op.type, '')

            if method_name != '':
                method = getattr(self, method_name)
                #print(False)
                method(val)
    
    def addition(self, val):
        self.num += val.num
    
    def subtraction(self, val):
        self.num -= val.num
    
    def multiplication(self, val):
        self.num *= val.num
    
    def division(self, val):
        if val.num == 0:
            self.error = DivisionByZeroError(val.pos)
            self.num = None
        else:
            self.num /= val.num

class Interpreter:
    def __init__(self, parse_result):
        self.num = Value()
        self.parse_result = parse_result
        self.error = None
        self.start()
    
    def start(self):
        self.num = self.view(self.parse_result)
        if self.num.error != None:
            self.error = self.num.error
    
    def getResult(self):
        return self.num, self.error
    
    def view(self, node):
        method_name = 'view_' + type(node).__name__

        method_pointer = getattr(self, method_name, 'failure')

        return method_pointer(node)
    
    def failure(self):
        return "Failed to capture any node"
    
    def view_NumNode(self, node):
        return Value(node.token)
        #return "Found NumNode"
    
    def view_ErrorNode(self, node):
        return "Found ErrorNode"
    
    def view_BinOpNode(self, node):
        left = self.view(node.left_token)
        right = self.view(node.right_token)
        #print(left, node.token.type, right)

        left.operation(right, node.token)
        #print(left)
        #return f'({left}, "BinOpNode" = {node.token}, {right})'
        return left
    
    def view_UnOpNode(self, node):
        temp_num = node.num_token.val
        if node.token.type == '-':
            temp_num *= -1
        return Value(temp_num)
        #return "Found UnOpNode"