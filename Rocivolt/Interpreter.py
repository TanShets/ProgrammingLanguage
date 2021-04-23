from Parser import *
from symbols import *
from Lexer import *
from Error import *
from Context import *
from Values import *
from base_functions import *
import importlib

class Interpreter:
    def __init__(self, parse_result, parent_context):
        self.num = Value()
        self.parse_result = parse_result
        self.error = None

        self.isFunction = 0
        self.isLoop = 0
        self.isReturn = False
        self.isBreak = False

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
    
    def view_NullNode(self, node, parent_context):
        return Value(node.token, parent_context)
    
    def view_NumNode(self, node, parent_context):
        return Value(node.token, parent_context)
        #return "Found NumNode"
    
    def view_ErrorNode(self, node, parent_context):
        self.error = node.token
        return Value()
    
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
        #print("left", left)
        if left.error is not None:
            self.error = left.error
            return Value()
        right = self.view(node.right_token, parent_context)
        #print("Right", right)
        #print(left, node.token.type, right)
        #print(left.num, left.error, type(left).__name__)

        if left.error is None and right.error is None:
            # print(type(left).__name__, type(right).__name__)
            # print(left, right)
            # print(node.token)
            left.operation(right, node.token)
            if left.error is not None:
                self.error = left.error
        elif right.error is not None:
            self.error = right.error
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
            variable_value = parent_context.var_table.get(node.var_token.val).copy()
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
            
            if variable_value.error is None:
                parent_context.var_table.setValue(node.var_token.val, variable_value)
            else:
                return variable_value

        return Value()
    
    def view_BooleanNode(self, node, parent_context):
        return Value(node.token, parent_context)
    
    def view_ConditionalNode(self, node, parent_context):
        fulfilled = False
        for i in node.blocks:
            condition_value = self.view(i['condition'], parent_context)
            #print(type(i['condition']).__name__)
            if condition_value.error is not None:
                self.error = condition_value.error
                return Value()

            if condition_value.num == 1:
                #print(i['condition'])
                fulfilled = True
                for j in i['body']:
                    result = self.view(j, parent_context)
                    if result.error is not None:
                        self.error = result.error
                        return Value()
                    elif self.isFunction > 0 and self.isReturn:
                        return result
                    elif self.isLoop > 0 and self.isBreak:
                        return Value()
            
            if fulfilled:
                return Value()
        #print("Found you")
        if node.else_block is not None:
            for i in node.else_block:
                result = self.view(i, parent_context)
                #print(result)
                if result.error is not None:
                    self.error = result.error
                    return Value()
                elif self.isFunction > 0 and self.isReturn:
                    return result
                elif self.isLoop > 0 and self.isBreak:
                    return Value()
            return Value()
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
        self.isLoop += 1
        while condition.num not in [0, 'false']:
            for i in node.block:
                #print(i)
                value = self.view(i, parent_context)
                #print(value)
                if self.isReturn:
                    self.isLoop -= 1
                    return value
                elif self.isBreak:
                    self.isLoop -= 1
                    self.isBreak = False
                    return values
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
        return Value()
    
    def view_FunctionDefinitionNode(self, node, parent_context):
        #print(node.body)
        outcome = parent_context.setFunction(len(node.parameters), node)
        if type(outcome) is FunctionDefinitionError:
            self.error = outcome
        return Value()
    
    def view_FunctionCallNode(self, node, parent_context):
        # print(node)
        # print(node.parameters)
        #print(node.parameters)
        #print(self.isLoop, self.isReturn, self.isBreak)
        #print(True, node.name)
        self.isFunction += 1
        function_definition = parent_context.get(len(node.parameters), node.name)
        function_definition.context.parent_context = parent_context
        if function_definition is None:
            self.isFunction -= 1
            self.error = FunctionCallError(f'Function: {node.name} not previously defined', node.token.pos_start)
            return Value()
        else:
            for i in range(len(node.parameters)):
                parameter_value = self.view(node.parameters[i], parent_context)
                #print("Val:", function_definition.parameters[i], parameter_value)
                function_definition.setValue(function_definition.parameters[i], parameter_value)
            
            body = function_definition.body
            #print(body)
            values = []
            for i in range(len(body)):
                #print(body[i])
                temp_val = self.view(body[i], function_definition.context)
                if self.isReturn:
                    self.isReturn = False
                    self.isFunction -= 1
                    return temp_val
                
                if type(temp_val) is Value and temp_val.error is not None:
                    self.error = temp_val.error
                    break
                elif type(temp_val) is list:
                    stop_now = False
                    for j in temp_val:
                        if j.error is not None:
                            stop_now = True
                            break
                    if stop_now:
                        break
                values.append(temp_val)
            
            if not self.isReturn:
                if len(body) > 0:
                    pos_x = body[-1].token.pos_start
                else:
                    pos_x = node.token.pos_start
                self.isFunction -= 1
                return Value(
                        Token(
                            TT_NULL, 'null', pos_start = pos_x
                        )
                    )
    
    def view_InBuiltFunctionCallNode(self, node, parent_context):
        call_name = node.name.val
        method_name = T_INBUILT_FUNCTIONS.get(call_name, None)

        if method_name is None:
            self.error = FunctionCallError(f'function-> \'{call_name}\' not found', node.token.pos_start)
            return Value()
        else:
            #BaseFunction.do_print(["dfdd"])
            #answer_node = method()
            method = getattr(BaseFunction, method_name)
            parameter_vals = []
            for i in node.parameters:
                temp_val = self.view(i, parent_context)
                if self.error is not None:
                    return Value()
                parameter_vals.append(temp_val)
            answer = method(parameter_vals, parent_context = parent_context)
            if type(answer) is not ErrorNode:
                return answer
            else:
                self.error = answer.token
                return Value()

    def view_ReturnNode(self, node, parent_context):
        if self.isFunction > 0:
            #print(type(node.expression).__name__)
            val = self.view(node.expression, parent_context)
            #print(val)
            self.isReturn = True
            return val
        else:
            self.error = InvalidSyntaxError("Unexpected 'return' without overlying function", node.token.pos_start)
            return Value()
    
    def view_BreakNode(self, node, parent_context):
        if self.isLoop == 0:
            self.error = InvalidSyntaxError(
                "Unexpected 'break' without overlying loop", node.token.pos_start
            )
        else:
            self.isBreak = True
        return Value()

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