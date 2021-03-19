class Context:
    def __init__(self, filename, position = None, parent = None):
        self.filename = filename
        self.position = position
        self.parent = parent
        self.var_table = VarTable()
    
    def __str__(self):
        return f'In {self.filename}, line {self.position.line_no}: col {self.position.col_no}\n'

class VarTable:
    def __init__(self, parent = None):
        self.vars = dict()
        self.parent = None
    
    def setValue(self, name, value):
        self.vars[name] = value
    
    def get(self, name):
        val = self.vars.get(name, None)
        if val is None and self.parent is not None:
            return self.parent.get(name)
        return val
    
    def __repr__(self):
        return f'{self.vars}'