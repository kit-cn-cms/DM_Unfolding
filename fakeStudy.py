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
ch.Draw("Evt_Pt_MET:Evt_Pt_GenMET>>met(100,0,800,100,0,800)","Weight_XS*Weight_GEN_nom","colz")
canvas.Print("MET.pdf")

canvas2=ROOT.TCanvas()
canvas2.cd()
# ROOT.gPad.SetLogz(False)
# ch.Draw("Jet_Pt[0]:Jet_GenJet_Pt[0]>>JetPt(20,0,200,20,0,200)","Weight_XS*Weight_GEN_nom","colz")
ch.Draw("Jet_Pt:Jet_GenJet_Pt>>JetPt(100,0,400,100,0,400)","Weight_XS*Weight_GEN_nom","colz")
canvas2.Print("LeadingJetPt.pdf")
