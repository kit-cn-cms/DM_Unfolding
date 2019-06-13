import ROOT as r
import time 

class HistoMaker():
	def __init__(self,sample, variables, output="test.root"):
		r.ROOT.EnableImplicitMT()
		self.sample = sample
		self.vars = variables.variables
		self.output = output

		self.chains = dict()
		self.dataFrames = dict()
		self.chains["nominal"] =  self.createChain(self.sample.filenames, "nominal")
		for sys in self.sample.extSystematics:
			self.chains[sys] = self.createChain(self.sample.filenames, sys)
		self.createDataFrames()
		# self.createHistos()
		# print self.vars

	def createChain(self,filenames, sys):
		chain = r.TChain("MVATree")
		print "creating chain for sys: ", sys
		for i,file in enumerate(filenames[sys]):
			chain.Add(file)
			if i % 100 == 0:
				print "adding file #", i ,"/", len(filenames[sys])
		# print chain.GetEntries(), "Entries in Sample", self.sample.name, "for Systematic", sys
		return chain

	def createDataFrames(self):
		self.dataFrames["nominal"] = r.ROOT.RDataFrame(self.chains["nominal"])
		for sys in self.sample.extSystematics:
			self.dataFrames[sys] =  r.ROOT.RDataFrame(self.chains[sys])
		print "created DataFrames for Sample", self.sample.name

	def createHistos(self):
		histoFile = r.TFile(self.output,"RECREATE")
		watch=r.TStopwatch()
		watch.Start();
		hists = []
		syshists = dict()
		print "creating templates for", self.sample.name

		#TODO: Weights and systs from weights

		# nominal sample
		filteredNominalFrame = self.dataFrames["nominal"].Filter(self.sample.selection)
		# cols =  filteredNominalFrame.GetColumnNames()
		# for col in cols:
			# print col
		for var in self.vars:
			# h = filteredNominalFrame.Histo1D((self.sample.name+"_"+str(var), str(var), self.vars[var]["nBins"], self.vars[var]["xmin"], self.vars[var]["xmax"]), str(var))
			h = filteredNominalFrame.Define("jet0", "Jet_Pt").Histo1D("jet0");
			hists.append(h)
		self.sample.histos["nominal"] = hists

		# systematic sample
		hists = []
		for sys in self.sample.extSystematics:
			filteredFrame = self.dataFrames[sys].Filter(self.sample.selection)
			for var in self.vars:
				h = filteredFrame.Histo1D((self.sample.name+"_"+str(var)+"_"+str(sys), str(var)+str(sys), self.vars[var]["nBins"], self.vars[var]["xmin"], self.vars[var]["xmax"]), str(var))
				hists.append(h)
			self.sample.histos[sys] = hists


		for key, hists in self.sample.histos.iteritems():
			for h in hists:
				h.Write()
				print "created ", h.GetName(), "to", self.output

		# for sys in self.sample.extSystematics:
		watch.Stop()
		watch.Print()


