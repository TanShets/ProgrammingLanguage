from Parser import *

class Context:
    def __init__(self, filename, position = None, parent = None):
        self.filename = filename
        self.position = position
        self.parent = parent
        self.functions = dict()
        if parent is None:
            self.var_table = VarTable()
        else:
            self.var_table = VarTable(parent.var_table)
    
    def __str__(self):
        return f'In {self.filename}, line {self.position.line_no}: col {self.position.col_no}\n'
    
    def setFunction(self, no_of_parameters, function_definition):
        if (function_definition.name, no_of_parameters) in list(self.functions.keys()):
            return FunctionDefinitionError(
                "Function of given name and parameters already defined", 
                function_definition.token.pos_start
            )
        
        self.functions[(function_definition.name.val, no_of_parameters)] = Function(
                                                                        self, function_definition.body, 
                                                                        function_definition.parameters,
                                                                        function_definition.token.pos_start
                                                                    )
        #print(self.functions)
        return True
    
    def get(self, no_of_parameters, function_name):
        try:
            if self.functions.get((function_name.val, no_of_parameters)) is None:
                if self.parent_context is not None:
                    return self.parent_context.get(no_of_parameters, function_name)
                else:
                    return None
            else:
                return self.functions.get((function_name.val, no_of_parameters)).copy()
        except:
            return None

class VarTable:
    def __init__(self, parent = None):
        self.vars = dict()
        self.parent = parent
    
    def setValue(self, name, value):
        self.vars[name] = value
    
    def get(self, name):
        val = self.vars.get(name, None)
        if val is None and self.parent is not None:
            return self.parent.get(name)
        return val
    
    def __repr__(self):
        return f'{self.vars}'

class Function:
    def __init__(self, parent_context, body, parameters, position = None):
        self.position = position
        self.context = Context(parent_context.filename, self.position, parent_context)
        self.parameters = parameters
        for i in self.parameters:
            self.context.var_table.setValue(i.token.val, None)
        self.body = body
    
    def copy(self):
        return Function(self.context.parent, self.body, self.parameters, self.position)
    
    def setValue(self, name, value):
        self.context.var_table.setValue(name.token.val, value)
        #print(self.context.var_table)