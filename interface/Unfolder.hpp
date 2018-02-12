#include <iostream>
#include <tuple>
#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TUnfoldDensity.h"
#include "TSpline.h"
#include "TGraph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>




using namespace std;


class Unfolder
{
public:
	TUnfoldDensity* Unfold(TH2F* A, TH1F* input);
	void ParseConfig();
	TUnfoldDensity* SetUp(TH2F* A, TH1F* input);
	std::tuple<int, TSpline*, TGraph*>  FindBestTau(TUnfoldDensity* unfold);
	void VisualizeTau(std::tuple<int, TSpline* , TGraph* > tuple);
	void DoUnfolding(TUnfoldDensity* unfold, TH1F* h_Data);
	std::tuple<TH1*, TH1*> GetOutput(TUnfoldDensity* unfold);




	float biasScale;
	int nScan;
	double tauMin;
	double tauMax;
};