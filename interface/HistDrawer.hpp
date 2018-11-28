#ifndef HistDrawer_HPP_
#define HistDrawer_HPP_


#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"

#include "../interface/PathHelper.hpp"

class HistDrawer
{
public:
	void Draw1D(TH1* hist, TString name, bool log = false, TString xlabel = "none", TString ylabel = "# Events");
	void Draw2D(TH2 * hist, TString name, bool log = false, bool moveUF = false, TString xlabel = "none", TString ylabel = "none", TString zlabel = "none");
	void DrawRatio(TH1* hist1, TH1* hist2, TString name, TString xlabel = "none", TString ylabel = "Ratio");
	void DrawStack(std::vector<TH1*> MC, std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap, TString name, bool log = false, bool normalize = false, TString xlabel = "none", TString ylabel = " Events");

	void DrawDataMC(TH1* data, std::vector<TH1*> MC, std::vector<std::string> names, TString name, bool log = false, bool normalize = false, bool drawpull = false, TString xlabel = "none", TString ylabel = " Events");
	void DrawDataMC(TH1* data, std::vector<TH1*> MC, std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap, TString name, bool log = false, bool normalize = false, bool drawpull = false, TString xlabel = "none", TString ylabel = "# Events");
	void DrawDataMCerror(TGraphAsymmErrors* dataTGraph, std::vector<TH1*> MC, std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap, TString name, bool log = false, bool normalize = false, bool drawpull = false, TString xlabel = "none", TString ylabel = "# Events");

	void SetHistoStyle(TH1 * histo, int color, bool filled = false);
	TCanvas* getCanvas(TString name, bool ratiopad = false, bool pullpad = false);

	TLegend* getLegend();
	void DrawLumiLabel(TCanvas* canvas);


	PathHelper path;


};

#endif