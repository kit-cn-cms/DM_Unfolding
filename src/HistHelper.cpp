#include "HistHelper.hpp"
#include <TRandom3.h>



TH1F* HistHelper::Get1DHisto(TString name) {
	TH1::AddDirectory(kFALSE);
	TRandom3 rand;

	TFile *file = new TFile(path.GetHistoFilePath(), "open");
	TH1F* hist = (TH1F*)file->Get(name);
	// if (name.Contains("data")) {
	// 	for ( Int_t iBin = 1; iBin <= hist->GetNbinsX(); iBin++) {
	// 		// cout << h->GetBinContent(iBin) << endl;
	// 		double content = hist->GetBinContent(iBin);
	// 		double g = rand.Poisson(content);
	// 		// cout << "old: " << content << " new: " << g << endl;
	// 		hist->SetBinContent(iBin, g);
	// 	}
	// }
	file ->Close();
	return hist;
}

TH2F* HistHelper::Get2DHisto(TString name) {
	TH1::AddDirectory(kFALSE);

	TFile *file = new TFile(path.GetHistoFilePath(), "open");
	TH2F* hist = (TH2F*)file->Get(name);
	file ->Close();
	return hist;
}
std::vector<TH1*> HistHelper::getAllVariations(TString bkgname, std::vector<TString> variations) {

};