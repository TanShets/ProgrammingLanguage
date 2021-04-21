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
                    self.num = parent_context.var_table.get(num_token.val).num
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

class StringValue(Value):
    def __init__(self, string_token = None, parent_context = None):
        if string_token is not None and string_token.type == TT_STRING:
            super().__init__(string_token, parent_context)
        else:
            self.num = None
        
    def operation(self, val, op):
        if type(val) is StringValue:
            #print(True)
            method_name = StringValue.operators.get(op.type, '')

            if method_name != '':
                method = getattr(self, method_name)
                #print(False)
                method(val)
    
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

class Interpreter:
    def __init__(self, parse_result, parent_context):
        self.num = Value()
        self.parse_result = parse_result
        self.error = None
        self.start(parent_context)
    
    def start(self, parent_context):
        #print(self.parse_result)
        self.num = self.view(self.parse_result, parent_context)
        if self.num is not None and type(self.num) is Value and self.num.error != None:
            self.error = self.num.error
    
    def getResult(self):
        return self.num, self.error
    
    def view(self, node, parent_context):
        #print(node)
        method_name = 'view_' + type(node).__name__
        #print(method_name)
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
        #print(type(left.num).__name__, type(right.num).__name__)
        #print(left, node.token.type, right)
        #print(left.num, left.error, type(left).__name__)

        if left.error is None and right.error is None:
            #print(type(left).__name__, type(right).__name__)
            left.operation(right, node.token)
        elif right.error is not None:
            left = right
        #print(left)
        #return f'({left}, "BinOpNode" = {node.token}, {right})'
        return left
    
    def view_StringNode(self, node, parent_context):
        return StringValue(node.token, parent_context)
    
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
            parent_context.var_table.setValue(node.var_token.val, value)
        else:
            variable_value = parent_context.var_table.get(node.var_token.val)
            if variable_value is None:
                return Value(node.var_token, parent_context)
            elif value is None or value.num is None:
                x = Value()
                x.error = NullValueError(
                    "Cannot perform arithmetic operations on Null values", 
                    value.pos, parent_context
                )
                return x
            elif value is not None and value.error is not None:
                return value
            
            x_type = type(variable_value).__name__
            if node.token.type == TT_INCREMENT:
                stat1 = (x_type == 'Value' and type(value) is Value)
                stat2 = (x_type == 'StringValue' and type(value) is StringValue)
                stat3 = (x_type == 'ArrayValue' and type(value) is ArrayValue)
                if stat1 or stat2 or stat3:
                    variable_value.addition(value)
                else:
                    self.error = InvalidTypeError("Mismatched values for operation", value.pos, parent_context)
            elif node.token.type == TT_DECREMENT and (x_type in ['float', 'int'] and type(value) is Value):
                variable_value.subtract(value)
            elif node.token.type == TT_PRODUCT_INCREMENT and (x_type in ['float', 'int'] and type(value) is Value):
                variable_value.multiply(value)
            elif node.token.type == TT_PRODUCT_DECREMENT and (x_type in ['float', 'int'] and type(value) is Value):
                if value.num == 0:
                    x = Value()
                    x.error = DivisionByZeroError(value.pos, parent_context)
                    return x
                variable_value.divide(value)
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
    
    def view_ForConditionNode(self, node, parent_context):
        tok_a = node.start.token
        tok_b = node.end.token
        tok_c = node.change.token

        if node.change.token.val == 0:
            val = Value()
            val.error = RunTimeError(
                "change in loop given 0 value - Infinite loop (If such a loop is needed, use while instead)", 
                node.change.token.pos_start
            )
            return val
        elif (tok_a.val < tok_b.val and tok_c.val < 0) or (tok_a.val > tok_b.val and tok_c.val > 0):
            val = Value()
            temp_pos = node.change.token.pos_start
            if node.change.token.pos_start is None:
                temp_pos = node.end.token.pos_start
            val.error = RunTimeError(
                "Infinite loop encountered", temp_pos
            )
            return val
        
        if parent_context.var_table.get(node.var.val) is None or node.hasStarted is False:
            start_value = self.view(node.start, parent_context)
            parent_context.var_table.setValue(node.var.val, start_value)
            node.hasStarted = True
            #node.index += 1
            return Value(Token(T_KEYWORDS['true']), parent_context)
        
        temp_val = parent_context.var_table.get(node.var.val)
        change_val = self.view(node.change, parent_context)
        temp_val.addition(change_val)

        if (tok_c.val > 0 and temp_val.num >= tok_b.val) or (tok_c.val < 0 and temp_val <= tok_b.val):
            parent_context.var_table.setValue(node.var.val, temp_val)
            return Value(Token(T_KEYWORDS['false']), parent_context)
        else:
            parent_context.var_table.setValue(node.var.val, temp_val)
            #node.index += 1
            return Value(Token(T_KEYWORDS['true']), parent_context)
    
    def view_LoopNode(self, node, parent_context):
        condition = self.view(node.condition, parent_context)
        values = []
        while condition.num not in [0, 'false']:
            for i in node.block:
                value = self.view(i, parent_context)
                values.append(value)
                #print(value)
                if type(value) in [StringValue, Value] and value.error is not None:
                    self.error = value.error
                    break
                elif type(value) is list and len(value) > 0 and type(value[0]) in [StringValue, Value]:
                    for i in value:
                        if i.error is not None:
                            self.error = i.error
                            break
                    
                    if self.error is not None:
                        break
            
            if self.error is not None:
                break
            condition = self.view(node.condition, parent_context)
        node.condition.hasStarted = False
        
        if condition.error is not None:
            return condition
        
        #print(values)
        return values
    
    def view_FunctionDefinitionNode(self, node, parent_context):
        outcome = parent_context.setFunction(len(node.parameters), node)
        if type(outcome) is FunctionDefinitionError:
            self.error = outcome
        return Value()
    
    def view_FunctionCallNode(self, node, parent_context):
        # print(node)
        # print(node.parameters)
        function_definition = parent_context.get(len(node.parameters), node.name)
        if function_definition is None:
            self.error = FunctionCallError(f'Function: {node.name} not previously defined', node.token.pos_start)
            return Value()
        else:
            for i in range(len(node.parameters)):
                parameter_value = self.view(node.parameters[i], parent_context)
                #print("Val:", function_definition.parameters[i], parameter_value)
                function_definition.setValue(function_definition.parameters[i], parameter_value.num)
            
            body = function_definition.body
            values = []
            for i in range(len(body)):
                #print(body[i])
                temp_val = self.view(body[i], function_definition.context)
                if temp_val.error is not None:
                    self.error = temp_val.error
                    break
                values.append(temp_val)
            return values
    
    def view_ArrayNode(self, node, parent_context):
        values = {}
        for i in node.keys:
            #print("Val:", i, node.nodes[i])
            #val = self.view(i, parent_context)
            key = self.view(i, parent_context)
            val = self.view(node.nodes[i], parent_context)
            if val.error is not None:
                return val
            elif key.error is not None:
                return key
            
            values[key] = val
        return ArrayValue(values, parent_context, node.nodes[node.keys[0]])