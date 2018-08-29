#ifndef UnfoldWrapper_HPP_
#define UnfoldWrapper_HPP_

#include <TString.h>
#include <TH2.h>
#include <TH1.h>
#include "TUnfoldDensity.h"
#include "FileWriter.hpp"





class UnfoldWrapper
{
public:
	UnfoldWrapper(TString varName, TString label, std::vector<TH2*> A, TH1F* data, std::vector<TH1F*> fakes, std::vector<TH1*> misses, std::vector<std::vector<TH1*>> MC, std::vector<std::vector<TH1*>> GenMC, std::vector<std::string> variations, std::vector<std::string> bkgnames,std::vector<double> BinEdgesGen);
	void DoIt();
	// ~UnfoldWrapper();
	FileWriter writer;

	TString varName;
	TString label;
	std::vector<TH2*> A;
	std::vector<TH1*> misses;
	TH1F* data;
	std::vector<TH1F*> fakes;
	std::vector<std::string> variations;
	std::vector<std::string> bkgnames;
	std::vector<std::vector<TH1*>> MC;
	std::vector<std::vector<TH1*>> GenMC;
	std::vector<double> BinEdgesGen;


};


#endif

