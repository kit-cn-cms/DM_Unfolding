#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TChain.h"
#include "TString.h"
#include <dirent.h>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>



using namespace std;




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

