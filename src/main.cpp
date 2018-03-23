#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
#include "TUnfoldSys.h"
#include "TUnfoldDensity.h"

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
	int xMin_Gen = pt.get<int>("Binning.xMin_Gen");
	int xMax_Gen = pt.get<int>("Binning.xMax_Gen");
	std::vector<double> BinEdgesGen = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
	std::vector<double> BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesReco"));
	int nBins_Gen = BinEdgesGen.size() - 1;
	int nBins_Reco = BinEdgesReco.size() - 1;
	std::vector<string>	variation = to_array<std::string>(pt.get<std::string>("general.variation"));
	std::vector<string> bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));
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
	TH1F* MET_data = histhelper.Get1DHisto("Evt_Pt_MET_data_nominal");

	//variation x BkgNames
	//Backgrounds
	std::vector<std::vector<TH1*>> v_MET_bkgs(variation.size());
	std::vector<std::vector<TH1*>> v_GenMET_bkgs(variation.size());
	std::vector<std::vector<TH1*>> v_fakes_bkgs(variation.size());
	std::vector<std::vector<TH1*>> v_testmet_bkgs(variation.size());
	std::vector<std::vector<TH2*>> v_A_bkgs(variation.size());

	std::vector<TH1F*> MET_all;
	std::vector<TH1F*> GenMET_all;
	std::vector<TH1F*> fakes_all;
	std::vector<TH1F*> TestMET_all;
	std::vector<TH2*> A_all;

	//PseudoData
	std::vector<std::vector<TH1*>> v_MET_DummyDatas(variation.size());
	std::vector<std::vector<TH1*>> v_MET_bkgs_Split(variation.size());
	std::vector<std::vector<TH1*>> v_GenMET_bkgs_Split(variation.size());
	std::vector<std::vector<TH1*>> v_fakes_bkgs_Split(variation.size());
	std::vector<std::vector<TH1*>> v_testmet_bkgs_Split(variation.size());
	std::vector<std::vector<TH2*>> v_A_bkgs_Split(variation.size());

	std::vector<TH1F*> MET_DummyData_all;
	std::vector<TH1F*> MET_all_Split;
	std::vector<TH1F*> GenMET_all_Split;
	std::vector<TH1F*> fakes_all_Split;
	std::vector<TH1F*> TestMET_all_Split;
	std::vector<TH2*> A_all_Split;

	//Signal
	std::vector<TH1F*> GenMET_signal;
	std::vector<TH1F*> MET_signal;

	int nVariation = 0;
	for (auto& var : variation) {
		//backgrounds
		MET_all.push_back(histhelper.Get1DHisto(recovar + "_" + bkgnames.at(0) + "_" + var));
		GenMET_all.push_back(histhelper.Get1DHisto(genvar + "_" + bkgnames.at(0) + "_" + var));
		fakes_all.push_back(histhelper.Get1DHisto("fakes_"  + bkgnames.at(0) + "_" + var));
		TestMET_all.push_back(histhelper.Get1DHisto("TestMET" + bkgnames.at(0) + "_" + var));
		A_all.push_back(histhelper.Get2DHisto("A_" + bkgnames.at(0) + "_" + var));

		MET_all.at(nVariation)->Reset();
		GenMET_all.at(nVariation)->Reset();
		fakes_all.at(nVariation)->Reset();
		TestMET_all.at(nVariation)->Reset();
		A_all.at(nVariation)->Reset();

		//Pseudata
		MET_DummyData_all.push_back(histhelper.Get1DHisto("DummyData_" + bkgnames.at(0) + "_" + var + "_Split"));
		MET_all_Split.push_back(histhelper.Get1DHisto(recovar + "_" + bkgnames.at(0) + "_" + var + "_Split"));
		GenMET_all_Split.push_back(histhelper.Get1DHisto(genvar + "_" + bkgnames.at(0) + "_" + var + "_Split"));
		fakes_all_Split.push_back(histhelper.Get1DHisto("fakes_"  + bkgnames.at(0) + "_" + var + "_Split"));
		TestMET_all_Split.push_back(histhelper.Get1DHisto("TestMET" + bkgnames.at(0) + "_" + var + "_Split"));
		A_all_Split.push_back(histhelper.Get2DHisto("A_" + bkgnames.at(0) + "_" + var + "_Split"));

		MET_DummyData_all.at(nVariation)->Reset();
		MET_all_Split.at(nVariation)->Reset();
		GenMET_all_Split.at(nVariation)->Reset();
		fakes_all_Split.at(nVariation)->Reset();
		TestMET_all_Split.at(nVariation)->Reset();
		A_all_Split.at(nVariation)->Reset();

		//Signal
		GenMET_signal.push_back(histhelper.Get1DHisto("Evt_Pt_GenMET_signal_" + var));
		MET_signal.push_back(histhelper.Get1DHisto("Evt_Pt_MET_signal_" + var));
		nVariation += 1;
	}

	nVariation = 0;
	for (auto& var : variation) {
		for (const auto& name : bkgnames) {
			v_MET_bkgs.at(nVariation).push_back(histhelper.Get1DHisto(recovar + "_" + name + "_" + var));

			v_GenMET_bkgs.at(nVariation).push_back(histhelper.Get1DHisto(genvar + "_" + name + "_" + var));
			GenMET_all.at(nVariation)->Add(histhelper.Get1DHisto(genvar + "_" + name + "_" + var));

			v_fakes_bkgs.at(nVariation).push_back(histhelper.Get1DHisto("fakes_" + name + "_" + var));
			fakes_all.at(nVariation)->Add(histhelper.Get1DHisto("fakes_" + name + "_" + var));

			v_testmet_bkgs.at(nVariation).push_back(histhelper.Get1DHisto("TestMET" + name + "_" + var));
			TestMET_all.at(nVariation)->Add(histhelper.Get1DHisto("TestMET" + name + "_" + var));

			v_A_bkgs.at(nVariation).push_back(histhelper.Get2DHisto("A_" + name + "_" + var));
			A_all.at(nVariation)->Add(histhelper.Get2DHisto("A_" + name + "_" + var));



			v_MET_DummyDatas.at(nVariation).push_back(histhelper.Get1DHisto("DummyData_" + name + "_" + var + "_Split"));
			MET_DummyData_all.at(nVariation)->Add(histhelper.Get1DHisto("DummyData_" + name + "_" + var + "_Split"));

			v_MET_bkgs_Split.at(nVariation).push_back(histhelper.Get1DHisto(recovar + "_" + name + "_" + var + "_Split"));
			MET_all_Split.at(nVariation)->Add(histhelper.Get1DHisto(recovar + "_" + name + "_" + var + "_Split"));

			v_GenMET_bkgs_Split.at(nVariation).push_back(histhelper.Get1DHisto(genvar + "_" + name + "_" + var + "_Split"));
			GenMET_all_Split.at(nVariation)->Add(histhelper.Get1DHisto(genvar + "_" + name + "_" + var + "_Split"));

			v_fakes_bkgs_Split.at(nVariation).push_back(histhelper.Get1DHisto("fakes_" + name + "_" + var + "_Split"));
			fakes_all_Split.at(nVariation)->Add(histhelper.Get1DHisto("fakes_" + name + "_" + var + "_Split"));

			v_testmet_bkgs_Split.at(nVariation).push_back(histhelper.Get1DHisto("TestMET" + name + "_" + var + "_Split"));
			TestMET_all_Split.at(nVariation)->Add(histhelper.Get1DHisto("TestMET" + name + "_" + var + "_Split"));

			v_A_bkgs_Split.at(nVariation).push_back(histhelper.Get2DHisto("A_" + name + "_" + var + "_Split"));
			A_all_Split.at(nVariation)->Add(histhelper.Get2DHisto("A_" + name + "_" + var + "_Split"));
		}
		nVariation += 1;
	}

	//Subtract Fakes from Data
	std::vector<TH1F*> h_DataMinFakes;
	std::vector<TH1F*> h_DummyDataMinFakes;

	for (unsigned int i = 0; i < variation.size(); i++) {
		TH1F* tmp = (TH1F*) MET_data->Clone();
		tmp->Add(fakes_all.at(i), -1);
		h_DataMinFakes.push_back(tmp);

		TH1F* tmp2 = (TH1F*) MET_DummyData_all.at(i)->Clone();
		tmp2->Add(fakes_all_Split.at(i), -1);
		h_DummyDataMinFakes.push_back(tmp2);

	}


	//calculate GenOverflow
	float GenIntegral = 0;
	for (int i = 0; i < nBins_Gen ; i++) {
		GenIntegral += A_all.at(0)->GetBinContent(0, i);
	}
	cout << "A underflow (0,all): " << GenIntegral << endl;
	cout << "Fake integral: " << fakes_all.at(0)->Integral() << endl;

	cout << "Data integral: " << MET_data->Integral() << endl;
	cout << "Data-Fake integral: " << h_DataMinFakes.at(0)->Integral(h_DataMinFakes.at(0)->GetXaxis()->FindBin(250), 25) << endl;

	cout << "Gen integral: " << GenMET_all.at(0)->Integral() << endl;
	cout << "Reco (passes GenSelection) integral: " << TestMET_all.at(0)->Integral() << endl;

//Do Unfolding
//Split Input (e.g. only on MC)
	cout << "Unfolding using only MC with a split of " <<  split << ":" << endl;
	Unfolder Unfolder_Split;
	Unfolder_Split.ParseConfig();
	TUnfoldDensity* unfold_Split = Unfolder_Split.SetUp(A_all_Split.at(0), MET_DummyData_all.at(0));
	TH2* ProbMatrix_Split = (TH2*) A_all_Split.at(0)->Clone();
	ProbMatrix_Split->Reset();
	unfold_Split-> TUnfold::GetProbabilityMatrix(ProbMatrix_Split,TUnfoldDensity::kHistMapOutputVert);
	Drawer.Draw2D(ProbMatrix_Split, "ProbMatrix_Split");
	//subtract fakes
	unfold_Split->SubtractBackground(fakes_all_Split.at(0), "fakes_Split", 1, 0.0);

	//addsys variations of MigrationMatrix
	for (unsigned int i = 1; i < variation.size(); i++) {
		unfold_Split->AddSysError(A_all_Split.at(i), variation.at(i) + TString("_Split"), TUnfoldDensity::kHistMapOutputVert, TUnfoldDensity::kSysErrModeMatrix);
	}

	unfold_Split->SetBias(GenMET_all_Split.at(0));

	std::tuple<int , TSpline*, TGraph*> TauResult_Split;
	TauResult_Split = Unfolder_Split.FindBestTau(unfold_Split);
	Unfolder_Split.VisualizeTau(TauResult_Split, "Split");

	// unfold_Split->DoUnfold(10.0);

	std::tuple<TH1*, TH1*> unfold_output_Split;
	unfold_output_Split = Unfolder_Split.GetOutput(unfold_Split);

	//ERRORS
	TH2* ErrorMatrix_Split = unfold_Split->GetEmatrixTotal("ErrorMatrix_Split");

	//STAT SOURCES
	TH2* ErrorMatrix_MCstat_Split = unfold_Split->GetEmatrixSysUncorr("ErrorMatrix_MCstat_Split");
	Drawer.Draw2D(ErrorMatrix_MCstat_Split, "ErrorMatrix_MCstat_Split");
	TH2* ErrorMatrix_input_Split = unfold_Split->GetEmatrixInput("ErrorMatrix_input_Split");
	Drawer.Draw2D(ErrorMatrix_input_Split, "ErrorMatrix_input_Split");

	//SYST SOURCES
	//subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr_Split = unfold_Split->GetEmatrixSysBackgroundUncorr("fakes_Split", "fakes_Split");
	Drawer.Draw2D(ErrorMatrix_subBKGuncorr_Split, "ErrorMatrix_subBKGuncorr_Split");
	TH2* ErrorMatrix_subBKGscale_Split = (TH2*) ErrorMatrix_subBKGuncorr_Split->Clone();
	ErrorMatrix_subBKGscale_Split->Reset();
	unfold_Split->GetEmatrixSysBackgroundScale(ErrorMatrix_subBKGscale_Split, "fakes_Split");
	Drawer.Draw2D(ErrorMatrix_subBKGscale_Split, "ErrorMatrix_subBKGscale_Split");


	//Variations of MigrationMatrix
	std::vector<TH2*> v_ErrorMatrixVariations_Split;
	for (auto& var : variation) {
		TH2* tmp = (TH2*) ErrorMatrix_subBKGuncorr_Split->Clone();
		tmp->Reset();
		unfold_Split->GetEmatrixSysSource(tmp,  var + TString("_Split"));
		v_ErrorMatrixVariations_Split.push_back(tmp);
		Drawer.Draw2D(tmp, "ErrorMatrixVariations_" + var + "_Split", true);
	}

	TH1D* METTotalError_Split = new TH1D("TotalError_Split", "MET", nBins_Gen, BinEdgesGen.data());
	std::vector<double> EStat_split;
	std::vector<double> ESyst_split;
	std::vector<double> zeros;
	std::vector<double> BinCenters_Split;
	std::vector<double> BinContents_Split;
	std::vector<double> TotalError_Split;

	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		double staterror = ErrorMatrix_MCstat_Split->GetBinContent(bin, bin)
		                   + ErrorMatrix_input_Split->GetBinContent(bin, bin);
		EStat_split.push_back(sqrt(staterror));
		double systerror = ErrorMatrix_subBKGuncorr_Split->GetBinContent(bin, bin)
		                   + ErrorMatrix_subBKGscale_Split->GetBinContent(bin, bin);
		for (auto mat : v_ErrorMatrixVariations_Split ) {
			systerror += mat->GetBinContent(bin, bin);
		}
		ESyst_split.push_back(sqrt(systerror));
		zeros.push_back(0);
		TotalError_Split.push_back(sqrt(staterror + systerror));
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
	TUnfoldDensity* unfold = Unfolder.SetUp(A_all.at(0), MET_data);
	TH2* ProbMatrix = (TH2*) A_all.at(0)->Clone();
	ProbMatrix->Reset();
	unfold-> TUnfold::GetProbabilityMatrix(ProbMatrix,TUnfoldDensity::kHistMapOutputVert);
	Drawer.Draw2D(ProbMatrix, "ProbMatrix");

	unfold->SubtractBackground(fakes_all.at(0), "fakes", 1, 0.0); //subtract fakes

	//addsys variations of MigrationMatrix
	nVariation = 0;
	for (auto& var : variation) {
		// for (unsigned int i = 1; i < variation.size(); i++) {
		unfold->AddSysError(A_all.at(nVariation), TString(var), TUnfoldDensity::kHistMapOutputVert, TUnfoldDensity::kSysErrModeMatrix);
		nVariation += 1;
	}

	unfold->SetBias(GenMET_all.at(0));

	std::tuple<int , TSpline*, TGraph*> TauResult;
	TauResult = Unfolder.FindBestTau(unfold);
	Unfolder.VisualizeTau(TauResult, "data");

	//0st element=unfolded 1st=folded back
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);

	//ERRORS
	TH2* ErrorMatrix = unfold->GetEmatrixTotal("ErrorMatrix");

	//STAT SOURCES
	TH2* ErrorMatrix_MCstat = unfold->GetEmatrixSysUncorr("ErrorMatrix_MCstat");
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat");
	TH2* ErrorMatrix_input = unfold->GetEmatrixInput("ErrorMatrix_input");
	Drawer.Draw2D(ErrorMatrix_input, "ErrorMatrix_input");

	//SYST SOURCES
	//subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr = unfold->GetEmatrixSysBackgroundUncorr("fakes", "fakes");
	Drawer.Draw2D(ErrorMatrix_subBKGuncorr, "ErrorMatrix_subBKGuncorr");
	TH2* ErrorMatrix_subBKGscale = (TH2*) ErrorMatrix_subBKGuncorr->Clone();
	ErrorMatrix_subBKGscale->Reset();
	unfold->GetEmatrixSysBackgroundScale(ErrorMatrix_subBKGscale, "fakes");
	Drawer.Draw2D(ErrorMatrix_subBKGscale, "ErrorMatrix_subBKGscale");

	//Variations of MigrationMatrix
	std::vector<TH2*> v_ErrorMatrixVariations;
	for (auto& var : variation) {
		TH2* tmp = (TH2*) ErrorMatrix_subBKGuncorr->Clone();
		tmp->Reset();
		unfold->GetEmatrixSysSource(tmp, TString(var));
		v_ErrorMatrixVariations.push_back(tmp);
		Drawer.Draw2D(tmp, "ErrorMatrixVariations_" + TString(var), true);
	}


	TH1D* METTotalError = new TH1D("TotalError", "MET", nBins_Gen, BinEdgesGen.data());
	std::vector<double> EStat;
	std::vector<double> ESyst;
	std::vector<double> BinCenters;
	std::vector<double> BinContents;
	std::vector<double> TotalError;

	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		double staterror = ErrorMatrix_MCstat->GetBinContent(bin, bin)
		                   + ErrorMatrix_input->GetBinContent(bin, bin);
		EStat.push_back(sqrt(staterror));
		double systerror = ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin)
		                   + ErrorMatrix_subBKGscale->GetBinContent(bin, bin);
		for (auto mat : v_ErrorMatrixVariations ) {
			systerror += mat->GetBinContent(bin, bin);
		}
		ESyst.push_back(sqrt(systerror));
		zeros.push_back(0);
		TotalError.push_back(sqrt(staterror + systerror));
		BinCenters.push_back(std::get<0>(unfold_output)->GetBinCenter(bin));
		BinContents.push_back(std::get<0>(unfold_output)->GetBinContent(bin));
		METTotalError->SetBinContent(bin, std::get<0>(unfold_output)->GetBinContent(bin));
		METTotalError->SetBinError(bin, sqrt(ErrorMatrix->GetBinContent(bin, bin)));
	}

	TGraphErrors *MET_Stat = new TGraphErrors(nBins_Gen, BinCenters.data(), BinContents.data(), zeros.data(), EStat.data());
	TGraphErrors *MET_Syst = new TGraphErrors(nBins_Gen, BinCenters.data(), BinContents.data(), zeros.data(), TotalError.data());
	TH2* L = unfold->GetL("L");
	TH2* RhoTotal = unfold->GetRhoIJtotal("RhoTotal");

// Draw Distributions
	bool log = true;
	bool drawpull = true;
//General Distributions
	Drawer.Draw1D(MET_all.at(0), "MET_all");
	Drawer.Draw1D(MET_all_Split.at(0), "MET_all_Split");
	Drawer.Draw1D(GenMET_all.at(0), "GenMET_all");
	Drawer.Draw1D(GenMET_all_Split.at(0), "GenMET_all_Split");
	Drawer.Draw1D(MET_data, "MET_data");
	Drawer.Draw1D(fakes_all.at(0), "fakes_all");
	Drawer.Draw1D(fakes_all_Split.at(0), "fakes_all_Split");
	Drawer.Draw1D(METTotalError, "MET_unfolded_errors");
	Drawer.Draw1D(METTotalError_Split, "MET_unfolded_errors_Split");

	nVariation = 0;
	for (auto& var : variation) {
		Drawer.Draw2D(A_all.at(nVariation), "A_all" + var, log);
		Drawer.Draw2D(A_all_Split.at(nVariation), "A_all_" + var + "_Split", log);
		nVariation += 1;
	}

	Drawer.Draw2D(ErrorMatrix, "ErrorMatrix", log);
	Drawer.Draw2D(ErrorMatrix_Split, "ErrorMatrix_Split", log);
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat", log);
	Drawer.Draw2D(ErrorMatrix_MCstat_Split, "ErrorMatrix_MCstat_Split", log);
	Drawer.Draw2D(L, "L");
	Drawer.Draw2D(L_Split, "L_Split");
	Drawer.Draw2D(RhoTotal, "RhoTotal");
	Drawer.Draw2D(RhoTotal_Split, "RhoTotal_Split");

	Drawer.DrawDataMC(MET_data, v_MET_bkgs.at(0), bkgnames, "MET", log);
	Drawer.DrawDataMC(MET_DummyData_all.at(0), v_MET_bkgs_Split.at(0), bkgnames, "MET_Split", log, false, drawpull);

//Output of Unfolding
	std::vector<string> GenBkgNames;
	for (const std::string& name : bkgnames) {
		GenBkgNames.push_back("Gen_" + name);
	}

	//split Input
	Drawer.Draw1D(std::get<0>(unfold_output_Split), recovar + "_unfolded_Split");
	Drawer.Draw1D(std::get<1>(unfold_output_Split), recovar + "_foldedback_Split");

	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkgs_Split.at(0), GenBkgNames, "MET_UnfoldedvsGen_Split", log, false, drawpull);
	Drawer.DrawDataMCerror(METTotalError_Split, MET_Split_Stat, MET_Split_Syst, v_GenMET_bkgs_Split.at(0), GenBkgNames, "MET_UnfoldedvsGenErrors_Split", log, false, drawpull);

	Drawer.DrawDataMC(METTotalError_Split, v_GenMET_bkgs_Split.at(0), GenBkgNames, "MET_UnfoldedvsGen_normalized_Split", log);
	Drawer.DrawDataMC(h_DummyDataMinFakes.at(0), {std::get<1>(unfold_output_Split)},  {"FoldedBack"}, "MET_DummyDatavsFoldedBack_Split");

	Drawer.DrawDataMC(h_DummyDataMinFakes.at(0), v_testmet_bkgs_Split.at(0), bkgnames, "DummyDataMinFakesvsTestMET" , log);

//Using Data
	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "_unfolded", log);
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "_foldedback", log);

	Drawer.DrawDataMC(METTotalError, v_GenMET_bkgs.at(0), GenBkgNames, "MET_UnfoldedvsGen", log, false, drawpull);
	Drawer.DrawDataMCerror(METTotalError, MET_Stat, MET_Syst, v_GenMET_bkgs.at(0), GenBkgNames, "MET_UnfoldedvsGenErrors", log, false, drawpull);

	Drawer.DrawDataMC(METTotalError, v_GenMET_bkgs.at(0), GenBkgNames, "MET_UnfoldedvsGen_normalized", log, true);
	Drawer.DrawDataMC(h_DataMinFakes.at(0), {std::get<1>(unfold_output)},  {"FoldedBack"}, "MET_DatavsFoldedBack", log);

	Drawer.Draw1D(h_DataMinFakes.at(0), "DataMinFakes");
	Drawer.DrawDataMC(h_DataMinFakes.at(0), v_testmet_bkgs.at(0), bkgnames, "DataMinFakesvsTestMET", log);


	Drawer.DrawDataMC(v_MET_bkgs[0][3], {v_MET_bkgs[1][3]}, {"JESup"}, "nomvsJESup", log);









	return (0);
}

#endif