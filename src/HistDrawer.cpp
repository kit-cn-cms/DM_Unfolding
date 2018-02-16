#include "../interface/HistDrawer.hpp"


#include <iostream>
#include "TFile.h"
#include "TCanvas.h"


void HistDrawer::Draw1D(TH1* hist, TString name, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = new TCanvas(name, name);
	hist->Draw("histe");
	hist->SetTitle(name);

	if (xlabel == "none") {
		hist->SetXTitle(name);
	}
	else hist-> SetXTitle(xlabel);
	hist->SetYTitle(ylabel);

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->Write();
	output->Close();
}

void HistDrawer::Draw2D(TH2* hist, TString name, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = new TCanvas(name, name);

	hist->Draw("COLZ");
	hist->SetTitle(name);

	if (xlabel == "none") {
		hist->SetXTitle(name + "_Reco");
	}
	else hist-> SetXTitle(xlabel);

	if (ylabel == "none") {
		hist->SetYTitle(name + "_Gen");
	}
	else hist-> SetYTitle(ylabel);

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->Write();
	output->Close();
}

void HistDrawer::DrawRatio(TH1* hist1, TH1* hist2, TString name, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = new TCanvas(name, name);

	// TH1F *ratio_Genpt = (TH1F*)pt_unfolded->Clone("ratio_Genpt");
	hist1->Divide(hist2);
	hist1->Draw("histe");
	hist1->SetTitle(name);

	if (xlabel == "none") {
		hist1->SetXTitle(name);
	}
	else hist1-> SetXTitle(xlabel);

	hist1-> SetYTitle(ylabel);

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->Write();
	output->Close();
}