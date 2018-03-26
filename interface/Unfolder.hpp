#ifndef Unfolder_HPP_
#define Unfolder_HPP_


#include "TUnfoldDensity.h"
#include "TSpline.h"
#include "TGraph.h"
#include "../interface/PathHelper.hpp"



class Unfolder
{
public:
	void ParseConfig();
	TUnfoldDensity* SetUp(TH2* A, TH1F* input);
	std::tuple<int, TSpline*, TGraph*>  FindBestTau(TUnfoldDensity* unfold, TString name, const char * distribution= 0, const char * axisSteering = 0);
	std::tuple<int, TSpline*, TGraph*>  FindBestTauLcurve(TUnfoldDensity* unfold, TString name);

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