#ifndef HistMaker_HPP_
#define HistMaker_HPP_



#include "TFile.h"
#include "TH1F.h"
#include "TChain.h"
#include "TString.h"
#include "TH2F.h"
#include "../interface/PathHelper.hpp"



class HistMaker
{
public:
	std::vector<TString> GetInputFileList(std::vector<std::string> paths , TString type);
	void SetUpHistos();
	void ParseConfig();
	void FillHistos(TChain * MCChain, TChain * DataChain, std::map<std::string, TChain*> BkgChains);
	void MakeHistos();
	TChain* ChainFiles(std::vector<TString> filelist);
	TH1F* Get1DHisto(TString name);
	TH2F* Get2DHisto(TString name);


	PathHelper path;
	TFile* histos;
	TString genvar;
	TString recovar;
	std::vector<std::string> MCPath;
	std::vector<std::string> DataPath;
	std::vector<std::string> bkgnames;
	std::map<std::string, std::vector<std::string>> BkgPaths;
	std::map<std::string, std::vector<TString>> BkgFilelists;
	std::map<std::string, TChain*> BkgChains;
	std::vector<std::string> weights;


	TString variation;
	int nBins_Gen;
	int nBins_Reco;
	int xMin;
	int xMax;
	int split;
	double nMax;
	bool useData;
};

#endif