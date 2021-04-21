from Rocivolt import *

def print_list_result(result):
	lists = [[result, 0]]
	while len(lists) > 0:
		current_list, index = lists[-1][0], lists[-1][1]
		hasTraversed = False
		for i in range(index, len(current_list)):
			if type(current_list[i]) is list:
				lists[-1][1] = i + 1
				lists.append([current_list[i], 0])
				break
			else:
				if i == len(current_list) - 1:
					hasTraversed = True
				print(current_list[i])

		if hasTraversed or index == len(current_list):
			lists.pop()

context = Context("<stdin>")
while True:
	cmd = input("Rocivolt> ")

	if cmd.lower() in ['exit', 'quit']:
		break
	result, error = run("<stdin>", cmd, context)
	if error is not None:
		print(error)
	else:
		if type(result) is list:
			print_list_result(result)
		else:
			print(result, end = '\n\n')
	
	#print(context.var_table)