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
	std::vector<TH1*> getAllVariations(TString bkgname, std::vector<TString> variations);



	PathHelper path;
	TFile* histos;

};

#endif