#include "../interface/Unfolder.hpp"


#include <iostream>
#include <tuple>
#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;


TUnfoldDensity* Unfolder::SetUp(TH2* A, TH1F* input) {
	cout << "Setting Up Unfolder.." << endl;
	TUnfoldDensity* unfold = new TUnfoldDensity(A, TUnfoldDensity::kHistMapOutputVert, TUnfold::kRegModeCurvature,
	        TUnfold::kEConstraintArea, TUnfoldDensity::kDensityModeBinWidthAndUser, 0, 0, 0, "*[UOB]" );
	float n_input = unfold->SetInput(input);
	if (n_input >= 1) {
		std::cout << "Unfolding result may be wrong\n";
	}

	cout << "Unfolder SetUp!" << endl;
	return unfold;
}

void Unfolder::ParseConfig() {
	cout << "Parsing Unfolder Config..." << endl;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(string(path.GetConfigPath()), pt);
	biasScale = pt.get<float>("Unfolding.biasScale");
	nScan = pt.get<int>("Unfolding.nScan");
	tauMin = pt.get<double>("Unfolding.tauMin");
	tauMax = pt.get<double>("Unfolding.tauMax");
}

std::tuple<int , TSpline*, TGraph*> Unfolder::FindBestTau(TUnfoldDensity* unfold, TString name, const char * distribution, const char * axisSteering ) {
	cout << "Finding BestTau" << endl;
	int iBest;
	TSpline *scanResult = 0;
	TGraph *lCurve = 0;

	iBest = unfold->ScanTau(nScan, tauMin, tauMax, &scanResult, TUnfoldDensity::kEScanTauRhoAvgSys, distribution, axisSteering, &lCurve);

	std::cout << " Best tau=" << unfold->GetTau() << "\n";
	std::cout << "chi**2 = chi**2_A+chi**2_L/Ndf = " << unfold->GetChi2A() << "+" << unfold->GetChi2L() << " / " << unfold->GetNdf() << "\n";

	//Graphs to Visualize best choice of Tau
	Double_t t[1], rho[1], x[1], y[1];
	scanResult->GetKnot(iBest, t[0], rho[0]);
	lCurve->GetPoint(iBest, x[0], y[0]);
	TGraph *bestRhoLogTau = new TGraph(1, t, rho);
	TGraph *bestLCurve = new TGraph(1, x, y);
	Double_t *tAll = new Double_t[nScan], *rhoAll = new Double_t[nScan];
	for (Int_t i = 0; i < nScan; i++) {
		scanResult->GetKnot(i, tAll[i], rhoAll[i]);
	}
	TGraph *knots = new TGraph(nScan, tAll, rhoAll);

	TFile *output = new TFile(path.GetOutputFilePath(), "UPDATE");
	//Draw Tau Graphs
	TCanvas* tau = new TCanvas("tau_" + name, "tau_" + name);
	tau->cd();
	scanResult->Draw();
	knots->Draw("*");
	bestRhoLogTau->SetMarkerColor(kRed);
	bestRhoLogTau->Draw("*");
	tau->SaveAs(path.GetPdfPath() + "tau_" + name + ".pdf");
	tau->SaveAs(path.GetPdfPath() + "../pngs/tau_" + name + ".png");
	tau->Write();

	TCanvas* clCurve = new TCanvas("LCurve_" + name, "LCurve_" + name);
	clCurve->cd();
	lCurve->Draw();
	bestLCurve->Draw("*");
	clCurve->SaveAs(path.GetPdfPath() + "LCurve_" + name + ".pdf");
	clCurve->SaveAs(path.GetPdfPath() +  "../pngs/LCurve_" + name + ".png");

	output->Close();

	return std::make_tuple(iBest, scanResult, lCurve);
}

std::tuple<int , TSpline*, TGraph*> Unfolder::FindBestTauLcurve(TUnfoldDensity* unfold, TString name) {
	cout << "Finding BestTau" << endl;
	int iBest;
	TSpline *logTauX, *logTauY, *logTauCurvature;
	TGraph *lCurve = 0;

	// iBest = unfold->ScanTau(nScan, tauMin, tauMax, &scanResult, TUnfoldDensity::kEScanTauRhoAvgSys, distribution, axisSteering, &lCurve);
	iBest = unfold->ScanLcurve(nScan, 0, 0, &lCurve, &logTauX, &logTauY);

	std::cout << " Best tau=" << unfold->GetTau() << "\n";
	std::cout << "chi**2 = chi**2_A+chi**2_L/Ndf = " << unfold->GetChi2A() << "+" << unfold->GetChi2L() << " / " << unfold->GetNdf() << "\n";

	//Graphs to Visualize best choice of Tau
	Double_t t[1], x[1], y[1];
	logTauX->GetKnot(iBest, t[0], x[0]);
	logTauY->GetKnot(iBest, t[0], y[0]);
	TGraph *bestLcurve = new TGraph(1, x, y);
	TGraph *bestLogTauLogChi2 = new TGraph(1, t, x);

	TCanvas* tau = new TCanvas("logtauvsChi2_" + name, "logtauvsChi2_" + name);
	logTauX->Draw();
	bestLogTauLogChi2->SetMarkerColor(kRed);
	bestLogTauLogChi2->Draw("*");
	tau->SaveAs(path.GetPdfPath() + "logtauvsChi2_" + name + ".pdf");
	tau->SaveAs(path.GetPdfPath() +  "../pngs/logtauvsChi2_" + name + ".png");

	TCanvas* clCurve = new TCanvas("LCurve_" + name, "LCurve_" + name);
	lCurve->Draw("AL");
	bestLcurve->SetMarkerColor(kRed);
	bestLcurve->Draw("*");
	clCurve->SaveAs(path.GetPdfPath() + "LCurve_" + name + ".pdf");
	clCurve->SaveAs(path.GetPdfPath() +  "../pngs/LCurve_" + name + ".png");

	// return std::make_tuple(iBest, scanResult, lCurve);
}



void Unfolder::DoUnfolding(TUnfoldDensity* unfold, TH1F* h_Data) {
	unfold->DoUnfold(unfold->GetTau(), h_Data, biasScale);
}

std::tuple<TH1*, TH1*> Unfolder::GetOutput(TUnfoldDensity* unfold) {
	//GetOutput
	TH1 *h_unfolded = unfold->GetOutput("h_unfolded");
	h_unfolded->SetXTitle("unfolded");
	TH1 *h_unfoldedback = unfold->GetFoldedOutput("unfoldedback");

	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	h_unfolded->Write();
	h_unfoldedback->Write();
	output->Close();

	return std::make_tuple(h_unfolded, h_unfoldedback);
}

void Unfolder::GetRegMatrix(TUnfoldDensity* unfold) {
	TH2 *histL = unfold->GetL("L");
	for (Int_t j = 1; j <= histL->GetNbinsY(); j++) {
		cout << "L[" << unfold->GetLBinning()->GetBinName(j) << "]";
		for (Int_t i = 1; i <= histL->GetNbinsX(); i++) {
			Double_t c = histL->GetBinContent(i, j);
			if (c != 0.0) cout << " [" << i << "]=" << c;
		}
		cout << "\n";
	}
}

void Unfolder::SubBkg(TUnfoldDensity* unfold, TH1* h_bkg, TString name) {
	unfold->SubtractBackground(h_bkg, name);
}




