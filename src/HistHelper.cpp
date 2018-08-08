#include "HistHelper.hpp"
#include <TRandom3.h>
#include <iostream>



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

std::vector<std::vector<TH1*>> HistHelper::getAllVariations(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations) {
	std::vector<std::vector<TH1*>> v;
	variations.insert(variations.begin(), "");
	std::cout << "#### Getting Histograms for varname: " << varname << " ####" << std::endl;

	for (auto& var : variations) {
		std::vector<TH1*> vtemp;
		for (auto& bkgname : bkgnames) {
			if (var != "") {
				auto tmphist = Get1DHisto( bkgname + "_" + varname + "_" + var);
				if (tmphist != nullptr) vtemp.push_back(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname + "_" + var << std::endl;
			}
			else {
				auto tmphist = Get1DHisto( bkgname + "_" + varname);
				if (tmphist != nullptr) vtemp.push_back(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname << std::endl;
			}
		}
		v.push_back(vtemp);
		vtemp.clear();
	}
	return v;
};

std::vector<std::vector<TH2*>> HistHelper::getAllVariations2D(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations) {
	std::vector<std::vector<TH2*>> v;
	variations.insert(variations.begin(), "");
	std::cout << "#### Getting Histograms for varname: " << varname << " ####" << std::endl;

	for (auto& var : variations) {
		std::vector<TH2*> vtemp;
		for (auto& bkgname : bkgnames) {
			if (var != "") {
				auto tmphist = Get2DHisto( bkgname + "_" + varname + "_" + var);
				if (tmphist != nullptr) vtemp.push_back(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname + "_" + var << std::endl;
			}
			else {
				auto tmphist = Get2DHisto( bkgname + "_" + varname);
				if (tmphist != nullptr) vtemp.push_back(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname << std::endl;
			}
		}
		v.push_back(vtemp);
		vtemp.clear();
	}
	return v;
};

std::vector<TH1F*> HistHelper::AddAllBkgs(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations) {
	std::vector<TH1F*> v;
	variations.insert(variations.begin(), "");
	std::cout << "#### adding up histos for varname: " << varname << " ####" << std::endl;

	auto addedhist = (TH1F*) Get1DHisto( bkgnames.at(0) + "_" + varname)->Clone();
	if (addedhist == nullptr) std::cout << "couldn't find nominal template: " << bkgnames.at(0) + "_" + varname << std::endl;

	addedhist->Reset();
	//sys variations
	for (auto& var : variations) {
		for (auto bkgname : bkgnames) {
			if (var != "") {
				auto tmphist = Get1DHisto( bkgname + "_" + varname + "_" + var);
				if (tmphist != nullptr) addedhist->Add(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname + "_" + var << std::endl;
			}
			else {
				auto tmphist = Get1DHisto( bkgname + "_" + varname);
				if (tmphist != nullptr) addedhist->Add(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname << std::endl;
			}
		}
		addedhist->SetName(varname + "_" + var + "_all");
		v.push_back((TH1F*) addedhist->Clone());
		addedhist->Reset();
	}

	return v;
};

std::vector<TH2*> HistHelper::AddAllBkgs2D(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations) {
	std::vector<TH2*> v;
	variations.insert(variations.begin(), "");
	std::cout << "#### adding up histos for varname: " << varname << " ####" << std::endl;

	auto addedhist = (TH2*) Get2DHisto( bkgnames.at(0) + "_" + varname)->Clone();
	if (addedhist == nullptr) std::cout << "couldn't find nominal template: " << bkgnames.at(0) + "_" + varname << std::endl;

	addedhist->Reset();

	//sys variations
	for (auto& var : variations) {
		for (auto bkgname : bkgnames) {
			if (var != "") {
				auto tmphist = Get2DHisto( bkgname + "_" + varname + "_" + var);
				if (tmphist != nullptr) addedhist->Add(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname + "_" + var << std::endl;
			}
			else {
				auto tmphist = Get2DHisto( bkgname + "_" + varname);
				if (tmphist != nullptr) addedhist->Add(tmphist);
				else std::cout << "couldn't find " << bkgname + "_" + varname << std::endl;
			}
		}
		addedhist->SetName(varname + "_" + var + "_all");
		v.push_back((TH2*) addedhist->Clone());
		addedhist->Reset();
	}

	return v;
};