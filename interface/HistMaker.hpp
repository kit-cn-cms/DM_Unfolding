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
	std::vector<TString> GetInputFileList(TString path , TString type);
	void SetUpHistos();
	void ParseConfig();
	void FillHistos(TChain* MCChain, TChain* DataChain);
	void MakeHistos();
	TChain* ChainFiles(std::vector<TString> filelist);
	TH1F* Get1DHisto(TString name);
	TH2F* Get2DHisto(TString name);


	PathHelper* path;
	TFile* histos;
	TString genvar;
	TString recovar;
	TString MCPath;
	TString DataPath;
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