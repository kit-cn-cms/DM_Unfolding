#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


#include "../interface/HistMaker.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/PathHelper.hpp"
#include "../interface/HistHelper.hpp"


using namespace std;
#ifndef Main_CPP_
#define Main_CPP_

int main(int argc, char** argv) {



	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);
	PathHelper path;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(string(path.GetConfigPath()), pt);
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");
	int split = pt.get<int>("general.split");
	// pt.put("general.fillhistos",false);
	bool fillhistos = true;

// Fill Histos?
	char c;

	std::ifstream histfile(path.GetHistoFilePath());
	if (histfile.good()) {
		cout << "Histo ROOT file found here: " << path.GetHistoFilePath() << endl;
		cout << "Do you want to refill the Histos? (y/n)" << endl;
		cin >> c;
		if (c == 'y') fillhistos = true;
		else fillhistos = false;
	}


	HistMaker histomaker;
	HistHelper histhelper;
	if (fillhistos) {
		histomaker.MakeHistos();
	}

//Return relevant Histos

//Full Sample
	TH1F* MET_data = histhelper.Get1DHisto("Evt_Pt_MET_data");
	TH1F* MET_DummyData_Wjet = histhelper.Get1DHisto("DummyData_Wjet_Split");
	TH1F* MET_DummyData_Zjet = histhelper.Get1DHisto("DummyData_Zjet_Split");
	TH1F* MET_DummyData_all = (TH1F*) MET_DummyData_Wjet->Clone();
	MET_DummyData_all->Add(MET_DummyData_Zjet);

	TH1F* GenMET_signal = histhelper.Get1DHisto("Evt_Pt_GenMET_signal");
	TH1F* MET_signal = histhelper.Get1DHisto("Evt_Pt_MET_signal");

	TH1F* MET_Wjet = histhelper.Get1DHisto("Evt_Pt_MET_Wjet");
	TH1F* GenMET_Wjet = histhelper.Get1DHisto("Evt_Pt_GenMET_Wjet");
	TH1F* MET_Wjet_Split = histhelper.Get1DHisto("Evt_Pt_MET_Wjet_Split");
	TH1F* GenMET_Wjet_Split = histhelper.Get1DHisto("Evt_Pt_GenMET_Wjet_Split");

	TH1F* MET_Zjet = histhelper.Get1DHisto("Evt_Pt_MET_Zjet");
	TH1F* GenMET_Zjet = histhelper.Get1DHisto("Evt_Pt_GenMET_Zjet");
	TH1F* MET_Zjet_Split = histhelper.Get1DHisto("Evt_Pt_MET_Zjet_Split");
	TH1F* GenMET_Zjet_Split = histhelper.Get1DHisto("Evt_Pt_GenMET_Zjet_Split");

	TH2F* A_Wjet = histhelper.Get2DHisto("A_Wjet");
	TH2F* A_Zjet = histhelper.Get2DHisto("A_Zjet");
	TH2F* A_Wjet_Split = histhelper.Get2DHisto("A_Wjet_Split");
	TH2F* A_Zjet_Split = histhelper.Get2DHisto("A_Zjet_Split");

	TH1F* GenMET_all = (TH1F*)GenMET_Wjet->Clone();
	GenMET_all->Add(GenMET_Zjet);
	TH1F* GenMET_all_Split = (TH1F*)GenMET_Wjet_Split->Clone();
	GenMET_all_Split->Add(GenMET_Zjet_Split);


	TH1F* MET_all = (TH1F*)MET_Wjet->Clone();
	MET_all->Add(MET_Zjet);
	TH1F* MET_all_Split = (TH1F*)MET_Wjet_Split->Clone();
	MET_all_Split->Add(MET_Zjet_Split);

	TH2F* A_all = (TH2F*)A_Wjet->Clone();
	A_all->Add(A_Zjet);
	TH2F* A_all_Split = (TH2F*)A_Wjet_Split->Clone();
	A_all_Split->Add(A_Zjet_Split);
//Do Unfolding
//Split Input (e.g. only on MC)
	cout << "Unfolding using only MC with a split of " <<  split << ":" << endl;
	Unfolder Unfolder_Split;
	Unfolder_Split.ParseConfig();
	TUnfoldDensity* unfold_Split = Unfolder_Split.SetUp(A_all_Split, MET_DummyData_all);

	std::tuple<int , TSpline*, TGraph*> TauResult_Split;
	TauResult_Split = Unfolder_Split.FindBestTau(unfold_Split);
	Unfolder_Split.VisualizeTau(TauResult_Split, "Split");
	Unfolder_Split.DoUnfolding(unfold_Split, MET_DummyData_all);

	std::tuple<TH1*, TH1*> unfold_output_Split;
	unfold_output_Split = Unfolder_Split.GetOutput(unfold_Split);
	Unfolder_Split.GetRegMatrix(unfold_Split);

//Data Input
	cout << "Unfolding using real Data:" << endl;
	Unfolder Unfolder;
	Unfolder.ParseConfig();
	TUnfoldDensity* unfold = Unfolder.SetUp(A_all, MET_data);

	std::tuple<int , TSpline*, TGraph*> TauResult;
	TauResult = Unfolder.FindBestTau(unfold);
	Unfolder.VisualizeTau(TauResult, "data");
	Unfolder.DoUnfolding(unfold, MET_data);

	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);
	Unfolder.GetRegMatrix(unfold);

// Draw Distributions
//General Distributions
	HistDrawer Drawer;
	Drawer.Draw1D(MET_Wjet, "MET_Wjet");
	Drawer.Draw1D(GenMET_Wjet, "GenMET_Wjet");
	Drawer.Draw1D(MET_Wjet_Split, "MET_Wjet_Split");
	Drawer.Draw1D(GenMET_Wjet_Split, "GenMET_Wjet_Split");

	Drawer.Draw1D(MET_Zjet, "MET_Zjet");
	Drawer.Draw1D(GenMET_Zjet, "GenMET_Zjet");
	Drawer.Draw1D(MET_Zjet_Split, "MET_Zjet_Split");
	Drawer.Draw1D(GenMET_Zjet_Split, "GenMET_Zjet_Split");

	Drawer.Draw1D(MET_all, "MET_all");
	Drawer.Draw1D(MET_all_Split, "MET_all_Split");
	Drawer.Draw1D(GenMET_all, "GenMET_all");
	Drawer.Draw1D(GenMET_all_Split, "GenMET_all_Split");
	Drawer.Draw1D(MET_data, "MET_data");

	Drawer.Draw2D(A_Wjet, "A_Wjet");
	Drawer.Draw2D(A_Zjet, "A_Zjet");
	Drawer.Draw2D(A_Wjet_Split, "A_Wjet_Split");
	Drawer.Draw2D(A_Zjet_Split, "A_Zjet_Split");
	Drawer.Draw2D(A_all, "A_all");
	Drawer.Draw2D(A_all_Split, "A_all_Split");

	std::vector<TH1F*> MET_bkg;
	MET_bkg.push_back(MET_Wjet);
	MET_bkg.push_back(MET_Zjet);
	Drawer.DrawDataMC(MET_data, MET_bkg, "MET");

//Output of Unfolding
//split Input
	Drawer.Draw1D(std::get<0>(unfold_output_Split), recovar + "_unfolded_Split");
	Drawer.Draw1D(std::get<1>(unfold_output_Split), recovar + "_foldedback_Split");
	Drawer.DrawRatio(std::get<0>(unfold_output_Split), GenMET_all_Split, "ratio_unfolded_Gen_Split", "unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output_Split), MET_DummyData_all, "ratio_foldedback_DummyData_Split", "foldedback/data");
//Using Data
	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "_unfolded");
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "_foldedback");
	Drawer.DrawRatio(std::get<0>(unfold_output), GenMET_all, "ratio_unfolded_Gen", "unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output), MET_data, "ratio_foldedback_data", "foldedback/data");



	return (0);
}

#endif