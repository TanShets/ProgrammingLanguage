from Rocivolt import *
import sys

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Enter a filename!!!!")
        sys.exit(0)
    filename = sys.argv[1]
    #print(filename)
    #filename = input("Enter the file name: ")
    if filename[-5:] != '.rocv':
        print("Error: Incorrect file format!!!")
    else:
        fp = open(filename, 'r')
        parent_context = Context(filename)
        line = ''.join(fp.readlines())
        result, error = run(filename, line, parent_context)
        #print(line)
        if error is not None:
            print(error)
        #print(result)