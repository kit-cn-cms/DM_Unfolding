#include "../interface/Unfolder.hpp"

using namespace std;


TString GetOutputFilePath()
{
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/rootfiles/output.root";
	return filepath;
}


TUnfoldDensity* Unfolder::SetUp(TH2F* A, TH1F* input) {
	cout << "Setting Up Unfolder.." << endl;
	TUnfoldDensity* unfold = new TUnfoldDensity(A, TUnfold::kHistMapOutputVert, TUnfold::kRegModeDerivative);
	unfold->SetInput(input);
	cout << "Unfolder SetUp!" << endl;
	return unfold;
}

void Unfolder::ParseConfig() {
	cout << "Parsing Unfolder Config..." << endl;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);
	biasScale = pt.get<float>("Unfolding.biasScale");
	nScan = pt.get<int>("Unfolding.nScan");
	tauMin = pt.get<double>("Unfolding.tauMin");
	tauMax = pt.get<double>("Unfolding.tauMax");
}

std::tuple<int , TSpline*, TGraph*> Unfolder::FindBestTau(TUnfoldDensity* unfold) {
	cout << "Finding BestTau" << endl;
	int iBest;
	TSpline *scanResult = 0;
	TGraph *lCurve = 0;

	iBest = unfold->ScanTau(nScan, tauMin, tauMax, &scanResult, TUnfoldDensity::kEScanTauRhoAvg, 0, 0, &lCurve);

	std::cout << " Best tau=" << unfold->GetTau() << "\n";
	std::cout << "chi**2=" << unfold->GetChi2A() << "+" << unfold->GetChi2L() << " / " << unfold->GetNdf() << "\n";
	return std::make_tuple(iBest, scanResult, lCurve);

}

void Unfolder::VisualizeTau(std::tuple<int, TSpline* , TGraph* > tuple) {
	int iBest = get<0>(tuple);
	TSpline* scanResult = get<1>(tuple);
	TGraph* lCurve = get<2>(tuple);

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

	TFile *output = new TFile(GetOutputFilePath(), "recreate");
	//Draw Tau Graphs
	TCanvas* tau = new TCanvas("tau", "tau");
	tau->cd();
	scanResult->Draw();
	knots->Draw("*");
	bestRhoLogTau->SetMarkerColor(kRed);
	bestRhoLogTau->Draw("*");
	tau->SaveAs("pdfs/tau.pdf");
	tau->Write();
	output->Close();
}

void Unfolder::DoUnfolding(TUnfoldDensity* unfold, TH1F* h_Data) {
	unfold->DoUnfold(unfold->GetTau(), h_Data, biasScale);
}

std::tuple<TH1*, TH1*> Unfolder::GetOutput(TUnfoldDensity* unfold) {
	//GetOutput
	TH1 *h_unfolded = unfold->GetOutput("h_unfolded");
	h_unfolded->Print();
	h_unfolded->SetXTitle("unfolded");
	TH1 *h_unfoldedback = unfold->GetFoldedOutput("unfoldedback");

	TFile *output = new TFile(GetOutputFilePath(), "update");
	h_unfolded->Write();
	h_unfoldedback->Write();
	output->Map();
	output->Close();

	return std::make_tuple(h_unfolded, h_unfoldedback);
}

TUnfoldDensity* Unfolder::Unfold(TH2F* A, TH1F* input) {
	ParseConfig();
	TUnfoldDensity* unfold = SetUp(A, input);
	std::tuple<int , TSpline*, TGraph*> TauResult;
	TauResult = FindBestTau(unfold);
	VisualizeTau(TauResult);
	DoUnfolding(unfold, input);
	std::tuple<TH1*, TH1*> output;
	return unfold;
}



