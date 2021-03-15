from Rocivolt import *

while True:
	cmd = input("Rocivolt> ")

	if cmd.lower() in ['exit', 'quit']:
		break
	
	result, error = run("<stdin>", cmd)
	if error is not None:
		print(error)
	else:
		print(result, end = '\n\n')