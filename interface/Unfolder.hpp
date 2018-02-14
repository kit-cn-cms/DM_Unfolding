#ifndef Unfolder_HPP_
#define Unfolder_HPP_


#include "TUnfoldDensity.h"
#include "TSpline.h"
#include "TGraph.h"
#include "../interface/PathHelper.hpp"



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
	void GetRegMatrix(TUnfoldDensity* unfold);
	void SubBkg(TUnfoldDensity* unfold, TH1* h_bkg, TString name);





	PathHelper path;
	float biasScale;
	int nScan;
	double tauMin;
	double tauMax;
};

#endif