#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
#include "TUnfoldSys.h"
// #include "TMath.h"
#include <math.h>
#include "TSortedList.h"



#include "../interface/HistMaker.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/PathHelper.hpp"
#include "../interface/HistHelper.hpp"


using namespace std;
#ifndef Main_CPP_
#define Main_CPP_


template<typename T>
std::vector<T> to_array(const std::string & s)
{
	std::vector<T> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, ',')) result.push_back(boost::lexical_cast<T>(item));
	return result;
}

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
	// int nBins_Gen = pt.get<int>("Binning.nBins_Gen");
	int xMin_Gen = pt.get<int>("Binning.xMin_Gen");
	int xMax_Gen = pt.get<int>("Binning.xMax_Gen");
	std::vector<double> BinEdgesGen = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
	std::vector<double> BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesReco"));
	int nBins_Gen = BinEdgesGen.size() - 1;

	std::vector<string> bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));
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
	if (fillhistos) {
		//Fill Histos
		histomaker.MakeHistos();
	}
	//Reset Output ROOTFILE
	std::remove(path.GetOutputFilePath());

//Return relevant Histos

//Full Sample
	HistHelper histhelper;

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

	TH1F* fakes_Zjet = histhelper.Get1DHisto("fakes_Zjet");
	TH1F* fakes_Wjet = histhelper.Get1DHisto("fakes_Wjet");

	TH1F* fakes_Zjet_Split = histhelper.Get1DHisto("fakes_Zjet_Split");
	TH1F* fakes_Wjet_Split = histhelper.Get1DHisto("fakes_Wjet_Split");

	TH1F* TestMET_Zjet = histhelper.Get1DHisto("TestMETZjet");
	TH1F* TestMET_Wjet = histhelper.Get1DHisto("TestMETWjet");

	TH1F* TestMET_Zjet_Split = histhelper.Get1DHisto("TestMETZjet_Split");
	TH1F* TestMET_Wjet_Split = histhelper.Get1DHisto("TestMETWjet_Split");

	TH1F* SubTest_Zjet = histhelper.Get1DHisto("h_subTest_Zjet");
	TH1F* SubTest_Wjet = histhelper.Get1DHisto("h_subTest_Wjet");


	TH2F* A_Wjet = histhelper.Get2DHisto("A_Wjet");
	TH2F* A_Zjet = histhelper.Get2DHisto("A_Zjet");
	TH2F* A_Wjet_Split = histhelper.Get2DHisto("A_Wjet_Split");
	TH2F* A_Zjet_Split = histhelper.Get2DHisto("A_Zjet_Split");

	TH1F* GenMET_all = (TH1F*)GenMET_Zjet->Clone();
	GenMET_all->Add(GenMET_Wjet);

	TH1F* GenMET_all_Split = (TH1F*)GenMET_Zjet_Split->Clone();
	GenMET_all_Split->Add(GenMET_Wjet_Split);

	TH1F* MET_all = (TH1F*)MET_Zjet->Clone();
	MET_all->Add(MET_Wjet);

	TH1F* TestMET_all = (TH1F*)TestMET_Zjet->Clone();
	TestMET_all->Add(TestMET_Wjet);

	cout << "Gen integral: " << GenMET_all->Integral() << endl;
	cout << "Reco (passes GenSelection) integral: " << TestMET_all->Integral() << endl;

	TH1F* MET_all_Split = (TH1F*)MET_Wjet_Split->Clone();
	MET_all_Split->Add(MET_Zjet_Split);

	TH2F* A_all = (TH2F*)A_Zjet->Clone();
	A_all->Add(A_Wjet);
	float GenIntegral = 0;
	for (int i = 0; i < 25 ; i++) {
		GenIntegral += A_all->GetBinContent(0, i);
	}
	cout << "A underflow (0,all): " << GenIntegral << endl;
	TH2F* A_all_Split = (TH2F*)A_Zjet_Split->Clone();
	A_all_Split->Add(A_Wjet_Split);

	TH1F* fakes_all = (TH1F*) fakes_Zjet->Clone();
	fakes_all->Add(fakes_Wjet);
	cout << "Fake integral: " << fakes_all->Integral() << endl;

	TH1F* fakes_Split_all = (TH1F*) fakes_Zjet_Split->Clone();
	fakes_Split_all->Add(fakes_Wjet_Split);

	TH1F* h_DummyDataMinFakes = (TH1F*) MET_DummyData_all->Clone();
	h_DummyDataMinFakes->Add(fakes_Split_all, -1);

	TH1F* h_DataMinFakes = (TH1F*) MET_data->Clone();
	h_DataMinFakes->Add(fakes_all, -1);
	cout << "Data integral: " << MET_data->Integral() << endl;
	cout << "Data-Fake integral: " << h_DataMinFakes->Integral(h_DataMinFakes->GetXaxis()->FindBin(250), 25) << endl;

	std::vector<TH1*> v_MET_bkg_Split;
	v_MET_bkg_Split.push_back(MET_Zjet_Split);
	v_MET_bkg_Split.push_back(MET_Wjet_Split);

	std::vector<TH1*> v_MET_bkg;
	v_MET_bkg.push_back(MET_Zjet);
	v_MET_bkg.push_back(MET_Wjet);

	std::vector<TH1*> v_GenMET_bkg_Split;
	v_GenMET_bkg_Split.push_back(GenMET_Zjet_Split);
	v_GenMET_bkg_Split.push_back(GenMET_Wjet_Split);

	std::vector<TH1*> v_GenMET_bkg;
	v_GenMET_bkg.push_back(GenMET_Zjet);
	v_GenMET_bkg.push_back(GenMET_Wjet);
//Do Unfolding
//Split Input (e.g. only on MC)
	cout << "Unfolding using only MC with a split of " <<  split << ":" << endl;
	Unfolder Unfolder_Split;
	Unfolder_Split.ParseConfig();
	TUnfoldDensity* unfold_Split = Unfolder_Split.SetUp(A_all_Split, MET_DummyData_all);

	Unfolder_Split.SubBkg(unfold_Split, fakes_Split_all, "fakes_Split"); //subtract fakes

	unfold_Split->SetBias(GenMET_all_Split);

	std::tuple<int , TSpline*, TGraph*> TauResult_Split;
	TauResult_Split = Unfolder_Split.FindBestTau(unfold_Split);
	Unfolder_Split.VisualizeTau(TauResult_Split, "Split");

	std::tuple<TH1*, TH1*> unfold_output_Split;
	unfold_output_Split = Unfolder_Split.GetOutput(unfold_Split);
	// Unfolder_Split.GetRegMatrix(unfold_Split);

	TH2* ErrorMatrix_Split = unfold_Split->GetEmatrixTotal("ErrorMatrix_Split");
	TH1D* METTotalError_Split = new TH1D("TotalError_Split", ";MET", nBins_Gen, BinEdgesGen.data());
	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		METTotalError_Split->SetBinContent(bin, std::get<0>(unfold_output_Split)->GetBinContent(bin));
		METTotalError_Split->SetBinError(bin, sqrt(ErrorMatrix_Split->GetBinContent(bin, bin)));
	}
	TH2* L_Split = unfold_Split->GetL("L_Split");
	TH2* RhoTotal_Split = unfold_Split->GetRhoIJtotal("RhoTotal_Split");

//Data Input
	cout << "Unfolding using real Data:" << endl;
	Unfolder Unfolder;
	Unfolder.ParseConfig();
	TUnfoldDensity* unfold = Unfolder.SetUp(A_all, MET_data);

	Unfolder.SubBkg(unfold, fakes_all, "fakes"); //subtract fakes
	unfold->SetBias(GenMET_all);

	std::tuple<int , TSpline*, TGraph*> TauResult;
	TauResult = Unfolder.FindBestTau(unfold);
	Unfolder.VisualizeTau(TauResult, "data");

	//0st element=unfolded 1st=folded back
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);
	// Unfolder.GetRegMatrix(unfold);
	TH2* ErrorMatrix = unfold->GetEmatrixTotal("ErrorMatrix");
	TH1D* METTotalError = new TH1D("TotalError", ";MET", nBins_Gen, BinEdgesGen.data());
	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		METTotalError->SetBinContent(bin, std::get<0>(unfold_output)->GetBinContent(bin));
		METTotalError->SetBinError(bin, sqrt(ErrorMatrix->GetBinContent(bin, bin)));
	}
	TH2* L = unfold->GetL("L");
	TH2* RhoTotal = unfold_Split->GetRhoIJtotal("RhoTotal");




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
	Drawer.Draw1D(fakes_all, "fakes_all");
	Drawer.Draw1D(fakes_Split_all, "fakes_Split_all");
	Drawer.Draw1D(METTotalError, "MET_unfolded_errors");
	Drawer.Draw1D(METTotalError_Split, "MET_unfolded_errors_Split");




	Drawer.Draw2D(A_Wjet, "A_Wjet", true);
	Drawer.Draw2D(A_Zjet, "A_Zjet", true);
	Drawer.Draw2D(A_Wjet_Split, "A_Wjet_Split", true);
	Drawer.Draw2D(A_Zjet_Split, "A_Zjet_Split", true);
	Drawer.Draw2D(A_all, "A_all", true);
	Drawer.Draw2D(A_all_Split, "A_all_Split", true);
	Drawer.Draw2D(ErrorMatrix, "ErrorMatrix");
	Drawer.Draw2D(ErrorMatrix_Split, "ErrorMatrix_Split");
	Drawer.Draw2D(L, "L");
	Drawer.Draw2D(L_Split, "L_Split");
	Drawer.Draw2D(RhoTotal, "RhoTotal");
	Drawer.Draw2D(RhoTotal_Split, "RhoTotal_Split");




	bool log = true;
	Drawer.DrawDataMC(MET_data, v_MET_bkg, bkgnames, "MET", log);
	Drawer.DrawDataMC(MET_DummyData_all, v_MET_bkg_Split, bkgnames, "MET_Split", log);

//Output of Unfolding
	std::vector<string> GenBkgNames;
	for (const std::string& name : bkgnames) {
		GenBkgNames.push_back("Gen_" + name);
	}
//split Input
	Drawer.Draw1D(std::get<0>(unfold_output_Split), recovar + "_unfolded_Split");
	Drawer.Draw1D(std::get<1>(unfold_output_Split), recovar + "_foldedback_Split");

	Drawer.DrawRatio(std::get<0>(unfold_output_Split), GenMET_all_Split, "ratio_unfolded_Gen_Split", "unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output_Split), MET_DummyData_all, "ratio_foldedback_DummyData_Split", "foldedback/data");

	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkg_Split, GenBkgNames, "MET_UnfoldedvsGen_Split", log);
	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkg_Split, GenBkgNames, "MET_UnfoldedvsGen_normalized_Split", log);
	Drawer.DrawDataMC(h_DummyDataMinFakes, {std::get<1>(unfold_output_Split)},  {"FoldedBack"}, "MET_DummyDatavsFoldedBack_Split");

	Drawer.DrawDataMC(h_DummyDataMinFakes, {TestMET_Zjet_Split, TestMET_Wjet_Split}, bkgnames, "DummyDataMinFakesvsSubtestMET" , log);


//Using Data
	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "_unfolded");
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "_foldedback");
	Drawer.DrawRatio(std::get<0>(unfold_output), GenMET_all, "ratio_unfolded_Gen", "unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output), MET_data, "ratio_foldedback_data", "foldedback/data");

	Drawer.DrawDataMC(std::get<0>(unfold_output), v_GenMET_bkg, GenBkgNames, "MET_UnfoldedvsGen", log);
	Drawer.DrawDataMC(METTotalError, v_GenMET_bkg, GenBkgNames, "MET_UnfoldedvsGen_normalized", log, true);
	Drawer.DrawDataMC(h_DataMinFakes, {std::get<1>(unfold_output)},  {"FoldedBack"}, "MET_DatavsFoldedBack", log);


	Drawer.Draw1D(h_DataMinFakes, "DataMinFakes");
	Drawer.DrawDataMC(h_DataMinFakes, {TestMET_Zjet, TestMET_Wjet}, bkgnames, "DataMinFakesvsSubtestMET", log);








	return (0);
}

#endif