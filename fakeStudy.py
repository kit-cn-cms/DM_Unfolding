#! /usr/bin/env python

import ROOT
import os
ROOT.gROOT.SetBatch()

path_ntuples="/nfs/dust/cms/user/swieland/Darkmatter/ntuples_tagging"
ch=ROOT.TChain("MVATree")

for path, subdirs, files in os.walk(path_ntuples):
	for sdir in subdirs:
		if "MET" not in sdir: 
			print sdir
			ch.Add(path_ntuples+"/"+sdir+"/*nominal*.root")

print ch.GetEntries()

ROOT.gStyle.SetPadRightMargin(0.15)
ROOT.gStyle.SetPalette(57)
w=1200
h=800
canvas = ROOT.TCanvas()
canvas.cd()
canvas.SetCanvasSize(w,h)

ROOT.gPad.SetLogz()
ROOT.gStyle.SetOptStat(0)
ch.Draw("Gen_Hadr_Recoil_Pt:Hadr_Recoil_Pt>>met(100,0,800,100,0,800)","Weight_XS*Weight_GEN_nom*Weight_PU*Weight_CSV","colz")
met=ROOT.gDirectory.Get("met")
met.GetXaxis().SetTitle("generated hadr. recoil |#vec{U}| [GeV/c]")
met.GetXaxis().SetTitleSize(0.05)
met.GetXaxis().SetLabelSize(0.04)
met.GetXaxis().SetTitleOffset(0.82)

met.GetYaxis().SetTitle("reconstructed hadr. recoil |#vec{U}| [GeV/c]")
met.SetTitle("")
met.GetYaxis().SetTitleSize(0.05)
met.GetYaxis().SetLabelSize(0.04)
met.GetYaxis().SetTitleOffset(0.85)

met.GetZaxis().SetTitle("Events")
met.GetZaxis().SetTitleSize(0.07)
met.GetZaxis().SetLabelSize(0.04)
met.GetZaxis().SetTitleOffset(0.7)

cms = ROOT.TLatex(0.12, 0.91, 'CMS private work in progress'  )
cms.SetNDC()
cms.SetTextSize(0.055)
lumi = ROOT.TLatex(0.7, 0.91, '35.9 fb^{-1} (13 TeV)'  )
lumi.SetNDC()
lumi.SetTextSize(0.045)

cms.Draw()
lumi.Draw()

canvas.Print("hadrRecoil.pdf")

# ROOT.gStyle.SetOptStat(000001111)

# canvas2=ROOT.TCanvas()
# canvas2.cd()
# # ROOT.gPad.SetLogz(False)
# # ch.Draw("Jet_Pt[0]:Jet_GenJet_Pt[0]>>JetPt(20,0,200,20,0,200)","Weight_XS*Weight_GEN_nom","colz")
# ch.Draw("Jet_Pt:Jet_GenJet_Pt>>JetPt(100,0,400,100,0,400)","Weight_XS*Weight_GEN_nom","colz")
# canvas2.Print("LeadingJetPt.pdf")


# canvas3=ROOT.TCanvas()
# canvas3.cd()

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom")
# canvas3.Print("recoMingen.pdf")

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom*recoSelected")
# canvas3.Print("recoMingen_RecoSelected.pdf")

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom*recoSelected*GenMETSelection*GenMonoJetSelection*GenLeptonVetoSelection*GenBTagVetoSelection*GenPhotonVetoSelection*GenmonoVselection")
# canvas3.Print("recoMingen_RecoGenSelected.pdf")

# ch.Draw("Evt_Pt_GenMET>>monov(100,0,800)","Weight_XS*Weight_GEN_nom*recoSelected*GenMETSelection*GenMonoJetSelection*GenLeptonVetoSelection*GenBTagVetoSelection*GenPhotonVetoSelection*(GenmonoVselection!=1)")
# monov=ROOT.gDirectory.Get("monov")
# print monov.Integral()
# canvas3.Print("monovfailed.pdf")