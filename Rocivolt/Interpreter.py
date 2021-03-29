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
                return f'{self.num}'
        else:
            return f''
    
    def __repr__(self):
        if self.pos is not None:
            if self.isBoolean:
                return f'{T_BACKWARD_BOOLEAN_TRACK[self.num]}'
            else:
                return f'{self.num}'
        else:
            return f''

    def __init__(self, num_token = None, parent_context = None):
        #print(type(num_token).__name__)
        self.error = None
        self.isBoolean = False
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
            elif num_token.type in [TT_TRUE, TT_FALSE]:
                self.num = T_KEYWORDS_VALS[num_token.type]
                self.isBoolean = True
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
        self.isBoolean = False
        self.context = val.context
    
    def subtraction(self, val):
        self.num -= val.num
        self.isBoolean = False
        self.context = val.context
    
    def multiplication(self, val):
        self.num *= val.num
        self.isBoolean = False
        self.context = val.context
    
    def division(self, val):
        if val.num == 0:
            #print(val.context)
            self.error = DivisionByZeroError(val.pos, self.context)
            self.num = None
        else:
            self.num /= val.num
            self.isBoolean = False
            self.context = val.context
    
    def exponent(self, val):
        self.num = self.num ** val.num
        self.isBoolean = False
        self.context = val.context
    
    def greater(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num > val.num]
    
    def lesser(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num < val.num]
    
    def equals(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num == val.num]
    
    def greater_e(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num >= val.num]
    
    def And(self, val):
        self.num = Value.decision[type(val) is Value and (self.num != 0 and val.num != 0)]
        self.isBoolean = True
    
    def Or(self, val):
        self.num = Value.decision[type(val) is Value and (self.num != 0 or val.num != 0)]
        self.isBoolean = True
    
    def lesser_e(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num <= val.num]
    
    def not_equals(self, val):
        self.isBoolean = True
        self.num = Value.decision[self.num != val.num]

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
        if node.token.type != TT_NOT:
            temp_num = node.num_token.val
            if node.token.type == '-':
                temp_num *= -1
            return Value(temp_num, parent_context)
        else:
            value = self.view(node.num_token, parent_context)
            if value.num != 0:
                value.num = 0
            else:
                value.num = 1
            value.isBoolean = True
            return value
        #return "Found UnOpNode"
    
    def view_VarNode(self, node, parent_context):
        #temp_num = parent_context.var_table.get(node.token.val)
        return Value(node.token, parent_context)
    
    def view_VarAssignNode(self, node, parent_context):
        #print(node.var_token.val)
        value = self.view(node.expression, parent_context)
        #print(value)
        if node.token.type == TT_EQUATION:
            parent_context.var_table.setValue(node.var_token.val, value.num)
        else:
            variable_value = parent_context.var_table.get(node.var_token.val)
            if variable_value is None:
                return Value(node.var_token, parent_context)
            elif value.num is None:
                x = Value()
                x.error = NullValueError(
                    "Cannot perform arithmetic operations on Null values", 
                    value.pos, parent_context
                )
                return x
            
            if node.token.type == TT_INCREMENT:
                variable_value += value.num
            elif node.token.type == TT_DECREMENT:
                variable_value -= value.num
            elif node.token.type == TT_PRODUCT_INCREMENT:
                variable_value *= value.num
            elif node.token.type == TT_PRODUCT_DECREMENT:
                if value.num == 0:
                    x = Value()
                    x.error = DivisionByZeroError(value.pos, parent_context)
                    return x
                variable_value /= value.num
            parent_context.var_table.setValue(node.var_token.val, variable_value)

        return Value()
    
    def view_BooleanNode(self, node, parent_context):
        return Value(node.token, parent_context)
    
    def view_ConditionalNode(self, node, parent_context):
        for i in node.blocks:
            condition_value = self.view(i['condition'], parent_context)

            if condition_value.num == 1:
                return self.view(i['body'], parent_context)
        
        if node.else_block is not None:
            return self.view(node.else_block, parent_context)
        else:
            return Value()