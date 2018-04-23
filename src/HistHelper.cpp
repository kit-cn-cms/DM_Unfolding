#include "HistHelper.hpp"



TH1F* HistHelper::Get1DHisto(TString name) {
	TH1::AddDirectory(kFALSE);

	TFile *file = new TFile(path.GetHistoFilePath(), "open");
	TH1F* hist = (TH1F*)file->Get(name);
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