import ROOT
import sys
import os

def getCanvas():
    c = ROOT.TCanvas("c", "c", 1024, 1024)
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


def drawshifts(file, process="unfolded", variable="Gen_Hadr_Recoil_Pt", syst="CMS_scale_j", addName="", xlabel="unfolded Recoil #vec{U} [Gev/c]"):
    c = getCanvas()
    c.cd(1)
    ROOT.gPad.SetLogy()
    nom = file.Get(process+"_"+variable)
    print process+"_"+variable+"_"+syst+"Up"
    up = file.Get(process+"_"+variable+"_"+syst+"Up")
    down = file.Get(process+"_"+variable+"_"+syst+"Down")

    nom.SetMarkerSize(1.3)
    nom.SetMarkerStyle(20)

    nom.Draw("PE0")
    up.Print()
    down.Print()
    # for i in range(up.GetNbinsX()):
    # print "bin ", i, " up: ", up.GetBinContent(i)-nom.GetBinContent(i)," down: ", down.GetBinContent(i)-nom.GetBinContent(i)

    nom.SetFillColor(1)
    nom.SetTitle("CMS private work")
    nom.GetXaxis().SetTitle(xlabel)
    # nom.GetXaxis().SetTitleSize(0.05)
    # nom.GetXaxis().SetTitleOffset(0.8)
    nom.GetYaxis().SetTitleSize(0.03)
    nom.GetYaxis().SetTitleOffset(0.8)

    up.Draw("samehist")
    up.SetFillColor(0)
    up.SetLineColor(2)
    down.Draw("samehist")
    down.SetFillColor(0)
    down.SetLineColor(4)

    legend = ROOT.TLegend(0.4, 0.6, 0.8, 0.8)
    legend.AddEntry(nom, "nominal", "P")
    legend.AddEntry(up, syst + " up", "l")
    legend.AddEntry(down, syst + " down", "l")
    legend.Draw()
    legend.SetBorderSize(0)
    legend.SetLineStyle(0)

    c.cd(2)
    ratioUp = nom.Clone()
    ratioUp.Divide(up)
    ratioUp.SetLineColor(up.GetLineColor())
    ratioUp.Draw("E0")
    ratioUp.GetYaxis().SetTitle("#frac{nominal}{variation}")
    ratioUp.GetYaxis().CenterTitle()
    ratioUp.GetYaxis().SetRangeUser(0.45, 1.65)
    ratioUp.GetXaxis().SetLabelSize(nom.GetXaxis().GetLabelSize() * 3.5)
    ratioUp.GetYaxis().SetLabelSize(nom.GetYaxis().GetLabelSize() * 3.5)
    ratioUp.GetXaxis().SetTitleSize(nom.GetXaxis().GetTitleSize() * 4.5)
    ratioUp.GetYaxis().SetTitleSize(nom.GetYaxis().GetTitleSize() * 1.7)
    ratioUp.GetYaxis().SetTitleOffset(0.8)
    ratioUp.SetTitle("")
    ratioUp.GetYaxis().SetNdivisions(505)

    ratioDown = nom.Clone()
    ratioDown.Divide(down)
    ratioDown.SetLineColor(down.GetLineColor())
    ratioDown.Draw("E0same")
    # ratioDown.SetTitle("");
    # ratioDown.GetYaxis().SetTitle("nominal/variation");

    c.Update()
    lineratio = ROOT.TLine(c.cd(2).GetUxmin(), 1.0, c.cd(2).GetUxmax(), 1.0)
    lineratio.SetLineColor(ROOT.kBlack)
    lineratio.Draw()
    c.Update()

    c.SaveAs("pdfs/shifts/shifts_"+syst+addName+".pdf")
    c.SaveAs("pngs/shifts/shifts_"+syst+addName+".png")


file = ROOT.TFile.Open("rootfiles/data.root")
MCfile = ROOT.TFile.Open("rootfiles/MCdata.root")
# file.ls()

nom = file.Get("unfolded_Gen_Hadr_Recoil_Pt")
MCDatanom = MCfile.Get("unfolded_Gen_Hadr_Recoil_Pt")
unfoldedsysts = [
    "CMS_scale_j",
    "CMS_res_j",
    "Weight_PU",
    "CMS_btag_lf",
    "CMS_btag_hf",
    "CMS_btag_hfstats1",
    "CMS_btag_lfstats1",
    "CMS_btag_hfstats2",
    "CMS_btag_lfstats2",
    "CMS_btag_cferr1",
    "CMS_btag_cferr2"
]


for sys in unfoldedsysts:
    drawshifts(file=file, process="unfolded",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="realData")
    drawshifts(file=MCfile, process="unfolded",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="MCData")


BKGfile = ROOT.TFile.Open("rootfiles/histos_normedmuRmuF.root")
signalsFile = ROOT.TFile.Open("rootfiles/signals.root")

madgraphmuRmuR = [
    "madgraph_scale_variation_muR",
    "madgraph_scale_variation_muF",
]

powhegmuRmuR = [
    "powheg_scale_variation_muR",
    "powheg_scale_variation_muF",
]

signalmuRmuR = [
    "signal_scale_variation_muR",
    "signal_scale_variation_muF",
]

WZSystematicsmuRmuR = [
    "Weight_scale_variation_muR",
    "Weight_scale_variation_muF",
    "BosonWeight_QCD1",
    "BosonWeight_QCD2",
    "BosonWeight_QCD3",
    "BosonWeight_EW1",
    "BosonWeight_EW2",
    "BosonWeight_EW3",
    "BosonWeight_Alpha",
    "BosonWeight_Mixed",
]

additionalsysts = [
    "Weight_PDF",

]    

defaultmuRmuR = [
    "Weight_scale_variation_muR",
    "Weight_scale_variation_muF",
]

for sys in WZSystematicsmuRmuR:
    drawshifts(file=BKGfile, process="z_nunu_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="z_nunu_jets", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=BKGfile, process="z_nunu_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="z_nunu_jets", xlabel="Gen Recoil #vec{U} [Gev/c]")

for sys in additionalsysts + powhegmuRmuR + defaultmuRmuR:
    drawshifts(file=BKGfile, process="singletop",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="singletop", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=BKGfile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="ttbar", xlabel="Gen Recoil #vec{U} [Gev/c]")


for sys in additionalsysts + madgraphmuRmuR + defaultmuRmuR:
    drawshifts(file=BKGfile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="qcd", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=BKGfile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="gamma_jets", xlabel="Gen Recoil #vec{U} [Gev/c]")

axialsamples = []
vectorsamples = []
pseudosamples = []
scalarsamples = []

listnames = ["Axial", "Vector", "Pseudo", "Scalar"]
lists = [axialsamples, vectorsamples, pseudosamples, scalarsamples]
signalSamplepath = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging"

for x in os.listdir(signalSamplepath):
    for i,listname in enumerate(listnames):
        if x.startswith(listname):
            lists[i].append(x)
            for sys in signalmuRmuR:
                drawshifts(file=signalsFile, process=x,
                variable="Gen_Hadr_Recoil_Pt", syst=sys, addName=x, xlabel="Gen Recoil #vec{U} [Gev/c]")


noSelectionmuRmuR = [
    "muR",
    "muF",
]

noselectionFile = ROOT.TFile.Open("rootfiles/muRmuFnormalization.root")

for sys in noSelectionmuRmuR:
    drawshifts(file=noselectionFile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="qcd_noSelection", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=noselectionFile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="qcd_noSelection_scaled", xlabel="Gen Recoil #vec{U} [Gev/c]")
    
    drawshifts(file=noselectionFile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="ttbar_noSelection", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=noselectionFile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="ttbar_noSelection_scaled", xlabel="Gen Recoil #vec{U} [Gev/c]")

    drawshifts(file=noselectionFile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="gamma_jets_noSelection", xlabel="Gen Recoil #vec{U} [Gev/c]")
    drawshifts(file=noselectionFile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="gamma_jets_noSelection_scaled", xlabel="Gen Recoil #vec{U} [Gev/c]")

# raw_input()
