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
        if self.pos is not None:
            return f'{self.num}'
        else:
            return f''
    
    def __repr__(self):
        if self.pos is not None:
            return f'{self.num}'
        else:
            return f''

    def __init__(self, num_token = None, parent_context = None):
        #print(type(num_token).__name__)
        self.error = None

        if num_token is not None:
            #print("1")
            if num_token.type == TT_IDENTIFIER:
                #print("2")
                if parent_context is not None:
                    #print("3")
                    self.num = parent_context.var_table.get(num_token.val)
                else:
                    self.num = None
                
                if self.num is None:
                    #print("4")
                    self.num = 0
                    self.error = NullValueError(
                                    f'\'{num_token.val}\' has not been initialized', 
                                    num_token.pos_start, parent_context
                                )
            else:
                self.num = num_token.val
            self.pos = num_token.pos_start
            self.context = Context(self.pos.filename, self.pos, parent_context)
        else:
            self.num = 0
            self.pos = None
            self.context = parent_context
    
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
        #print(self.parse_result)
        self.num = self.view(self.parse_result, parent_context)
        if self.num.error != None:
            self.error = self.num.error
    
    def getResult(self):
        return self.num, self.error
    
    def view(self, node, parent_context):
        #print(node)
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
        #print(type(node.left_token).__name__, type(node.right_token).__name__)
        #print(node)
        if type(node.left_token) is VarAssignNode:
            #print(True)
            temp = self.view(node.left_token, parent_context)
            left = self.view(VarNode(node.left_token.var_token), parent_context)
            #print(left)
        else:
            left = self.view(node.left_token, parent_context)
        right = self.view(node.right_token, parent_context)
        #print(left, node.token.type, right)
        #print(left.num, left.error, type(left).__name__)

        if left.error is None and right.error is None:
            left.operation(right, node.token)
        elif right.error is not None:
            left = right
        #print(left)
        #return f'({left}, "BinOpNode" = {node.token}, {right})'
        return left
    
    def view_UnOpNode(self, node, parent_context):
        temp_num = node.num_token.val
        if node.token.type == '-':
            temp_num *= -1
        return Value(temp_num, parent_context)
        #return "Found UnOpNode"
    
    def view_VarNode(self, node, parent_context):
        #temp_num = parent_context.var_table.get(node.token.val)
        return Value(node.token, parent_context)
    
    def view_VarAssignNode(self, node, parent_context):
        #print(node.var_token.val)
        value = self.view(node.expression, parent_context)
        #print(value)
        parent_context.var_table.setValue(node.var_token.val, value.num)
        return Value()