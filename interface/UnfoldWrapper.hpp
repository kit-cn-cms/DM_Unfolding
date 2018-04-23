#ifndef UnfoldWrapper_HPP_
#define UnfoldWrapper_HPP_

#include <TString.h>
#include <TH2.h>
#include <TH1.h>
#include "TUnfoldDensity.h"




class UnfoldWrapper
{
public:
	UnfoldWrapper(TString varName, TString label, std::vector<TH2*> A, TH1F* data, TH1* fakes, std::vector<std::vector<TH1*>> MC, std::vector<std::vector<TH1*>> GenMC, std::vector<std::string> variations, std::vector<std::string> bkgnames,std::vector<double> BinEdgesGen);
	void DoIt();
	// ~UnfoldWrapper();

	TString varName;
	TString label;
	std::vector<TH2*> A;
	TH1F* data;
	TH1* fakes;
	std::vector<std::string> variations;
	std::vector<std::string> bkgnames;
	std::vector<std::vector<TH1*>> MC;
	std::vector<std::vector<TH1*>> GenMC;
	std::vector<double> BinEdgesGen;

};


#endif

