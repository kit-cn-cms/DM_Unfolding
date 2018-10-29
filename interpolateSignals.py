import ROOT as r
import os

def GetMasses(string):
    string_ = string
    print string_
    index_l = string_.find("Mphi")
    index_m = string_.find("Mchi")
    index_r = string_.find("gSM")
    mphi = string_[index_l:index_m-1]
    mphi = mphi.replace("Mphi-","")
    mchi = string_[index_m:index_r-1] 
    mchi = mchi.replace("Mchi-","")
    return [float(mphi),float(mchi)]

def getSamplenames(SampleName="Axial", SamplePath="/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging"):
	nameList=[]
	for x in os.listdir(SamplePath):
		if x.startswith(SampleName):
			nameList.append(x)
	# print axialsamples
	return nameList

def createAdditionalMassPoints(histolist=[], additionalHistName=""):
	Nbins=histolist[0].GetNbinsX()+1
	graphs=[r.TGraph2D() for i in range(Nbins)]
	# print graphs

	for Bin in range(Nbins):
		counter = 0
		for h in histolist:
			h.Print()
			masses = GetMasses(h.GetName())
			print masses
			graphs[Bin].SetPoint(counter,masses[0],masses[1],h.GetBinContent(Bin))
			graphs[Bin].SetNpx(500)
			graphs[Bin].SetNpy(500)
			print Bin,h.GetBinContent(Bin)
			counter+=1
	c=r.TCanvas()
	c.cd()
	c.SetLogz()
	c.SetTheta(90);
	c.SetPhi(0);
	for i,gr in enumerate(graphs):
		# gr.GetHistogram().Draw("tri1z")
		gr.Draw("tri1z")
		c.SaveAs("pdfs/SignalGraphs/AxialBin"+str(i)+".pdf")
		c.SaveAs("pngs/SignalGraphs/AxialBin"+str(i)+".png")

	c.SetLogy()

	massPoints=[]
	for medMass in range(100,2000,100):
		for dmMass in range(0,600,50):
			h = histolist[0].Clone()
			h.Reset()
			if additionalHistName == "":
				name="Axial_Mphi-"+str(medMass)+"_Mchi-"+str(dmMass)+"_gSM"+additionalHistName
			else:
				name="Axial_Mphi-"+str(medMass)+"_Mchi-"+str(dmMass)+"_gSM"+"_"+additionalHistName
			h.SetName(name)
			for i,BinGraph in enumerate(graphs):
				h.SetBinContent(i,BinGraph.Interpolate(medMass,dmMass))
			h.Sumw2()
			h.Draw("histe")
			if h.Integral()==0:
				continue
			massPoints.append(name)
			h.Write()
			c.SaveAs("pngs/SignalGraphs/"+name+".png")
			c.SaveAs("pdfs/SignalGraphs/"+name+".pdf")

	if additionalHistName=="":
		with open('axial_masspoints.txt', 'w') as f:
			for point in massPoints:
				f.write("%s\n" % point)


signalsFile = r.TFile("rootfiles/signals.root","OPEN")
r.gROOT.SetBatch(True)
r.gStyle.SetPalette(1);


SamplePath = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging"
axialsamples = []
vectorsamples = []
pseudosamples = []
scalarsamples = []

listnames = ["Axial", "Vector", "Pseudo", "Scalar"]
lists = [axialsamples, vectorsamples, pseudosamples, scalarsamples]


axialsamplesNames = getSamplenames(SampleName="Axial", SamplePath="/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging")

def createHistolist(sampleNames=[], additionalString="Weight_scale_variation_muRUp"):
	histolist = []
	r.TH1.AddDirectory(0)
	UpDownvar = ["Up","Down"]
	# additionalSystematics.append("")
	for name in sampleNames:
		if additionalString=="":
			histoname = name
			histolist.append(signalsFile.Get(histoname))
		else:
			histoname = name +"_"+additionalString
			histolist.append(signalsFile.Get(histoname))
	# print histolist
	print histoname
	return histolist

additionalSystematics=[ "",
						"Weight_scale_variation_muRUp",
						"Weight_scale_variation_muRDown",
						"Weight_scale_variation_muFUp",
						"Weight_scale_variation_muFDown"]

axialFile=r.TFile("rootfiles/AxialSignals.root","RECREATE")

for sys in additionalSystematics:
	histolist=createHistolist(sampleNames=axialsamplesNames, additionalString=sys)
	createAdditionalMassPoints(histolist=histolist, additionalHistName=sys)
