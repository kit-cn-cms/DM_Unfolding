#include "TUnfoldDensity.h"
#include "TUnfoldSys.h"
#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "TMath.h"
#include "TGraphErrors.h"
#include "TSortedList.h"
#include <math.h>

#include "../interface/BinFinder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/HistHelper.hpp"
#include "../interface/HistMaker.hpp"
#include "../interface/PathHelper.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/UnfoldWrapper.hpp"

using namespace std;
#ifndef Main_CPP_
#define Main_CPP_

template<typename T>
std::vector<T>
to_array(const std::string& s)
{
  std::vector<T> result;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, ','))
    result.push_back(boost::lexical_cast<T>(item));
  return result;
}

int
main(int argc, char** argv)
{
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
  std::vector<double> BinEdgesGen =
    to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
  int nBins_Gen = BinEdgesGen.size() - 1;
  int nBins_Reco = pt.get<int>("Binning.nBins_Reco");
  std::vector<string> variation =
    to_array<std::string>(pt.get<std::string>("general.variation"));
  std::vector<string> systematics = to_array<std::string>(pt.get<std::string>("general.systematics"));

  std::vector<string> bkgnames =
    to_array<std::string>(pt.get<std::string>("Bkg.names"));
  bool fillhistos = true;
  bool DoUnfolding = pt.get<bool>("general.doUnfolding");
  bool FindBinning = pt.get<bool>("general.FindBinning");
  bool log = true;
  bool drawpull = true;

  // Fill Histos?
  char c;
  std::ifstream histfile(path.GetHistoFilePath());
  if (histfile.good()) {
    cout << "Histo ROOT file found here: " << path.GetHistoFilePath() << endl;
    cout << "Do you want to refill the Histos? (y/n)" << endl;
    cin >> c;
    if (c == 'y')
      fillhistos = true;
    else
      fillhistos = false;
  }
  HistMaker histomaker;
  if (fillhistos) {
    // Fill Histos
    histomaker.MakeHistos();
  }
  // Reset Output ROOTFILE
  std::remove(path.GetOutputFilePath());

// Return relevant Histos

  // Full Sample
  HistHelper histhelper;
  // Data
  TH1F* MET_data = histhelper.Get1DHisto("Evt_Pt_MET_data_nominal");

  // variation x BkgNames
  // Backgrounds
  std::vector<std::vector<TH1*>> v_MET_bkgs(variation.size());
  std::vector<std::vector<TH1*>> v_GenMET_bkgs(variation.size());
  std::vector<std::vector<TH1*>> v_fakes_bkgs(variation.size());
  std::vector<std::vector<TH1*>> v_testmet_bkgs(variation.size());
  std::vector<std::vector<TH1*>> v_testMETgenBinning_bkgs(variation.size());
  std::vector<std::vector<TH2*>> v_A_bkgs(variation.size());

  std::vector<TH1F*> MET_all;
  std::vector<TH1F*> GenMET_all;
  std::vector<TH1F*> fakes_all;
  std::vector<TH1F*> TestMET_all;
  std::vector<TH1F*> testMETgenBinning_all;
  std::vector<TH2*> A_all;
  std::vector<TH2*> A_equBins_all;

  // PseudoData
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

  // Signal
  std::vector<TH1F*> GenMET_signal;
  std::vector<TH1F*> MET_signal;

  int nVariation = 0;
  for (auto& var : variation) {
    // backgrounds
    MET_all.push_back(
      histhelper.Get1DHisto(recovar + "_" + bkgnames.at(0) + "_" + var));
    GenMET_all.push_back(
      histhelper.Get1DHisto(genvar + "_" + bkgnames.at(0) + "_" + var));
    fakes_all.push_back(
      histhelper.Get1DHisto("fakes_" + bkgnames.at(0) + "_" + var));
    TestMET_all.push_back(
      histhelper.Get1DHisto("TestMET" + bkgnames.at(0) + "_" + var));
    testMETgenBinning_all.push_back(
      histhelper.Get1DHisto("testMETgenBinning" + bkgnames.at(0) + "_" + var));
    A_all.push_back(histhelper.Get2DHisto("A_" + bkgnames.at(0) + "_" + var));
    A_equBins_all.push_back(
      histhelper.Get2DHisto("A_equBins" + bkgnames.at(0) + "_" + var));

    MET_all.at(nVariation)->Reset();
    GenMET_all.at(nVariation)->Reset();
    fakes_all.at(nVariation)->Reset();
    TestMET_all.at(nVariation)->Reset();
    testMETgenBinning_all.at(nVariation)->Reset();
    A_all.at(nVariation)->Reset();
    A_equBins_all.at(nVariation)->Reset();

    // Pseudata
    MET_DummyData_all.push_back(histhelper.Get1DHisto(
                                  "DummyData_" + bkgnames.at(0) + "_" + var + "_Split"));
    MET_all_Split.push_back(histhelper.Get1DHisto(
                              recovar + "_" + bkgnames.at(0) + "_" + var + "_Split"));
    GenMET_all_Split.push_back(histhelper.Get1DHisto(
                                 genvar + "_" + bkgnames.at(0) + "_" + var + "_Split"));
    fakes_all_Split.push_back(
      histhelper.Get1DHisto("fakes_" + bkgnames.at(0) + "_" + var + "_Split"));
    TestMET_all_Split.push_back(
      histhelper.Get1DHisto("TestMET" + bkgnames.at(0) + "_" + var + "_Split"));
    A_all_Split.push_back(
      histhelper.Get2DHisto("A_" + bkgnames.at(0) + "_" + var + "_Split"));

    MET_DummyData_all.at(nVariation)->Reset();
    MET_all_Split.at(nVariation)->Reset();
    GenMET_all_Split.at(nVariation)->Reset();
    fakes_all_Split.at(nVariation)->Reset();
    TestMET_all_Split.at(nVariation)->Reset();
    A_all_Split.at(nVariation)->Reset();

    // Signal
    GenMET_signal.push_back(
      histhelper.Get1DHisto("Evt_Pt_GenMET_signal_" + var));
    MET_signal.push_back(histhelper.Get1DHisto("Evt_Pt_MET_signal_" + var));
    nVariation += 1;
  }

  nVariation = 0;
  for (auto& var : variation) {
    for (const auto& name : bkgnames) {
      v_MET_bkgs.at(nVariation)
      .push_back(histhelper.Get1DHisto(recovar + "_" + name + "_" + var));
      MET_all.at(nVariation)
      ->Add(histhelper.Get1DHisto(recovar + "_" + name + "_" + var));

      v_GenMET_bkgs.at(nVariation)
      .push_back(histhelper.Get1DHisto(genvar + "_" + name + "_" + var));
      GenMET_all.at(nVariation)
      ->Add(histhelper.Get1DHisto(genvar + "_" + name + "_" + var));

      v_fakes_bkgs.at(nVariation)
      .push_back(histhelper.Get1DHisto("fakes_" + name + "_" + var));
      fakes_all.at(nVariation)
      ->Add(histhelper.Get1DHisto("fakes_" + name + "_" + var));

      v_testmet_bkgs.at(nVariation)
      .push_back(histhelper.Get1DHisto("TestMET" + name + "_" + var));
      TestMET_all.at(nVariation)
      ->Add(histhelper.Get1DHisto("TestMET" + name + "_" + var));

      v_testMETgenBinning_bkgs.at(nVariation)
      .push_back(
        histhelper.Get1DHisto("testMETgenBinning" + name + "_" + var));
      testMETgenBinning_all.at(nVariation)
      ->Add(histhelper.Get1DHisto("testMETgenBinning" + name + "_" + var));

      v_A_bkgs.at(nVariation)
      .push_back(histhelper.Get2DHisto("A_" + name + "_" + var));
      A_all.at(nVariation)->Add(histhelper.Get2DHisto("A_" + name + "_" + var));
      A_equBins_all.at(nVariation)
      ->Add(histhelper.Get2DHisto("A_equBins" + name + "_" + var));

      v_MET_DummyDatas.at(nVariation)
      .push_back(
        histhelper.Get1DHisto("DummyData_" + name + "_" + var + "_Split"));
      MET_DummyData_all.at(nVariation)
      ->Add(
        histhelper.Get1DHisto("DummyData_" + name + "_" + var + "_Split"));

      v_MET_bkgs_Split.at(nVariation)
      .push_back(
        histhelper.Get1DHisto(recovar + "_" + name + "_" + var + "_Split"));
      MET_all_Split.at(nVariation)
      ->Add(
        histhelper.Get1DHisto(recovar + "_" + name + "_" + var + "_Split"));

      v_GenMET_bkgs_Split.at(nVariation)
      .push_back(
        histhelper.Get1DHisto(genvar + "_" + name + "_" + var + "_Split"));
      GenMET_all_Split.at(nVariation)
      ->Add(
        histhelper.Get1DHisto(genvar + "_" + name + "_" + var + "_Split"));

      v_fakes_bkgs_Split.at(nVariation)
      .push_back(
        histhelper.Get1DHisto("fakes_" + name + "_" + var + "_Split"));
      fakes_all_Split.at(nVariation)
      ->Add(histhelper.Get1DHisto("fakes_" + name + "_" + var + "_Split"));

      v_testmet_bkgs_Split.at(nVariation)
      .push_back(
        histhelper.Get1DHisto("TestMET" + name + "_" + var + "_Split"));
      TestMET_all_Split.at(nVariation)
      ->Add(histhelper.Get1DHisto("TestMET" + name + "_" + var + "_Split"));

      v_A_bkgs_Split.at(nVariation)
      .push_back(histhelper.Get2DHisto("A_" + name + "_" + var + "_Split"));
      A_all_Split.at(nVariation)
      ->Add(histhelper.Get2DHisto("A_" + name + "_" + var + "_Split"));
    }
    nVariation += 1;
  }

  // Subtract Fakes from Data
  std::vector<TH1F*> h_DataMinFakes;
  std::vector<TH1F*> h_DummyDataMinFakes;

  for (unsigned int i = 0; i < variation.size(); i++) {
    TH1F* tmp = (TH1F*)MET_data->Clone();
    tmp->Add(fakes_all.at(i), -1);
    h_DataMinFakes.push_back(tmp);

    TH1F* tmp2 = (TH1F*)MET_DummyData_all.at(i)->Clone();
    tmp2->Add(fakes_all_Split.at(i), -1);
    h_DummyDataMinFakes.push_back(tmp2);
  }
  std::vector<std::vector<char>> v;

  // calculate GenOverflow
  float GenIntegral = 0;
  for (int i = 0; i < nBins_Gen; i++) {
    GenIntegral += A_all.at(0)->GetBinContent(0, i);
  }
  cout << "A underflow (0,all): " << GenIntegral << endl;
  cout << "Fake integral: " << fakes_all.at(0)->Integral() << endl;

  cout << "Data integral: " << MET_data->Integral() << endl;
  cout << "Data-Fake integral: "
       << h_DataMinFakes.at(0)->Integral(
         h_DataMinFakes.at(0)->GetXaxis()->FindBin(250), 25)
       << endl;
  cout << "Gen integral: " << GenMET_all.at(0)->Integral() << endl;
  cout << "Reco (passes GenSelection) integral: "
       << TestMET_all.at(0)->Integral() << endl;

  Drawer.DrawRatio(TestMET_all.at(0), MET_all.at(0), "Purity");
  Drawer.DrawRatio(testMETgenBinning_all.at(0), GenMET_all.at(0), "Stability");
  Drawer.DrawDataMC(h_DummyDataMinFakes.at(0),
                    v_testmet_bkgs_Split.at(0),
                    bkgnames,
                    "DummyDataMinFakesvsTestMET",
                    log);
  Drawer.DrawDataMC(h_DataMinFakes.at(0),
                    v_testmet_bkgs.at(0),
                    bkgnames,
                    "DataMinFakesvsTestMET",
                    log);
  Drawer.Draw2D(A_equBins_all.at(0), "MigrationMatrix_equBins", log, "MET_Reco", "MET_Gen");

  if (FindBinning) {
    BinFinder BinFinder(A_equBins_all.at(0));
    BinFinder.GetBins();
  }

  if (DoUnfolding) {
//Do the unfolding
    UnfoldWrapper Wrapper = UnfoldWrapper("MET", "data", A_all, MET_data, fakes_all.at(0), v_MET_bkgs, v_GenMET_bkgs, variation, bkgnames, BinEdgesGen);
    Wrapper.DoIt();

    UnfoldWrapper Wrapper_Split = UnfoldWrapper("MET", "Split", A_all_Split,  MET_DummyData_all.at(0), fakes_all_Split.at(0), v_MET_bkgs_Split, v_GenMET_bkgs_Split, variation, bkgnames, BinEdgesGen);
    Wrapper_Split.DoIt();


    // TH1F* InputwithSignal = (TH1F*) MET_DummyData_all.at(0)->Clone();
    // MET_signal.at(0)->Scale(0.1);
    // InputwithSignal->Add(MET_signal.at(0));

    // UnfoldWrapper Wrapper_Split_Signal = UnfoldWrapper("MET", "SplitSignal", A_all_Split,  InputwithSignal, fakes_all_Split.at(0), v_MET_bkgs_Split, v_GenMET_bkgs_Split, variation, bkgnames, BinEdgesGen);
    // Wrapper_Split_Signal.DoIt();
// Draw Stuff

    // General Distributions
    Drawer.Draw1D(MET_signal.at(0), "MET_signal");
    // Drawer.Draw1D(MET_all_Split.at(0), "MET_all_Split");
    // Drawer.Draw1D(GenMET_all.at(0), "GenMET_all");
    // Drawer.Draw1D(GenMET_all_Split.at(0), "GenMET_all_Split");
    // Drawer.Draw1D(MET_data, "MET_data");
    // Drawer.Draw1D(fakes_all.at(0), "fakes_all");
    // Drawer.Draw1D(fakes_all_Split.at(0), "fakes_all_Split");
    // Drawer.Draw1D(METTotalError, "MET_unfolded_errors");
    // Drawer.Draw1D(METTotalError_Split, "MET_unfolded_errors_Split");

    nVariation = 0;
    for (auto& var : variation) {
      Drawer.Draw2D(A_all.at(nVariation), "A_all" + var, log);
      Drawer.Draw2D(A_all_Split.at(nVariation), "A_all_" + var + "_Split", log);
      nVariation += 1;
    }
  }

  return (0);
}

#endif