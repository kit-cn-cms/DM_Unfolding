import json

class VariableContainer():
	def __init__(self, file):
		with open(file) as f:
			self.variables = json.load(f) 

