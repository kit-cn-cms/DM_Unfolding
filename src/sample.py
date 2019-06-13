import ROOT as r
import glob
import json
import os

class Sample():
	def __init__(self, name = "Samplename", path = "/path/to/sample", LateX = "", color = r.kBlack, extSysts = [], systs = [], selection =""):
		self.path = path
		self.color = r.kBlack
		self.systematics = []
		self.name = name
		if LateX != "":
			self.LateX = self.name
		else:
			self.LateX = self.name
		self.setColor(color)
		self.setSystematics(systs, extSysts)
		self.histos = dict()
		self.filenames= dict()
		self.getFiles()
		self.selection = selection

	def setColor(self, color):
		self.color = color

	def setSystematics(self, systs, extSysts): #give list of systematics
		self.systematics = systs
		self.extSystematics = extSysts

	def getFiles(self):
		print self.path+"*"+self.name+"/*nominal*.root"
		nominalnames = glob.glob(self.path+"*"+self.name+"*/*nominal*.root")
		self.filenames["nominal"]=nominalnames

		for sys in self.extSystematics:
			files = glob.glob(self.path+"*"+self.name+"*/*"+sys+"*.root")
			self.filenames[sys]=files
	
		if os.path.isfile("files.json"):
			with open("files.json") as f:
				dic = json.load(f)
			# if not dic[self.name]: 			
			if not self.name in dic: 			
				with open("files.json","a") as f:
					json.dump({self.name: self.filenames},f, sort_keys=True, indent=4)
		else:
			with open("files.json","w") as f:
				json.dump({self.name: self.filenames},f, sort_keys=True, indent=4)
