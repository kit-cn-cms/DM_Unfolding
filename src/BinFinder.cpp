#include "../interface/BinFinder.hpp"
#include <iostream>
#include <cmath>
#include "TFile.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
#include "TF1.h"

using namespace std;

template<typename T>
std::vector<T>
to_array(const std::string& s)
{
	std::vector<T> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, ','))
		result.push_back(boost::lexical_cast<T>(item));
	return result;
}

BinFinder::BinFinder(TH2* Matrix) {
	matrix = Matrix;
	double nBinsx = matrix->GetNbinsX();
	double nBinsy = matrix->GetNbinsY();
	if (nBinsx != nBinsy) cout << "WARNING Matrix not square!" << endl;
	nBins = nBinsx;

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(string(path.GetConfigPath()), pt);
	std::vector<double> BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesReco"));

	xMin = BinEdgesReco[2];
	xMax = BinEdgesReco[3];
	xMin = 250;
	xMax = 2000;
	rounding = pt.get<int>("Binning.rounding");
}

TH1* BinFinder::GetSlice(int xbin) {
	TH1F* slice = new TH1F("slice", "slice", nBins, xMin, xMax);
	for (Int_t ybin = 1; ybin <= nBins; ybin++) {
		slice->SetBinContent(ybin, matrix->GetBinContent(xbin, ybin));
	}
	slice->Sumw2();
	return slice;

}

std::vector<TH1*> BinFinder::GetSlices() {
	std::vector<TH1*> slices;
	for (Int_t bin = 1; bin <= nBins; bin++) {
		slices.push_back(GetSlice(bin));
	}
	return slices;
}

std::vector<double> BinFinder::FitSlices(std::vector<TH1*> slices) {
	std::vector<double> sigmas;
	TFile *output = new TFile(path.GetOutputFilePath(), "update");
	int bin = 1;
	double sigma = 0;
	for (auto& slice : slices) {
		TString name = "bin_" + TString(Form("%d", bin));
		TCanvas* c = new TCanvas(name, name, 1024, 1024);
		slice->Fit("gaus");
		slice->Draw("");
		TF1 *fit = slice->GetFunction("gaus");
		sigmas.push_back(fit->GetParameter(2) * 2);
		// cout << floor(sigma/10+0.5)*10 << endl;
		// bins.push_back();
		c->SaveAs(path.GetPdfPath() + "/bins/" + name + ".pdf");
		c->SaveAs(path.GetPdfPath() + "../pngs/bins/" + name + ".png");
		c->Write();
		delete c;
		bin++;

	}
	output->Close();
	return sigmas;
}

std::vector<int> BinFinder::CalculateBinEdges(std::vector<double> sigmas) {
	std::vector<int> edges;
	std::vector<int> roundedsigmas;
	std::vector<int> diffs;
	for (auto& sigma : sigmas) {
		roundedsigmas.push_back(floor(sigma / rounding + 0.5)*rounding);
	}
	edges.push_back(xMin);
	edges.push_back(xMin + roundedsigmas.at(0));
	int tmp = roundedsigmas.at(0);
	int prevsig = roundedsigmas.at(0);
	for (unsigned int i = 1; i <= roundedsigmas.size(); i++) {
		if (roundedsigmas.at(i) < tmp) tmp = prevsig;
		else tmp = roundedsigmas.at(i);
		i += roundedsigmas.at(i) / rounding;
		prevsig = tmp;
		diffs.push_back(tmp);
	}
	int i = 1;
	for (auto& diff : diffs) {
		edges.push_back(diff + edges.at(i));
		i++;
	}
	cout << "Following BinEdges are found: " << endl;
	for (auto& edge : edges) cout << edge << ",";
	cout  << endl;
	return edges;
}

std::vector<int> BinFinder::GetBins() {
	std::vector<TH1*> slices = GetSlices();
	std::vector<double> sigmas = FitSlices(slices);
	std::vector<int> edges = CalculateBinEdges(sigmas);
	return edges;
}