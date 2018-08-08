#ifndef HistHelper_HPP_
#define HistHelper_HPP_



#include "TFile.h"
#include "TH1F.h"
#include "TChain.h"
#include "TString.h"
#include "TH2F.h"
#include "PathHelper.hpp"



class HistHelper
{
public:
	TH1F* Get1DHisto(TString name);
	TH2F* Get2DHisto(TString name);
	std::vector<std::vector<TH1*>> getAllVariations(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations);
	std::vector<std::vector<TH2*>> getAllVariations2D(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations);
	std::vector<TH1F*> AddAllBkgs(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations);
	std::vector<TH2*> AddAllBkgs2D(std::vector<std::string> bkgnames, TString varname, std::vector<TString> variations);



	PathHelper path;
	TFile* histos;

};

#endif