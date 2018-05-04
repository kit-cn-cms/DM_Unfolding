#ifndef HistMaker_HPP_
#define HistMaker_HPP_



#include "TFile.h"
#include "TH1F.h"
#include "TChain.h"
#include "TString.h"
#include "TH2F.h"
#include "../interface/PathHelper.hpp"
#include "../interface/HistHelper.hpp"




class HistMaker
{
public:
	std::vector<TString> GetInputFileList(std::vector<std::string> paths , TString type);
	void ParseConfig();
	void FillHistos(std::vector<TChain*> SignalChains, std::vector<TChain*> DataChains, std::vector<std::vector<TChain*>> BkgChainsVariations);
	void MakeHistos();
	TChain* ChainFiles(std::vector<TString> filelist);
	TTree* CreateFriendTree(std::vector<std::string> BranchNamesLong, std::vector<std::string> BranchNamesFLoat, long n_Events, TString label);


	TH1F* Get1DHisto(TString name);
	TH2F* Get2DHisto(TString name);


	PathHelper path;
	HistHelper histhelper;
	TFile* histos;
	TString genvar;
	TString recovar;
	std::string samplepath;
	std::vector<std::string> SignalPath;
	std::vector<std::string> DataPath;
	std::vector<std::string> bkgnames;
	std::map<std::string, std::vector<std::string>> BkgPaths;
	std::vector<std::string> weights;
	std::vector<std::string> additionalBranchNamesLong;
	std::vector<std::string> additionalBranchNamesFloat;
	std::vector<std::string> LHAFixBranchesLong;
	std::vector<std::string> LHAFixBranchesFloat;

	std::vector<std::string> variation;
	int nBins_Gen;
	int nBins_Reco;
	int xMin;
	int xMax;
	int split;
	double nMax;
	bool splitSignal;
	bool useBatch;
};

#endif