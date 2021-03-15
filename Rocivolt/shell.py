from Rocivolt import *

while True:
	cmd = input("Rocivolt> ")

	if cmd.lower() in ['exit', 'quit']:
		break
	context = Context("<stdin>")
	result, error = run("<stdin>", cmd, context)
	if error is not None:
		print(error)
	else:
		print(result, end = '\n\n')