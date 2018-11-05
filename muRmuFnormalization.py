import ROOT as r
import os
import array
import subprocess

def getCanvas():
    c = r.TCanvas("c", "c", 1024, 1024)
    c.Divide(1, 2)
    c.cd(1).SetPad(0., 0.3, 1.0, 1.0)
    c.cd(1).SetBottomMargin(0.0)
    c.cd(2).SetPad(0., 0.0, 1.0, 0.3)
    c.cd(2).SetTopMargin(0.0)
    c.cd(1).SetTopMargin(0.07)
    c.cd(2).SetBottomMargin(0.4)
    c.cd(1).SetRightMargin(0.05)
    c.cd(1).SetLeftMargin(0.15)
    c.cd(2).SetRightMargin(0.05)
    c.cd(2).SetLeftMargin(0.15)
    c.cd(2).SetTicks(1, 1)
    c.cd(1).SetTicks(1, 1)
    return c


def createChain(Bins, commonpath="/nfs/dust/cms/user/swieland/Darkmatter/ntUples_tagging"):
    chain = r.TChain("MVATree")
    for Bin in Bins:
        print "adding ",  commonpath+"/"+Bin+"/*nominal*.root", " to chain"
        chain.Add(commonpath+"/"+Bin+"/*nominal*.root")
    print "having", chain.GetEntries(), " events in chain "
    return chain


def getSignalXS(samplename):
    XS_tree = r.TTree("XS", "XS")
    XS_tree.ReadFile("Madgraph_Signal_XS.csv", "sample/C:xs/F")

    sampleVar = array.array("c", "x" * (len(samplename)))
    xsVar = array.array("f", [0.0])
    # xsVar = 0.
    XS_tree.SetBranchAddress("sample", sampleVar)
    XS_tree.SetBranchAddress("xs", xsVar)
    entries = XS_tree.GetEntries()
    for i in range(entries):
        XS_tree.GetEntry(i)
        # print ''.join(sampleVar)
        # print sampleVar.tostring()
        samplestring = ''.join(sampleVar)
        if samplestring == samplename:
            signalXS = xsVar[0];
            print samplestring, " has XS ", signalXS
            continue
    return signalXS

def calcNormFactor(chain, variableName="Gen_Hadr_Recoil_Pt", samplename="qcd", isSignal=False):
    if isSignal:
        XSWeight = getSignalXS(samplename)


    Frame = r.ROOT.RDataFrame(chain).Define("commonweight", "Weight_XS*Weight_GEN_nom")\
                                    .Define("muRUpWeight", "Weight_scale_variation_muR_2p0_muF_1p0*commonweight")\
                                    .Define("muRDownWeight", "Weight_scale_variation_muR_0p5_muF_1p0*commonweight")\
                                    .Define("muFUpWeight", "Weight_scale_variation_muR_1p0_muF_2p0*commonweight")\
                                    .Define("muFDownWeight", "Weight_scale_variation_muR_1p0_muF_0p5*commonweight")

    print "created DataFrame for ", samplename

    outputfile = r.TFile("rootfiles/muRmuFnormalization.root", "UPDATE")

    c = getCanvas()
    c.cd()
    r.gPad.SetLogy()

    h_nominal = Frame.Histo1D((samplename+"_"+variableName, samplename+"_nominal", 300, 0, 3000), variableName,"commonweight")
    h_MuRUp = Frame.Histo1D((samplename+"_"+variableName+"_muRUp", samplename+"_muRUp", 300, 0, 3000), variableName, "muRUpWeight")
    h_MuRDown = Frame.Histo1D((samplename+"_"+variableName+"_muRDown", samplename+"_muRDown", 300, 0, 3000), variableName, "muRDownWeight")
    h_MuFUp = Frame.Histo1D((samplename+"_"+variableName+"_muFUp", samplename +"_muFUp", 300, 0, 3000), variableName, "muFUpWeight")
    h_MuFDown = Frame.Histo1D((samplename+"_"+variableName+"_muFDown", samplename+"_muFDown", 300, 0, 3000), variableName, "muFDownWeight")

    h_nominal.Sumw2()
    h_MuRUp.Sumw2()
    h_MuRDown.Sumw2()
    h_MuFUp.Sumw2()
    h_MuFDown.Sumw2()

    h_nominal_scaled = h_nominal.Clone()
    h_nominal_scaled.SetName(samplename+"_"+variableName+"_scaled")
    h_nominal_scaled.Write()

    h_MuRUp_scaled = h_MuRUp.Clone()
    h_MuRUp_scaled.SetName(samplename+"_"+variableName+"_scaled_muRUp")
    
    h_MuRDown_scaled = h_MuRDown.Clone()
    h_MuRDown_scaled.SetName(samplename+"_"+variableName+"_scaled_muRDown")
    
    h_MuFUp_scaled = h_MuFUp.Clone()
    h_MuFUp_scaled.SetName(samplename+"_"+variableName+"_scaled_muFUp")

    h_MuFDown_scaled = h_MuFDown.Clone()
    h_MuFDown_scaled.SetName(samplename+"_"+variableName+"_scaled_muFDown")

    if isSignal:
        h_nominal.Scale(XSWeight)
        h_MuRUp.Scale(XSWeight)
        h_MuRDown.Scale(XSWeight)
        h_MuFUp.Scale(XSWeight)
        h_MuFDown.Scale(XSWeight)


    h_nominal.Draw()
    h_nominal.Write()
    int_nom = h_nominal.Integral()
    print "wrote nominal, integral=", int_nom

    h_MuRUp.Draw()
    h_MuRUp.Write()
    int_muRUp = h_MuRUp.Integral()
    print "wrote muRUp, integral=", int_muRUp
    muRUpWeight = float(int_nom/int_muRUp)
    print "muRUpWeight = ", muRUpWeight
    h_MuRUp_scaled.Scale(muRUpWeight)
    h_MuRUp_scaled.Write()
    print "wrote scaled muRUp, integral=", h_MuRUp_scaled.Integral()

    h_MuRDown.Draw()
    h_MuRDown.Write()
    int_muRDown = h_MuRDown.Integral()
    print "wrote muRDown, integral=", int_muRDown
    muRDownWeight =float(int_nom/int_muRDown)
    print "muRDownWeight = ", muRDownWeight
    h_MuRDown_scaled.Scale(muRDownWeight)
    h_MuRDown_scaled.Write()
    print "wrote scaled muRDown, integral=", h_MuRDown_scaled.Integral()

    h_MuFUp.Draw()
    h_MuFUp.Write()
    int_muFUp = h_MuFUp.Integral()
    print "wrote muFUp integral=", int_muFUp
    muFUpWeight = float(int_nom/int_muFUp)
    print "muFUpWeight = ", muFUpWeight
    h_MuFUp_scaled.Scale(muFUpWeight)
    h_MuFUp_scaled.Write()
    print "wrote scaled muFUp, integral=", h_MuFUp_scaled.Integral()


    h_MuFDown.Draw()
    h_MuFDown.Write()
    int_muFDown = h_MuFDown.Integral()
    print "wrote muFDown integral=", int_muFDown
    muFDownWeight = float(int_nom/int_muFDown)
    print "muFDownWeight = ", muFDownWeight
    h_MuFDown_scaled.Scale(muFDownWeight)
    h_MuFDown_scaled.Write()
    print "wrote scaled muFDown, integral=", h_MuFDown_scaled.Integral()

    outputfile.Close()

    weights = {"muRUp": muRUpWeight, "muRDown": muRDownWeight,
               "muFUp": muFUpWeight, "muFDown": muFDownWeight}
    return weights


def scaleHistos(weights, processname="qcd", variableName="Gen_Hadr_Recoil_Pt", generatorLabel="madgraph", isSignal=False):
    print "scaling existing histos"
    # histofile = r.TFile("rootfiles/data_normedmuRmuF.root","UPDATE")
    if not isSignal:
        histofile = r.TFile("rootfiles/data_normedmuRmuF.root","UPDATE")
    else:
        histofile = r.TFile("rootfiles/signals_normedmuRmuF.root","UPDATE")

    # also save normalized templates to collection of all histos 
    histosfile = r.TFile("rootfiles/histos_normedmuRmuF.root","UPDATE")

    defaultweightnames = [
        "Weight_scale_variation_muRUp",
        "Weight_scale_variation_muRDown",
        "Weight_scale_variation_muFUp",
        "Weight_scale_variation_muRDown",
    ]

    nominalName = processname+"_"+variableName

    nominal = histofile.Get(nominalName)
    print "nominal integal= ", nominal.Integral()
    nominal.Write()

    scaledHistoList = []

    for key in weights.keys():
        # r.gDirectory.Delete(processname+"_"+variableName+"_"+generatorLabel+"_scale_variation_"+key) #delete if histo already exists
        h = histofile.Get(nominalName+"_"+"Weight_scale_variation_"+key)
        h_clone = h.Clone()
        h_clone.Scale(weights[key])
        h_clone.SetName(nominalName+"_"+generatorLabel+"_scale_variation_"+key)
        print key," integal= ", h_clone.Integral()
        histofile.WriteTObject(h_clone)
        histosfile.WriteTObject(h_clone)
    histofile.Close()
    histosfile.Close()


#reset file to store histos without selections
subprocess.call("rm -f /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/muRmuFnormalization.root", shell=True)

#save and reset file with normalized histos used for limits
subprocess.call("rm -f /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/data_normedmuRmuF.root", shell=True)
subprocess.call("cp /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/data.root /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/data_normedmuRmuF.root ", shell=True)

#save and reset file with normalized histos used for controlplots (collection of all histos)
subprocess.call("rm -f /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/histos_normedmuRmuF.root", shell=True)
subprocess.call("cp /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/histos.root /nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/histos_normedmuRmuF.root ", shell=True)

r.ROOT.EnableImplicitMT()


path = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_tagging"

# qcd
qcdBins = [
    "QCD_HT50to100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
    "QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"

]

qcdChain = createChain(Bins=qcdBins, commonpath=path)
qcdweights = calcNormFactor(chain=qcdChain, samplename="qcd")
scaleHistos(processname="qcd", variableName="Gen_Hadr_Recoil_Pt", weights=qcdweights, generatorLabel="madgraph")

# gamma+jets
GJetsBins=[
	"GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
	"GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
	"GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
	"GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"
]

GJetsChain = createChain(Bins=GJetsBins, commonpath=path)
GJetsweights = calcNormFactor(chain=GJetsChain, samplename="gamma_jets")
scaleHistos(processname="gamma_jets", variableName="Gen_Hadr_Recoil_Pt", weights=GJetsweights, generatorLabel="madgraph")

# diboson
# dibosonBins=[
# 	"WW_TuneCUETP8M1_13TeV-pythia8",
# 	"WZ_TuneCUETP8M1_13TeV-pythia8",
# 	"ZZ_TuneCUETP8M1_13TeV-pythia8"
# ]

# dibosonChain = createChain(Bins=dibosonBins, commonpath=path)
# dibosonweights = calcNormFactor(chain=dibosonChain, samplename="diboson")
# scaleHistos(processname="diboson", variableName="Gen_Hadr_Recoil_Pt", weights=dibosonweights, generatorLabel="pythia8")

# single top
singletopBins=[
	"ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8",
	"ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin",
	"ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin",
	# "ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4",
	# "ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4",
]

singletopChain = createChain(Bins=singletopBins, commonpath=path)
singletopweights = calcNormFactor(chain=singletopChain, samplename="singletop")
scaleHistos(processname="singletop", variableName="Gen_Hadr_Recoil_Pt", weights=singletopweights, generatorLabel="powheg")

# ttbar
ttbarBins=[
	"TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"
]

ttbarChain = createChain(Bins=ttbarBins, commonpath=path)
ttbarweights = calcNormFactor(chain=ttbarChain, samplename="ttbar")
scaleHistos(processname="ttbar", variableName="Gen_Hadr_Recoil_Pt", weights=ttbarweights, generatorLabel="powheg")

# # signals
# signalSamplepath = "/nfs/dust/cms/user/swieland/Darkmatter/ntUples_signal_madgraph_tagging"

# listnames = ["Axial", "Vector", "Pseudo", "Scalar"]
# lists = [axialsamples, vectorsamples, pseudosamples, scalarsamples]

# for x in os.listdir(signalSamplepath):
#     for i,listname in enumerate(listnames):
#         if x.startswith(listname):
#             lists[i].append(x)
#             print x
