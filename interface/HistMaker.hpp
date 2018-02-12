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
<<<<<<< HEAD
	void SetUpHistos();
	void ParseConfig();
	void FillHistos(TChain* chain);
	void MakeHistos();
	TChain* ChainFiles();
	TH1F* GetHisto(TString name);
=======
	TFile* SetUpHistos();
	void ParseConfig();
	void FillHistos(TChain* chain, TFile* histos);
	void MakeHistos();
	TChain* ChainFiles();
>>>>>>> de19a9a899ab5752a1c8c7351fe4f77f8e0dbae7


	TFile* histos;
	TString genvar;
	TString recovar;
	TString path;
	TString variation;
<<<<<<< HEAD
	int nBins_Gen;
=======
	int nBins_Gen; 
>>>>>>> de19a9a899ab5752a1c8c7351fe4f77f8e0dbae7
	int nBins_Reco;
	int xMin;
	int xMax;
	double nMax;
	std::vector<TString> filelist;

	TH1F h_Gen;
	TH1F h_Reco;
<<<<<<< HEAD
=======




>>>>>>> de19a9a899ab5752a1c8c7351fe4f77f8e0dbae7
};

