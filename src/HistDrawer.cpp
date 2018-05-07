#include "../interface/HistDrawer.hpp"


#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "THStack.h"
#include <TMath.h>
#include "TLine.h"
#include <TROOT.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <numeric>

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
	return dynamic_cast<const Base*>(ptr) != nullptr;
}

template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

	// initialize original index locations
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	std::sort(idx.begin(), idx.end(),
	[&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

	return idx;
}

void HistDrawer::Draw1D(TH1* hist, TString name, bool log, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = getCanvas(name);
	if (log) gPad->SetLogy();
	hist->Draw("histe");
	hist->SetTitle(name);

	if (xlabel == "none") {
		hist->SetXTitle(name);
	}
	else hist-> SetXTitle(xlabel);
	hist->SetYTitle(ylabel);

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + ".png");
	c->Write();
	output->Close();
}

void HistDrawer::Draw2D(TH2* hist, TString name, bool log, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = new TCanvas(name, name);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetOptStat(0);

	hist->Draw("COLZ1");
	hist->SetTitle(name);
	if (log) gPad->SetLogz();

	if (xlabel == "none") {
		hist->SetXTitle(name);
	}
	else hist-> SetXTitle(xlabel);

	if (ylabel == "none") {
		hist->SetYTitle(name);
	}
	else hist-> SetYTitle(ylabel);

	DrawLumiLabel(c);
	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + ".png");
	c->Write();
	output->Close();
}

void HistDrawer::DrawRatio(TH1* hist1, TH1* hist2, TString name, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = getCanvas(name);
	TH1F* ratio = (TH1F*) hist1->Clone();
	ratio->Divide(hist2);
	ratio->Draw("histe");
	ratio->SetTitle(name);

	if (xlabel == "none") {
		ratio->SetXTitle(name);
	}
	else ratio-> SetXTitle(xlabel);

	ratio-> SetYTitle(ylabel);
	c->Update();
	TLine *line = new TLine(c->GetUxmin(), 1.0, c->GetUxmax(), 1.0);
	line->SetLineColor(kRed);
	line->Draw();

	c->SaveAs(path.GetPdfPath() + name + ".pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + ".png");

	c->Write();
	output->Close();
}

void HistDrawer::DrawDataMC(TH1* data, std::vector<TH1*> MC, std::vector<std::string> names, TString name, bool log, bool normalize, bool drawpull, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = getCanvas(name, true, drawpull);
	TLegend* legend = getLegend();
	legend->AddEntry(data, "Data", "P");
	gStyle->SetErrorX(0.);
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	if (log) gPad->SetLogy();

	THStack* stack = new THStack(name, name);
	int fillcolor = 2;
	int index = 0;
	for (auto const& mc : MC) {
		SetHistoStyle(mc, fillcolor, true);
		mc->SetFillColor(fillcolor);
		if (normalize) mc ->Scale(1 / (MC.size()* mc->Integral()));
		stack->Add(mc);
		legend->AddEntry(mc, TString(names.at(index)), "F");
		index += 1;
		fillcolor += 1;
	}
	TH1F* lastStack = (TH1F*) (TH1*)stack->GetStack()->Last();
	if (normalize) 	data->Scale(1 / data->Integral());

	float max_data = data->GetMaximum();
	float max_Stack = lastStack->GetMaximum();
	if (max_data > max_Stack) stack->SetMaximum(max_data);
	else stack->SetMaximum(max_Stack);
	stack->SetMinimum(10);
	stack->Draw("hist");
	data->Draw("sameP");
	legend->Draw("same");
	data->SetStats(false);


	data->SetMarkerStyle(20);
	// data->SetMarkerSize();
	if (xlabel == "none") {
		data->SetXTitle(name);
	}
	else data-> SetXTitle(xlabel);
	DrawLumiLabel(c);

	c->cd(2);
	TH1* ratio = (TH1*) data->Clone();
	ratio->Divide( (TH1*)stack->GetStack()->Last());
	ratio->Draw("E0");
	ratio->GetYaxis()->SetTitle("#frac{Data}{MC Sample}");
	ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
	ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 2.4);
	ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 2.4);
	ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 3);
	ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 3);
	ratio->GetYaxis()->SetTitleOffset(0.5);
	ratio->SetTitle("");
	ratio->GetYaxis()->SetNdivisions(505);

	c->Update();
	TLine *lineratio = new TLine(c->cd(2)->GetUxmin(), 1.0, c->cd(2)->GetUxmax(), 1.0);
	lineratio->SetLineColor(kBlack);
	lineratio->Draw();
	c->Update();


	if (drawpull) {
		ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 1.75);
		ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 1.75);
		ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleOffset(0.3);

		c->Update();
		c->cd(3);
		TH1* pull = (TH1*) data->Clone();
		pull->Reset();
		for (Int_t bin = 1; bin <= data->GetNbinsX(); bin++) {
			double sigma_d = data->GetBinError(bin);
			double sigma_mc = lastStack->GetBinError(bin);
			double error = sqrt(sigma_d * sigma_d - sigma_mc * sigma_mc);
			double content = (data->GetBinContent(bin) - lastStack->GetBinContent(bin)) / sigma_d;
			pull->SetBinContent(bin, content);
			pull->SetBinError(bin, 0);
		}
		pull->Draw("P");
		pull->GetYaxis()->SetTitle("#frac{Data-MC}{#sigma}");
		pull->GetYaxis()->SetRangeUser(-3.5, 3.5);
		pull->GetXaxis()->SetLabelSize(pull->GetXaxis()->GetLabelSize() * 3);
		pull->GetYaxis()->SetLabelSize(pull->GetYaxis()->GetLabelSize() * 3);
		pull->GetXaxis()->SetTitleSize(pull->GetXaxis()->GetTitleSize() * 3.5);
		pull->GetYaxis()->SetTitleSize(pull->GetYaxis()->GetTitleSize() * 3.5);
		pull->GetYaxis()->SetTitleOffset(0.5);
		pull->GetYaxis()->SetNdivisions(505);
		pull->SetTitle("");
		c->Update();
		TLine *linepull = new TLine(c->cd(3)->GetUxmin(), 0.0, c->cd(3)->GetUxmax(), 0);
		linepull->SetLineColor(kBlack);
		linepull->Draw();
	}



	c->cd(1);
	c->SaveAs(path.GetPdfPath() + name + "_stacked.pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + "_stacked.png");
	c->Write();
	output->Close();
	std::cout << name << ": ";
	// data->Chi2Test(lastStack, "WWP");
}

void HistDrawer::DrawDataMC(TH1* data, std::vector<TH1*> MC, std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap, TString name, bool log, bool normalize, bool drawpull, TString xlabel, TString ylabel) {
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = getCanvas(name, true, drawpull);
	TLegend* legend = getLegend();
	legend->AddEntry(data, "Data", "P");
	gStyle->SetErrorX(0.);
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	if (log) gPad->SetLogy();
	THStack* stack = new THStack(name, name);

	//figure out yield and sort correspondingly
	std::vector<double> MCyield;
	std::vector<std::string> names;
	std::vector<std::string> sortedNames;
	for (auto const& x : nameGenSampleColorMap) {
		names.push_back(x.first);
		MCyield.push_back(std::get<0>(x.second)->Integral());
	}
	for (auto i : sort_indexes(MCyield)) {
		sortedNames.push_back(names[i]);
	}
	//loop over Bkgs and create stack
	for (auto const& bkgname : sortedNames) {
		std::pair<TH1*, int> x = nameGenSampleColorMap.at(bkgname);
		SetHistoStyle(std::get<0>(x), std::get<1>(x), true);
		std::get<0>(x)->SetFillColor(std::get<1>(x));
		std::get<0>(x)->Print();
		if (normalize) std::get<0>(x) ->Scale(1 / (MC.size()* std::get<0>(x)->Integral()));
		stack->Add(std::get<0>(x));
		legend->AddEntry(std::get<0>(x), TString(bkgname), "F");
	}
	TH1F* lastStack = (TH1F*) (TH1*)stack->GetStack()->Last();
	if (normalize) 	data->Scale(1 / data->Integral());

	float max_data = data->GetMaximum();
	float max_Stack = lastStack->GetMaximum();
	if (max_data > max_Stack) stack->SetMaximum(max_data);
	else stack->SetMaximum(max_Stack);
	stack->SetMinimum(10);
	stack->Draw("hist");
	data->Draw("sameP");
	legend->Draw("same");
	data->SetStats(false);
	DrawLumiLabel(c);


	data->SetMarkerStyle(20);
	// data->SetMarkerSize();
	if (xlabel == "none") {
		data->SetXTitle(name);
	}
	else data-> SetXTitle(xlabel);

	c->cd(2);
	TH1* ratio = (TH1*) data->Clone();
	ratio->Divide( (TH1*)stack->GetStack()->Last());
	ratio->Draw("E0");
	ratio->GetYaxis()->SetTitle("#frac{Data}{MC Sample}");
	ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
	ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 2.4);
	ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 2.4);
	ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 3);
	ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 3);
	ratio->GetYaxis()->SetTitleOffset(0.5);
	ratio->SetTitle("");
	ratio->GetYaxis()->SetNdivisions(505);

	c->Update();
	TLine *lineratio = new TLine(c->cd(2)->GetUxmin(), 1.0, c->cd(2)->GetUxmax(), 1.0);
	lineratio->SetLineColor(kBlack);
	lineratio->Draw();
	c->Update();


	if (drawpull) {
		ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 1.75);
		ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 1.75);
		ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleOffset(0.3);

		c->Update();
		c->cd(3);
		TH1* pull = (TH1*) data->Clone();
		pull->Reset();
		for (Int_t bin = 1; bin <= data->GetNbinsX(); bin++) {
			double sigma_d = data->GetBinError(bin);
			double sigma_mc = lastStack->GetBinError(bin);
			double error = sqrt(sigma_d * sigma_d - sigma_mc * sigma_mc);
			double content = (data->GetBinContent(bin) - lastStack->GetBinContent(bin)) / sigma_d;
			pull->SetBinContent(bin, content);
			pull->SetBinError(bin, 0);
		}
		pull->Draw("P");
		pull->GetYaxis()->SetTitle("#frac{Data-MC}{#sigma}");
		pull->GetYaxis()->SetRangeUser(-3.5, 3.5);
		pull->GetXaxis()->SetLabelSize(pull->GetXaxis()->GetLabelSize() * 3);
		pull->GetYaxis()->SetLabelSize(pull->GetYaxis()->GetLabelSize() * 3);
		pull->GetXaxis()->SetTitleSize(pull->GetXaxis()->GetTitleSize() * 3.5);
		pull->GetYaxis()->SetTitleSize(pull->GetYaxis()->GetTitleSize() * 3.5);
		pull->GetYaxis()->SetTitleOffset(0.5);
		pull->GetYaxis()->SetNdivisions(505);
		pull->SetTitle("");
		c->Update();
		TLine *linepull = new TLine(c->cd(3)->GetUxmin(), 0.0, c->cd(3)->GetUxmax(), 0);
		linepull->SetLineColor(kBlack);
		linepull->Draw();
	}



	c->cd(1);
	c->SaveAs(path.GetPdfPath() + name + "_stacked.pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + "_stacked.png");
	c->Write();
	output->Close();
	std::cout << name << ": ";
	// data->Chi2Test(lastStack, "WWP");
}


void HistDrawer::DrawDataMCerror(TGraphErrors* data_stat, TGraphAsymmErrors* data_syst, std::vector<TH1*> MC, std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap, TString name, bool log, bool normalize, bool drawpull, TString xlabel, TString ylabel) {
	gStyle->SetEndErrorSize(10);
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	TCanvas* c = getCanvas(name, true, drawpull);
	TLegend* legend = getLegend();
	legend->AddEntry(data_stat, "Data", "P");
	gStyle->SetErrorX(0.);
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	if (log) gPad->SetLogy();
	THStack* stack = new THStack(name, name);
	//figure out yield and sort correspondingly
	std::vector<double> MCyield;
	std::vector<std::string> names;
	std::vector<std::string> sortedNames;
	for (auto const& x : nameGenSampleColorMap) {
		names.push_back(x.first);
		MCyield.push_back(std::get<0>(x.second)->Integral());
	}
	for (auto i : sort_indexes(MCyield)) {
		sortedNames.push_back(names[i]);
	}
	//loop over Bkgs and create stack
	for (auto const& bkgname : sortedNames) {
		std::pair<TH1*, int> x = nameGenSampleColorMap.at(bkgname);
		SetHistoStyle(std::get<0>(x), std::get<1>(x), true);
		std::get<0>(x)->SetFillColor(std::get<1>(x));
		std::get<0>(x)->Print();
		if (normalize) std::get<0>(x) ->Scale(1 / (MC.size()* std::get<0>(x)->Integral()));
		stack->Add(std::get<0>(x));
		legend->AddEntry(std::get<0>(x), TString(bkgname), "F");
	}
	double max_data = TMath::MaxElement(data_syst->GetN(), data_syst->GetX());
	// float max_data = data->GetMaximum();
	TH1F* lastStack = (TH1F*) (TH1*)stack->GetStack()->Last();
	float max_Stack = lastStack->GetMaximum();
	if (max_data > max_Stack) stack->SetMaximum(max_data);
	else stack->SetMaximum(max_Stack);
	stack->SetMinimum(10);
	stack->Draw("hist");
	data_stat->Draw("Psame");
	data_stat->SetMarkerStyle(20);
	data_syst->Draw("same[]");
	legend->Draw("same");
	DrawLumiLabel(c);

	if (xlabel == "none") {
		lastStack->SetXTitle(name);
	}
	else lastStack-> SetXTitle(xlabel);
	lastStack->SetTitle("");

	// TH1* data = new TH1F("data", "data", lastStack->GetNbinsX(), BinEdges.data());
	TH1* data = (TH1*) MC.at(0)->Clone();
	data->ResetAttMarker();
	data->Reset();
	for (int i = 1; i <= lastStack->GetNbinsX() ; i++) {
		data->SetBinContent(i, data_syst->GetY()[i - 1]);
		float Ehigh = data_syst->GetEYhigh()[i - 1];
		float Elow = data_syst->GetEYlow()[i - 1];
		if (Ehigh > Elow) data->SetBinError(i, Ehigh);
		else data->SetBinError(i, Elow);
	}
	data->SetMarkerStyle(20);


	c->cd(2);
	TH1* ratio = (TH1*) data->Clone();
	ratio->Divide( (TH1*)stack->GetStack()->Last());
	ratio->Draw("E0");
	ratio->GetYaxis()->SetTitle("#frac{Data}{MC Sample}");
	ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
	ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 2.4);
	ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 2.4);
	ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 1.5);
	ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 2.2);
	ratio->GetYaxis()->SetTitleOffset(053);
	ratio->SetTitle("");
	ratio->GetYaxis()->SetNdivisions(505);

	c->Update();
	TLine *lineratio = new TLine(c->cd(2)->GetUxmin(), 1.0, c->cd(2)->GetUxmax(), 1.0);
	lineratio->SetLineColor(kBlack);
	lineratio->Draw();
	c->Update();


	if (drawpull) {
		ratio->GetXaxis()->SetLabelSize(ratio->GetXaxis()->GetLabelSize() * 1.75);
		ratio->GetYaxis()->SetLabelSize(ratio->GetYaxis()->GetLabelSize() * 1.75);
		ratio->GetXaxis()->SetTitleSize(ratio->GetXaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleSize(ratio->GetYaxis()->GetTitleSize() * 1.75);
		ratio->GetYaxis()->SetTitleOffset(0.3);

		c->Update();
		c->cd(3);
		TH1* pull = (TH1*) data->Clone();
		pull->Reset();
		for (Int_t bin = 1; bin <= data->GetNbinsX(); bin++) {
			double sigma_d = data->GetBinError(bin);
			double sigma_mc = lastStack->GetBinError(bin);
			double error = sqrt(sigma_d * sigma_d - sigma_mc * sigma_mc);
			double content = (data->GetBinContent(bin) - lastStack->GetBinContent(bin)) / sigma_d;
			pull->SetBinContent(bin, content);
			pull->SetBinError(bin, 0);
		}
		pull->Draw("P");
		pull->GetYaxis()->SetTitle("#frac{Data-MC}{#sigma}");
		pull->GetYaxis()->SetRangeUser(-3.5, 3.5);
		pull->GetXaxis()->SetLabelSize(pull->GetXaxis()->GetLabelSize() * 3);
		pull->GetYaxis()->SetLabelSize(pull->GetYaxis()->GetLabelSize() * 3);
		pull->GetXaxis()->SetTitleSize(pull->GetXaxis()->GetTitleSize() * 3.5);
		pull->GetYaxis()->SetTitleSize(pull->GetYaxis()->GetTitleSize() * 2.2);
		pull->GetYaxis()->SetTitleOffset(0.5);
		pull->GetYaxis()->SetNdivisions(505);
		pull->SetTitle("");
		pull->GetXaxis()->SetTitleOffset(1);
		pull->GetXaxis()->SetTitle(xlabel);
		c->Update();
		TLine *linepull = new TLine(c->cd(3)->GetUxmin(), 0.0, c->cd(3)->GetUxmax(), 0);
		linepull->SetLineColor(kBlack);
		linepull->Draw();
	}

	c->cd(1);
	c->SaveAs(path.GetPdfPath() + name + "_stacked.pdf");
	c->SaveAs(path.GetPdfPath() + "../pngs/" + name + "_stacked.png");
	c->Write();
	output->Close();
	std::cout << name << ": ";
	// data->Chi2Test(lastStack, "WWP");
}


void HistDrawer::SetHistoStyle(TH1* histo, int color, bool filled) {
	if (instanceof<TH1F>(histo)) histo->SetStats(false);
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

TCanvas* HistDrawer::getCanvas(TString name, bool ratiopad, bool pullpad) {
	if (ratiopad && !pullpad) {
		TCanvas* c = new TCanvas(name, name, 1024, 1024);
		c->Divide(1, 2);
		c->cd(1)->SetPad(0., 0.3, 1.0, 1.0);
		c->cd(1)->SetBottomMargin(0.0);
		c->cd(2)->SetPad(0., 0.0, 1.0, 0.3);
		c->cd(2)->SetTopMargin(0.0);
		c->cd(1)->SetTopMargin(0.07);
		c->cd(2)->SetBottomMargin(0.4);
		c->cd(1)->SetRightMargin(0.05);
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(2)->SetRightMargin(0.05);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(2)->SetTicks(1, 1);
		c->cd(1)->SetTicks(1, 1);
		return c;
	}
	if (pullpad) {
		TCanvas* c = new TCanvas(name, name, 1024, 1024);
		c->Divide(1, 3);
		c->cd(1)->SetPad(0., 0.4, 1.0, 1.0);
		c->cd(1)->SetBottomMargin(0.0);
		c->cd(2)->SetPad(0., 0.25, 1.0, 0.4);
		c->cd(2)->SetTopMargin(0.0);
		c->cd(3)->SetPad(0., 0.0, 1.0, 0.25);
		c->cd(3)->SetTopMargin(0.0);
		c->cd(1)->SetTopMargin(0.07);
		c->cd(2)->SetBottomMargin(0.0);
		c->cd(3)->SetBottomMargin(0.4);
		c->cd(1)->SetRightMargin(0.05);
		c->cd(1)->SetLeftMargin(0.15);
		c->cd(2)->SetRightMargin(0.05);
		c->cd(2)->SetLeftMargin(0.15);
		c->cd(3)->SetRightMargin(0.05);
		c->cd(3)->SetLeftMargin(0.15);
		c->cd(2)->SetTicks(1, 1);
		c->cd(1)->SetTicks(1, 1);
		return c;
	}
	else {
		TCanvas* c = new TCanvas(name, name, 1024, 768);
		c->SetRightMargin(0.05);
		c->SetTopMargin(0.07);
		c->SetLeftMargin(0.15);
		c->SetBottomMargin(0.15);
		c->SetTicks(1, 1);
		return c;
	}
}

void HistDrawer::DrawLumiLabel(TCanvas* canvas) {
	TPaveText *pt = new TPaveText(0.2, .95, 0.9, 0.99, "blNDC");
	pt->AddText("work in Progress            35.9 fb^{-1} (13 TeV)");
	pt->SetFillColor(kWhite);
	pt->Draw("SAME");
}

TLegend* HistDrawer::getLegend() {
	TLegend* legend = new TLegend(0.5, 0.7, 0.92, 0.9);
	legend->SetNColumns(2);
	legend->SetColumnSeparation(0.2);
	legend->SetBorderSize(0);
	legend->SetLineStyle(0);
	legend->SetTextFont(42);
	legend->SetTextSize(0.05);
	legend->SetFillStyle(0);
	return legend;
}

