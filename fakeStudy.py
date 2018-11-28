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

canvas=ROOT.TCanvas()
ROOT.gPad.SetLogz()
ROOT.gStyle.SetOptStat(0)
ch.Draw("Hadr_Recoil_Pt:Gen_Hadr_Recoil_Pt>>met(100,0,800,100,0,800)","Weight_XS*Weight_GEN_nom*Weight_PU*Weight_CSV","colz")
met=ROOT.gDirectory.Get("met")
met.GetXaxis().SetTitle("generated hadr. Recoil #vec{U} [Gev/c]")
met.GetYaxis().SetTitle("reconstructed hadr. Recoil #vec{U} [Gev/c]")
met.SetTitle("CMS private work simulation")

metclone=met.Clone()
for xbin in range(met.GetNbinsX()):
	for ybin in range(met.GetNbinsY()):
		metclone.SetBinContent(ybin,xbin,met.GetBinContent(xbin,ybin))
metclone.GetYaxis().SetTitle("generated hadr. Recoil #vec{U} [Gev/c]")
metclone.GetXaxis().SetTitle("reconstructed hadr. Recoil #vec{U} [Gev/c]")
metclone.SetTitle("CMS private work simulation")
metclone.Draw("colz")
canvas.Print("hadrRecoil.pdf")

ROOT.gStyle.SetOptStat(000001111)

# canvas2=ROOT.TCanvas()
# canvas2.cd()
# # ROOT.gPad.SetLogz(False)
# # ch.Draw("Jet_Pt[0]:Jet_GenJet_Pt[0]>>JetPt(20,0,200,20,0,200)","Weight_XS*Weight_GEN_nom","colz")
# ch.Draw("Jet_Pt:Jet_GenJet_Pt>>JetPt(100,0,400,100,0,400)","Weight_XS*Weight_GEN_nom","colz")
# canvas2.Print("LeadingJetPt.pdf")


canvas3=ROOT.TCanvas()
canvas3.cd()

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom")
# canvas3.Print("recoMingen.pdf")

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom*recoSelected")
# canvas3.Print("recoMingen_RecoSelected.pdf")

# ch.Draw("Evt_Pt_MET-Evt_Pt_GenMET>>Diff(100,-100,100)","Weight_XS*Weight_GEN_nom*recoSelected*GenMETSelection*GenMonoJetSelection*GenLeptonVetoSelection*GenBTagVetoSelection*GenPhotonVetoSelection*GenmonoVselection")
# canvas3.Print("recoMingen_RecoGenSelected.pdf")

ch.Draw("Evt_Pt_GenMET>>monov(100,0,800)","Weight_XS*Weight_GEN_nom*recoSelected*GenMETSelection*GenMonoJetSelection*GenLeptonVetoSelection*GenBTagVetoSelection*GenPhotonVetoSelection*(GenmonoVselection!=1)")
monov=ROOT.gDirectory.Get("monov")
print monov.Integral()
canvas3.Print("monovfailed.pdf")