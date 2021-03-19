from Rocivolt import *

context = Context("<stdin>")
while True:
	cmd = input("Rocivolt> ")

	if cmd.lower() in ['exit', 'quit']:
		break
	result, error = run("<stdin>", cmd, context)
	if error is not None:
		print(error)
	else:
		print(result, end = '\n\n')
	
	#print(context.var_table)