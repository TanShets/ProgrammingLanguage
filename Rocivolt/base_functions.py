from symbols import *
from Error import *
from Lexer import *
from Parser import *
from Values import *

class BaseFunction:
    def do_print(arguments, parent_context = None):
        for i in arguments:
            print(i, end = ' ')
        print()
        return Value(
            Token(TT_NULL, val = 'null'), 
            parent_context
        )
    
    def do_input(statement = None, parent_context = None):
        #print(statement)
        if type(statement) is list and len(statement) == 1:
            if type(statement[0]) is StringValue or (type(statement[0]) is Value and type(statement[0].num) is str):
                print(statement[0], end = '')
            else:
                return ErrorNode(InvalidSyntaxError(
                    "Expected String", statement[0].pos
                ))
        elif type(statement) is list and len(statement) > 1:
            return ErrorNode(InvalidSyntaxError(
                "Expected String", statement[1].pos
            ))
        temp_val = input()
        if type(statement) is list and len(statement) == 1:
            pos = statement[0].pos
        else:
            pos = None
        temp_token = Token(TT_STRING, val = temp_val, pos_start = pos)
        return StringValue(temp_token, parent_context)
    
    def convert_to_int(parameter, parent_context = None):
        if type(parameter) is list and len(parameter) == 1:
            if type(parameter[0]) is StringValue or (type(parameter[0]) is Value and type(parameter[0].num) is str):
                isFloatable = False
                try:
                    value_x = float(parameter[0].num)
                    isFloatable = True
                except:
                    value_x = None
                
                if parameter[0].isNull:
                    value_x = 0
                
                if parameter[0].num.isnumeric() or isFloatable:
                    if parameter[0].num.isnumeric():
                        new_val = int(parameter[0].num)
                    else:
                        new_val = int(value_x)

                    new_token = Token(
                        TT_INT, val = new_val, pos_start = parameter[0].pos
                    )
                    return Value(new_token, parent_context)
                else:
                    return ErrorNode(InvalidStringError(
                        "Expected numerical value", parameter[0].pos
                    ))
            elif type(parameter[0]) is Value and type(parameter[0].num) in [int, float]:
                new_token = Token(
                    TT_INT, val = int(parameter[0].num), pos_start = parameter[0].pos
                )
                return Value(new_token, parent_context)
            else:
                return ErrorNode(InvalidTypeError(
                    "Expected a string, int or float value", parameter[0].pos
                ))
        else:
            if type(parameter) is not list or len(parameter) == 0:
                pos = None
            else:
                pos = parameter[1].pos
            return ErrorNode(InvalidSyntaxError(
                "Expected a single value", pos
            ))
    
    def convert_to_float(parameter, parent_context = None):
        if type(parameter) is list and len(parameter) == 1:
            if type(parameter[0]) is StringValue or (type(parameter[0]) is Value and type(parameter[0].num) is str):
                isFloatable = False
                try:
                    value_x = float(parameter[0].num)
                    isFloatable = True
                except:
                    value_x = None
                
                if parameter[0].isNull:
                    value_x = float(0)
                
                if isFloatable:
                    new_val = value_x

                    new_token = Token(
                        TT_INT, val = new_val, pos_start = parameter[0].pos
                    )
                    return Value(new_token, parent_context)
                else:
                    return ErrorNode(InvalidStringError(
                        "Expected numerical value", parameter[0].pos
                    ))
            elif type(parameter[0]) is Value and type(parameter[0].num) in [int, float]:
                new_token = Token(
                    TT_FLOAT, val = float(parameter[0].num), pos_start = parameter[0].pos
                )
                return Value(new_token, parent_context)
            else:
                return ErrorNode(InvalidTypeError(
                    "Expected a string, int or float value", parameter[0].pos
                ))
        else:
            if type(parameter) is not list or len(parameter) == 0:
                pos = None
            else:
                pos = parameter[1].pos
            return ErrorNode(InvalidSyntaxError(
                "Expected a single value", pos
            ))
    
    def convert_to_string(parameter, parent_context = None):
        if type(parameter) is list and len(parameter) == 1:
            if type(parameter[0]) is StringValue:
                return parameter[0]
            elif type(parameter) is Value and type(parameter.num) in [int, float, str]:
                new_token = Token(TT_STRING, val = str(node.token.val), pos_start = parameter[0].pos)
                return StringValue(new_token, parent_context)
            else:
                new_token = Token(TT_STRING, val = parameter[0].__str__(), pos_start = parameter[0].pos)
                return StringValue(new_token, parent_context)
        else:
            if type(parameter) is not list or len(parameter) == 0:
                pos = None
            else:
                pos = parameter[1].pos
            return ErrorNode(InvalidSyntaxError(
                "Expected single object", pos
            ))