from Parser import *
from symbols import *
from Lexer import *
from Error import *
from Context import *

class Value:
    operators = {
        TT_PLUS: 'addition',
        TT_MINUS: 'subtraction',
        TT_MUL: 'multiplication',
        TT_DIV: 'division',
        TT_POWER: 'exponent'
    }

    def __str__(self):
        return f'{self.num}'
    
    def __repr__(self):
        return f'{self.num}'

    def __init__(self, num_token = None, parent_context = None):
        #print(type(num_token).__name__)
        if num_token is not None:
            self.num = num_token.val
            self.pos = num_token.pos_start
            self.context = Context(self.pos.filename, self.pos, parent_context)
        else:
            self.num = 0
            self.pos = None
            self.context = parent_context
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
        self.context = val.context
    
    def subtraction(self, val):
        self.num -= val.num
        self.context = val.context
    
    def multiplication(self, val):
        self.num *= val.num
        self.context = val.context
    
    def division(self, val):
        if val.num == 0:
            #print(val.context)
            self.error = DivisionByZeroError(val.pos, self.context)
            self.num = None
        else:
            self.num /= val.num
            self.context = val.context
    
    def exponent(self, val):
        self.num = self.num ** val.num
        self.context = val.context

class Interpreter:
    def __init__(self, parse_result, parent_context):
        self.num = Value()
        self.parse_result = parse_result
        self.error = None
        self.start(parent_context)
    
    def start(self, parent_context):
        self.num = self.view(self.parse_result, parent_context)
        if self.num.error != None:
            self.error = self.num.error
    
    def getResult(self):
        return self.num, self.error
    
    def view(self, node, parent_context):
        method_name = 'view_' + type(node).__name__

        method_pointer = getattr(self, method_name, 'failure')

        return method_pointer(node, parent_context)
    
    def failure(self, node, parent_context):
        return "Failed to capture any node"
    
    def view_NumNode(self, node, parent_context):
        return Value(node.token, parent_context)
        #return "Found NumNode"
    
    def view_ErrorNode(self, node, parent_context):
        return "Found ErrorNode"
    
    def view_BinOpNode(self, node, parent_context):
        left = self.view(node.left_token, parent_context)
        right = self.view(node.right_token, parent_context)
        #print(left, node.token.type, right)

        left.operation(right, node.token)
        #print(left)
        #return f'({left}, "BinOpNode" = {node.token}, {right})'
        return left
    
    def view_UnOpNode(self, node, parent_context):
        temp_num = node.num_token.val
        if node.token.type == '-':
            temp_num *= -1
        return Value(temp_num, parent_context)
        #return "Found UnOpNode"