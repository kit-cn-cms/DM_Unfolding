import ROOT
import sys
import os
ROOT.gROOT.SetBatch(True)

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

def GetMasses(string_):
    # print string_
    index_l = string_.find("Mphi")
    index_m = string_.find("Mchi")
    index_r = string_.find("gSM")
    mphi = string_[index_l:index_m-1]
    mphi = mphi.replace("Mphi-","")
    mchi = string_[index_m:index_r-1] 
    mchi = mchi.replace("Mchi-","")
    return [float(mphi),float(mchi)]

def GetCoupling(string_):
    s = string_.split('_MonoJ_')
    if s[0] == 'Axial':
        s[0] = "Axial-Vector"
    return s[0]


def drawshifts(file, process="unfolded", variable="Gen_Hadr_Recoil_Pt", syst="CMS_scale_j", addName="", xlabel="unfolded Recoil |#vec{U}_{T}| [GeV/c]", processlabel=""):
    ROOT.gStyle.SetOptStat(0)
    c = getCanvas()
    c.cd(1)
    ROOT.gPad.SetLogy()
    nom = file.Get(process+"_"+variable)
    print process+"_"+variable+"_"+syst+"Up"
    up = file.Get(process+"_"+variable+"_"+syst+"Up")
    down = file.Get(process+"_"+variable+"_"+syst+"Down")

    nom.SetMarkerSize(1.3)
    nom.SetMarkerStyle(20)
    nom.SetMarkerColor(1)

    nom.SetFillColor(1)
    nom.SetTitle("")
    nom.GetXaxis().SetTitle(xlabel)
    nom.GetXaxis().SetTitleSize(0.04)
    # nom.GetXaxis().SetTitleOffset(0.8)
    nom.GetYaxis().SetTitle("Events")
    nom.GetYaxis().SetTitleSize(0.07)
    nom.GetYaxis().SetTitleOffset(0.6)
    nom.GetYaxis().SetLabelSize(0.04)


    cms = ROOT.TLatex(0.12, 0.95, 'CMS #it{private work}'  )
    cms.SetNDC()
    cms.SetTextSize(0.07)
    ROOT.gStyle.SetPalette(1)

    lumi = ROOT.TLatex(0.7, 0.95, '35.9 fb^{-1} (13 TeV)'  )
    lumi.SetNDC()
    lumi.SetTextSize(0.05)

    processlabel = ROOT.TLatex(0.65, 0.4, processlabel )
    processlabel.SetNDC()
    processlabel.SetTextSize(0.07)

    nom.Draw("PE0")
    cms.Draw()
    lumi.Draw()
    processlabel.Draw()

    if 'MonoJ' in addName:
        masses = GetMasses(addName)
        coupling = GetCoupling(addName)
        m_med = masses[0]
        m_DM = masses[1]
        label = coupling + ", m_{med}="+str(int(m_med))+"GeV, m_{DM}="+str(int(m_DM))+"GeV"
        # print label
        signallabel = ROOT.TLatex(0.18, 0.07, label)
        signallabel.SetNDC()
        signallabel.SetTextSize(0.04)   
        signallabel.Draw()

    up.Print()
    down.Print()

    # for i in range(up.GetNbinsX()):
    # print "bin ", i, " up: ", up.GetBinContent(i)-nom.GetBinContent(i)," down: ", down.GetBinContent(i)-nom.GetBinContent(i)

    up.Draw("samehist")
    up.SetFillColor(0)
    up.SetLineColor(2)
    up.SetLineWidth(2)

    down.Draw("samehist")
    down.SetFillColor(0)
    down.SetLineColor(4)
    down.SetLineWidth(2)

    legend = ROOT.TLegend(0.32, 0.7, 0.83, 0.85)
    legend.AddEntry(nom, "nominal", "P")
    legend.AddEntry(up, syst + " up", "l")
    legend.AddEntry(down, syst + " down", "l")
    legend.Draw()
    legend.SetBorderSize(0)
    legend.SetLineStyle(0)
    legend.SetTextSize(0.05)

    c.cd(2)
    ratioUp = nom.Clone()
    ratioUp.Divide(up)
    ratioUp.SetLineColor(up.GetLineColor())
    ratioUp.Draw("E0")
    ratioUp.SetMarkerSize(0)
    ratioUp.GetYaxis().SetTitle("#frac{nominal}{variation}")
    ratioUp.GetYaxis().CenterTitle()
    ratioUp.GetYaxis().SetRangeUser(0.68, 1.32)
    # ratioUp.GetXaxis().SetLabelSize(nom.GetXaxis().GetLabelSize() * 3.5)
    # ratioUp.GetYaxis().SetLabelSize(nom.GetYaxis().GetLabelSize() * 3.5)
    # ratioUp.GetXaxis().SetTitleSize(nom.GetXaxis().GetTitleSize() * 3.5)
    # ratioUp.GetYaxis().SetTitleSize(nom.GetYaxis().GetTitleSize() * 2.0)
    ratioUp.GetYaxis().SetLabelSize(0.15)
    ratioUp.GetYaxis().SetTitleSize(0.12)
    ratioUp.GetYaxis().SetTitleOffset(0.6)

    ratioUp.GetXaxis().SetLabelSize(0.15)
    ratioUp.GetXaxis().SetTitleSize(0.15)

    ratioUp.SetTitle("")
    ratioUp.GetYaxis().SetNdivisions(505)

    ratioDown = nom.Clone()
    ratioDown.Divide(down)
    ratioDown.SetLineColor(down.GetLineColor())
    ratioDown.Draw("E0same")
    ratioDown.SetMarkerSize(0)

    # ratioDown.SetTitle("");
    # ratioDown.GetYaxis().SetTitle("nominal/variation");

    c.Update()
    lineratio = ROOT.TLine(c.cd(2).GetUxmin(), 1.0, c.cd(2).GetUxmax(), 1.0)
    lineratio.SetLineColor(ROOT.kBlack)
    lineratio.Draw()
    c.Update()

    c.SaveAs("pdfs/shifts/shifts_"+syst+addName+".pdf")
    c.SaveAs("pngs/shifts/shifts_"+syst+addName+".png")


ROOT.gStyle.SetOptStat(0)

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
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="realData", processlabel="unfolded")
    drawshifts(file=MCfile, process="unfolded",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="MCData", processlabel="unfolded")


BKGfile = ROOT.TFile.Open("rootfiles/data_normedmuRmuF.root")
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

WZSystematics = [
    "amc_scale_variation_muR",
    "amc_scale_variation_muF",
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

for sys in WZSystematics+additionalsysts:
    drawshifts(file=BKGfile, process="z_nunu_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="z_nunu_jets", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="Z(#nu#nu) + jets")
    drawshifts(file=BKGfile, process="z_ll_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="z_ll_jets", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="Z(ll) + jets") 
    drawshifts(file=BKGfile, process="w_lnu_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="w_lnu_jets", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="W(l#nu) + jets")

for sys in additionalsysts + powhegmuRmuR + defaultmuRmuR:
    drawshifts(file=BKGfile, process="singletop",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="singletop", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="single top")
    drawshifts(file=BKGfile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="ttbar", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="t#bar{t}")


for sys in additionalsysts + madgraphmuRmuR + defaultmuRmuR:
    drawshifts(file=BKGfile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="qcd", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="QCD")
    drawshifts(file=BKGfile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="gamma_jets", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]", processlabel="#gamma + jets)")

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
                variable="Gen_Hadr_Recoil_Pt", syst=sys, addName=x, xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")


noSelectionmuRmuR = [
    "muR",
    "muF",
]

noselectionFile = ROOT.TFile.Open("rootfiles/muRmuFnormalization.root")

for sys in noSelectionmuRmuR:
    drawshifts(file=noselectionFile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="qcd_noSelection", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")
    drawshifts(file=noselectionFile, process="qcd",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="qcd_noSelection_scaled", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")
    
    drawshifts(file=noselectionFile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="ttbar_noSelection", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")
    drawshifts(file=noselectionFile, process="ttbar",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="ttbar_noSelection_scaled", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")

    drawshifts(file=noselectionFile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt", syst=sys, addName="gamma_jets_noSelection", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")
    drawshifts(file=noselectionFile, process="gamma_jets",
               variable="Gen_Hadr_Recoil_Pt_scaled", syst=sys, addName="gamma_jets_noSelection_scaled", xlabel="Gen Recoil |#vec{U}_{T}| [GeV/c]")

# raw_input()
