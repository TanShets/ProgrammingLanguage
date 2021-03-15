class Context:
    def __init__(self, filename, position = None, parent = None):
        self.filename = filename
        self.position = position
        self.parent = parent
    
    def __str__(self):
        return f'In {self.filename}, line {self.position.line_no}: col {self.position.col_no}\n'