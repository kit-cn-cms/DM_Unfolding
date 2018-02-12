#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
// #include "TUnfold.h"
#include "TUnfoldDensity.h"
#include "TH2D.h"
#include "TSpline.h"
#include "TGraph.h"
#include "math.h"
#include "TStyle.h"
#include "TChain.h"
#include "TString.h"
// #include "gstyle.h"
// #include <sys/stat.h>
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
	void FillHistos(TChain* chain);
	void MakeHistos();
	TChain* ChainFiles();
	TH1F* GetHisto(TString name);


	TFile* histos;
	TString genvar;
	TString recovar;
	TString path;
	TString variation;
	int nBins_Gen;
	int nBins_Reco;
	int xMin;
	int xMax;
	double nMax;
	std::vector<TString> filelist;

	TH1F h_Gen;
	TH1F h_Reco;
};

