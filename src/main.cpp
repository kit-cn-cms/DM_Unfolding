#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


#include "../interface/HistMaker.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"


using namespace std;


int main()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);

// Fill Histos
	bool fillhistos = pt.get<bool>("general.fillhistos");
	HistMaker histomaker;

	if (fillhistos) {
		histomaker.MakeHistos();
	}
//Return relevant Histos
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");
	TH2F* A = histomaker.Get2DHisto("A");
	TH1F* data = histomaker.Get1DHisto("Data");
	TH1F* gen = histomaker.Get1DHisto(genvar);
	TH1F* reco = histomaker.Get1DHisto(recovar);
	TH1F* wjets = histomaker.Get1DHisto("Wjet");
	TH1F* zjets = histomaker.Get1DHisto("Zjet");

//Do Unfolding
	bool useData = pt.get<bool>("general.useData");
	Unfolder Unfolder;
	Unfolder.ParseConfig();
	TUnfoldDensity* unfold = Unfolder.SetUp(A, data);
	//Subtract BackGrounds
	if (useData) {
		Unfolder.SubBkg(unfold, wjets, "Wjet");
		Unfolder.SubBkg(unfold, zjets, "Zjet");
	}

	std::tuple<int , TSpline*, TGraph*> TauResult;
	TauResult = Unfolder.FindBestTau(unfold);
	Unfolder.VisualizeTau(TauResult);
	Unfolder.DoUnfolding(unfold, data);


	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);
	Unfolder.GetRegMatrix(unfold);

// Draw Distributions
	HistDrawer Drawer;
	Drawer.Draw1D(reco, recovar);
	Drawer.Draw1D(wjets, "Wjets_MET");
	Drawer.Draw1D(zjets, "Zjets_MET");
	Drawer.Draw1D(gen, genvar);
	Drawer.Draw1D(data, "Data");

	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "unfolded");
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "foldedback");
	Drawer.Draw2D(A, "A");
	Drawer.DrawRatio(std::get<0>(unfold_output), gen, "ratio_unfolded_Gen", "unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output), data, "ratio_foldedback_data", "foldedback/data");



	return (0);
}