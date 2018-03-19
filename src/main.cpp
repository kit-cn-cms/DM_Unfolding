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
#include "TGraphErrors.h"



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
	HistDrawer Drawer;

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
	//Data
	TH1F* MET_data = histhelper.Get1DHisto("Evt_Pt_MET_data");
	//Backgrounds
	std::vector<TH1*> v_MET_bkgs;
	TH1F* MET_all = (TH1F*) histhelper.Get1DHisto(recovar + "_" + bkgnames.at(0))->Clone();
	MET_all->Reset();

	std::vector<TH1*> v_GenMET_bkgs;
	TH1F* GenMET_all = (TH1F*) histhelper.Get1DHisto(genvar + "_" + bkgnames.at(0))->Clone();
	GenMET_all->Reset();

	std::vector<TH1*> v_fakes_bkgs;
	TH1F* fakes_all = (TH1F*)histhelper.Get1DHisto("fakes_" + bkgnames.at(0));
	fakes_all->Reset();

	std::vector<TH1*> v_testmet_bkgs;
	TH1F* TestMET_all = (TH1F*) histhelper.Get1DHisto("TestMET" + bkgnames.at(0))->Clone();
	TestMET_all->Reset();

	std::vector<TH2F*> v_A_bkgs;
	TH2F* A_all = (TH2F*)histhelper.Get2DHisto("A_" + bkgnames.at(0))->Clone();
	A_all->Reset();


	//PseudoData
	std::vector<TH1*> v_MET_DummyDatas;
	TH1F* MET_DummyData_all = (TH1F*) histhelper.Get1DHisto("DummyData_" + bkgnames.at(0) + "_Split")->Clone();
	MET_DummyData_all->Reset();

	std::vector<TH1*> v_MET_bkgs_Split;
	TH1F* MET_all_Split = (TH1F*) histhelper.Get1DHisto(recovar + "_" + bkgnames.at(0) + "_Split")->Clone();
	MET_all_Split->Reset();

	std::vector<TH1*> v_GenMET_bkgs_Split;
	TH1F* GenMET_all_Split = (TH1F*) histhelper.Get1DHisto(genvar + "_" + bkgnames.at(0) + "_Split")->Clone();
	GenMET_all_Split->Reset();

	std::vector<TH1*> v_fakes_bkgs_Split;
	TH1F* fakes_all_Split = (TH1F*)histhelper.Get1DHisto("fakes_" + bkgnames.at(0) + "_Split");
	fakes_all_Split->Reset();

	std::vector<TH1*> v_testmet_bkgs_Split;
	TH1F* TestMET_all_Split = (TH1F*) histhelper.Get1DHisto("TestMET" + bkgnames.at(0) + "_Split")->Clone();
	TestMET_all_Split->Reset();

	std::vector<TH2F*> v_A_bkgs_Split;
	TH2F* A_all_Split = (TH2F*)histhelper.Get2DHisto("A_" + bkgnames.at(0) + "_Split")->Clone();
	A_all_Split->Reset();

	for (const auto& name : bkgnames) {
		v_MET_bkgs.push_back(histhelper.Get1DHisto(recovar + "_" + name));

		v_GenMET_bkgs.push_back(histhelper.Get1DHisto(genvar + "_" + name));
		GenMET_all->Add(histhelper.Get1DHisto(genvar + "_" + name));

		v_fakes_bkgs.push_back(histhelper.Get1DHisto("fakes_" + name));
		fakes_all->Add(histhelper.Get1DHisto("fakes_" + name));

		v_testmet_bkgs.push_back(histhelper.Get1DHisto("TestMET" + name));
		TestMET_all->Add(histhelper.Get1DHisto("TestMET" + name));

		v_A_bkgs.push_back(histhelper.Get2DHisto("A_" + name));
		A_all->Add(histhelper.Get2DHisto("A_" + name));



		v_MET_DummyDatas.push_back(histhelper.Get1DHisto("DummyData_" + name + "_Split"));
		MET_DummyData_all->Add(histhelper.Get1DHisto("DummyData_" + name + "_Split"));

		v_MET_bkgs_Split.push_back(histhelper.Get1DHisto(recovar + "_" + name + "_Split"));
		MET_all_Split->Add(histhelper.Get1DHisto(recovar + "_" + name + "_Split"));

		v_GenMET_bkgs_Split.push_back(histhelper.Get1DHisto(genvar + "_" + name + "_Split"));
		GenMET_all_Split->Add(histhelper.Get1DHisto(genvar + "_" + name + "_Split"));

		v_fakes_bkgs_Split.push_back(histhelper.Get1DHisto("fakes_" + name + "_Split"));
		fakes_all_Split->Add(histhelper.Get1DHisto("fakes_" + name + "_Split"));

		v_testmet_bkgs_Split.push_back(histhelper.Get1DHisto("TestMET" + name + "_Split"));
		TestMET_all_Split->Add(histhelper.Get1DHisto("TestMET" + name + "_Split"));

		v_A_bkgs_Split.push_back(histhelper.Get2DHisto("A_" + name + "_Split"));
		A_all_Split->Add(histhelper.Get2DHisto("A_" + name + "_Split"));

	}

	//Signal
	TH1F* GenMET_signal = histhelper.Get1DHisto("Evt_Pt_GenMET_signal");
	TH1F* MET_signal = histhelper.Get1DHisto("Evt_Pt_MET_signal");

	//Subtract Fakes from Data
	TH1F* h_DataMinFakes = (TH1F*) MET_data->Clone();
	h_DataMinFakes->Add(fakes_all, -1);

	TH1F* h_DummyDataMinFakes = (TH1F*) MET_DummyData_all->Clone();
	h_DummyDataMinFakes->Add(fakes_all_Split, -1);

	//calculate GenOverflow
	float GenIntegral = 0;
	for (int i = 0; i < 25 ; i++) {
		GenIntegral += A_all->GetBinContent(0, i);
	}
	cout << "A underflow (0,all): " << GenIntegral << endl;
	cout << "Fake integral: " << fakes_all->Integral() << endl;

	cout << "Data integral: " << MET_data->Integral() << endl;
	cout << "Data-Fake integral: " << h_DataMinFakes->Integral(h_DataMinFakes->GetXaxis()->FindBin(250), 25) << endl;

	cout << "Gen integral: " << GenMET_all->Integral() << endl;
	cout << "Reco (passes GenSelection) integral: " << TestMET_all->Integral() << endl;

//Do Unfolding
//Split Input (e.g. only on MC)
	cout << "Unfolding using only MC with a split of " <<  split << ":" << endl;
	Unfolder Unfolder_Split;
	Unfolder_Split.ParseConfig();
	TUnfoldDensity* unfold_Split = Unfolder_Split.SetUp(A_all_Split, MET_DummyData_all);

	unfold_Split->SubtractBackground(fakes_all_Split, "fakes_Split"); //subtract fakes

	unfold_Split->SetBias(GenMET_all_Split);

	std::tuple<int , TSpline*, TGraph*> TauResult_Split;
	TauResult_Split = Unfolder_Split.FindBestTau(unfold_Split);
	Unfolder_Split.VisualizeTau(TauResult_Split, "Split");

	// unfold_Split->DoUnfold(10.0);

	std::tuple<TH1*, TH1*> unfold_output_Split;
	unfold_output_Split = Unfolder_Split.GetOutput(unfold_Split);

	//ERRORS
	TH2* ErrorMatrix_Split = unfold_Split->GetEmatrixTotal("ErrorMatrix_Split");
	//stat sources
	TH2* ErrorMatrix_MCstat_Split = unfold_Split->GetEmatrixSysUncorr("ErrorMatrix_MCstat_Split");
	Drawer.Draw2D(ErrorMatrix_MCstat_Split, "ErrorMatrix_MCstat_Split");
	TH2* ErrorMatrix_input_Split = unfold_Split->GetEmatrixInput("ErrorMatrix_input_Split");
	Drawer.Draw2D(ErrorMatrix_input_Split, "ErrorMatrix_input_Split");
	//syst sources
	//subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr_Split = unfold_Split->GetEmatrixSysBackgroundUncorr("fakes_Split", "fakes__Split");
	Drawer.Draw2D(ErrorMatrix_subBKGuncorr_Split, "ErrorMatrix_subBKGuncorr_Split");
	// TH2* ErrorMatrix_subBKGscale_Split = unfold_Split->GetEmatrixSysBackgroundScale("ErrorMatrix_subBKGscale_Split", "fakes_Split");

	TH1D* METTotalError_Split = new TH1D("TotalError_Split", "MET", nBins_Gen, BinEdgesGen.data());
	std::vector<double> EStat_split;
	std::vector<double> ESyst_split;
	std::vector<double> zeros;
	std::vector<double> BinCenters_Split;
	std::vector<double> BinContents_Split;
	std::vector<double> TotalError_Split;

	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		double staterror = sqrt(ErrorMatrix_MCstat_Split->GetBinContent(bin, bin)
		                        + ErrorMatrix_input_Split->GetBinContent(bin, bin)
		                       );
		EStat_split.push_back(staterror);
		double systerror = sqrt(ErrorMatrix_subBKGuncorr_Split->GetBinContent(bin, bin)
		                        // + ErrorMatrix_subBKGscale_Split->GetBinContent(bin, bin)
		                       );
		ESyst_split.push_back(systerror);
		zeros.push_back(0);
		TotalError_Split.push_back(sqrt(pow(staterror,2)+pow(systerror,2)));
		BinCenters_Split.push_back(std::get<0>(unfold_output_Split)->GetBinCenter(bin));
		BinContents_Split.push_back(std::get<0>(unfold_output_Split)->GetBinContent(bin));
		METTotalError_Split->SetBinContent(bin, std::get<0>(unfold_output_Split)->GetBinContent(bin));
		METTotalError_Split->SetBinError(bin, sqrt(ErrorMatrix_Split->GetBinContent(bin, bin)));
	}

	TGraphErrors *MET_Split_Stat = new TGraphErrors(nBins_Gen, BinCenters_Split.data(), BinContents_Split.data(), zeros.data(), EStat_split.data());
	TGraphErrors *MET_Split_Syst = new TGraphErrors(nBins_Gen, BinCenters_Split.data(), BinContents_Split.data(), zeros.data(), TotalError_Split.data());

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
	
	//ERRORS
	TH2* ErrorMatrix = unfold->GetEmatrixTotal("ErrorMatrix");
	//stat sources
	TH2* ErrorMatrix_MCstat = unfold->GetEmatrixSysUncorr("ErrorMatrix_MCstat");
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat");
	TH2* ErrorMatrix_input = unfold->GetEmatrixInput("ErrorMatrix_input");
	Drawer.Draw2D(ErrorMatrix_input, "ErrorMatrix_input");
	//syst sources
	//subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr = unfold->GetEmatrixSysBackgroundUncorr("fakes", "fakes_all");
	Drawer.Draw2D(ErrorMatrix_subBKGuncorr, "ErrorMatrix_subBKGuncorr_Split");
	// TH2* ErrorMatrix_subBKGscale_Split = unfold_Split->GetEmatrixSysBackgroundScale("ErrorMatrix_subBKGscale_Split", "fakes_Split");

	TH1D* METTotalError = new TH1D("TotalError", "MET", nBins_Gen, BinEdgesGen.data());
	std::vector<double> EStat;
	std::vector<double> ESyst;
	std::vector<double> BinCenters;
	std::vector<double> BinContents;
	std::vector<double> TotalError;

	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		double staterror = sqrt(ErrorMatrix_MCstat->GetBinContent(bin, bin)
		                        + ErrorMatrix_input->GetBinContent(bin, bin)
		                       );
		EStat.push_back(staterror);
		double systerror = sqrt(ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin)
		                        // + ErrorMatrix_subBKGscale_Split->GetBinContent(bin, bin)
		                       );
		ESyst.push_back(systerror);
		TotalError.push_back(sqrt(pow(staterror,2)+pow(systerror,2)));
		BinCenters.push_back(std::get<0>(unfold_output)->GetBinCenter(bin));
		BinContents.push_back(std::get<0>(unfold_output)->GetBinContent(bin));
		METTotalError->SetBinContent(bin, std::get<0>(unfold_output)->GetBinContent(bin));
		METTotalError->SetBinError(bin, sqrt(ErrorMatrix->GetBinContent(bin, bin)));
	}

	TGraphErrors *MET_Stat = new TGraphErrors(nBins_Gen, BinCenters.data(), BinContents.data(), zeros.data(), EStat.data());
	TGraphErrors *MET_Syst = new TGraphErrors(nBins_Gen, BinCenters.data(), BinContents.data(), zeros.data(), TotalError.data());
	TH2* L = unfold->GetL("L");
	TH2* RhoTotal = unfold_Split->GetRhoIJtotal("RhoTotal");

// Draw Distributions
	bool log = true;
	bool drawpull = true;
//General Distributions
	// Drawer.Draw1D(MET_Wjet, "MET_Wjet");
	// Drawer.Draw1D(GenMET_Wjet, "GenMET_Wjet");
	// Drawer.Draw1D(MET_Wjet_Split, "MET_Wjet_Split");
	// Drawer.Draw1D(GenMET_Wjet_Split, "GenMET_Wjet_Split");

	// Drawer.Draw1D(MET_Zjet, "MET_Zjet");
	// Drawer.Draw1D(GenMET_Zjet, "GenMET_Zjet");
	// Drawer.Draw1D(MET_Zjet_Split, "MET_Zjet_Split");
	// Drawer.Draw1D(GenMET_Zjet_Split, "GenMET_Zjet_Split");

	Drawer.Draw1D(MET_all, "MET_all");
	Drawer.Draw1D(MET_all_Split, "MET_all_Split");
	Drawer.Draw1D(GenMET_all, "GenMET_all");
	Drawer.Draw1D(GenMET_all_Split, "GenMET_all_Split");
	Drawer.Draw1D(MET_data, "MET_data");
	Drawer.Draw1D(fakes_all, "fakes_all");
	Drawer.Draw1D(fakes_all_Split, "fakes_all_Split");
	Drawer.Draw1D(METTotalError, "MET_unfolded_errors");
	Drawer.Draw1D(METTotalError_Split, "MET_unfolded_errors_Split");


	// Drawer.Draw2D(A_Wjet, "A_Wjet", true);
	// Drawer.Draw2D(A_Zjet, "A_Zjet", true);
	// Drawer.Draw2D(A_Wjet_Split, "A_Wjet_Split", true);
	// Drawer.Draw2D(A_Zjet_Split, "A_Zjet_Split", true);
	Drawer.Draw2D(A_all, "A_all", log);
	Drawer.Draw2D(A_all_Split, "A_all_Split", log);
	Drawer.Draw2D(ErrorMatrix, "ErrorMatrix", log);
	Drawer.Draw2D(ErrorMatrix_Split, "ErrorMatrix_Split", log);
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat", log);
	Drawer.Draw2D(ErrorMatrix_MCstat_Split, "ErrorMatrix_MCstat_Split", log);
	Drawer.Draw2D(L, "L");
	Drawer.Draw2D(L_Split, "L_Split");
	Drawer.Draw2D(RhoTotal, "RhoTotal");
	Drawer.Draw2D(RhoTotal_Split, "RhoTotal_Split");

	Drawer.DrawDataMC(MET_data, v_MET_bkgs, bkgnames, "MET", log);
	Drawer.DrawDataMC(MET_DummyData_all, v_MET_bkgs_Split, bkgnames, "MET_Split", log, false, drawpull);

//Output of Unfolding
	std::vector<string> GenBkgNames;
	for (const std::string& name : bkgnames) {
		GenBkgNames.push_back("Gen_" + name);
	}

	//split Input
	Drawer.Draw1D(std::get<0>(unfold_output_Split), recovar + "_unfolded_Split");
	Drawer.Draw1D(std::get<1>(unfold_output_Split), recovar + "_foldedback_Split");

	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkgs_Split, GenBkgNames, "MET_UnfoldedvsGen_Split", log, false, drawpull);
	Drawer.DrawDataMCerror(METTotalError_Split, MET_Split_Stat, MET_Split_Syst, v_GenMET_bkgs_Split, GenBkgNames, "MET_UnfoldedvsGenErrors_Split", log, false, drawpull);

	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkgs_Split, GenBkgNames, "MET_UnfoldedvsGen_normalized_Split", log);
	Drawer.DrawDataMC(h_DummyDataMinFakes, {std::get<1>(unfold_output_Split)},  {"FoldedBack"}, "MET_DummyDatavsFoldedBack_Split");

	Drawer.DrawDataMC(h_DummyDataMinFakes, v_testmet_bkgs_Split, bkgnames, "DummyDataMinFakesvsTestMET" , log);

//Using Data
	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "_unfolded", log);
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "_foldedback", log);

	Drawer.DrawDataMC(METTotalError, v_GenMET_bkgs, GenBkgNames, "MET_UnfoldedvsGen", log, false, drawpull);
	Drawer.DrawDataMCerror(METTotalError, MET_Stat, MET_Syst, v_GenMET_bkgs, GenBkgNames, "MET_UnfoldedvsGenErrors", log, false, drawpull);

	Drawer.DrawDataMC(METTotalError, v_GenMET_bkgs, GenBkgNames, "MET_UnfoldedvsGen_normalized", log, true);
	Drawer.DrawDataMC(h_DataMinFakes, {std::get<1>(unfold_output)},  {"FoldedBack"}, "MET_DatavsFoldedBack", log);

	Drawer.Draw1D(h_DataMinFakes, "DataMinFakes");
	Drawer.DrawDataMC(h_DataMinFakes, v_testmet_bkgs, bkgnames, "DataMinFakesvsTestMET", log);








	return (0);
}

#endif