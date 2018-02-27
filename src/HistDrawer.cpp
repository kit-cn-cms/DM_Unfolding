#include "../interface/HistDrawer.hpp"


#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLine.h"


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
	c->Update();
	TLine *line=new TLine(c->GetUxmin(),1.0,c->GetUxmax(),1.0);
	line->SetLineColor(kRed);
	line->Draw();

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->Write();
	output->Close();
}

void HistDrawer::DrawDataMC(TH1F* data, std::vector<TH1F*> MC, TString name, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = new TCanvas(name + "_stacked", name + "_stacked");
	THStack* stack = new THStack(name, name);
	int fillcolor = 2;
	for (auto const& mc : MC) {
		SetHistoStyle(mc, fillcolor, true);
		mc->SetFillColor(fillcolor);
		stack->Add(mc);
		fillcolor += 1;
	}
	stack->Draw("histe");
	data->Draw("samee");
	data->SetMarkerStyle(20);
	// data->SetMarkerSize();
	if (xlabel == "none") {
		data->SetXTitle(name);
	}
	else data-> SetXTitle(xlabel);

	c->SaveAs(path.GetPdfPath() + name + "_stacked.pdf");
	c->Write();
	output->Close();
}

void HistDrawer::SetHistoStyle(TH1F* histo, int color, bool filled) {
	histo->GetYaxis()->SetTitleOffset(1.4);
	histo->GetXaxis()->SetTitleOffset(1.2);
	histo->GetYaxis()->SetTitleSize(0.05);
	histo->GetXaxis()->SetTitleSize(0.05);
	histo->GetYaxis()->SetLabelSize(0.05);
	histo->GetXaxis()->SetLabelSize(0.05);
	histo->SetMarkerColor(color);
	if (filled) {
		histo->SetLineColor( kBlack );
		histo->SetFillColor( color );
		histo->SetLineWidth(2);
	}
	else {
		histo->SetLineColor(color);
		histo->SetLineWidth(2);
	}


}