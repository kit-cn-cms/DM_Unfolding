#ifndef HistDrawer_HPP_
#define HistDrawer_HPP_


#include "TH1F.h"
#include "TH2F.h"

#include "../interface/PathHelper.hpp"

class HistDrawer
{
public:
	void Draw1D(TH1* hist, TString name, TString xlabel = "none", TString ylabel = "# Events");
	void Draw2D(TH2 * hist, TString name, TString xlabel = "none", TString ylabel = "none");
	void DrawRatio(TH1* hist1, TH1* hist2, TString name, TString xlabel = "none", TString ylabel = "Ratio");
	void DrawDataMC(TH1F* data, std::vector<TH1F*> MC, TString name, TString xlabel = "none", TString ylabel = "# Events");
	void SetHistoStyle(TH1F* histo, int color, bool filled=false);





	PathHelper path;


};

#endif