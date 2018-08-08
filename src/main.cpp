#include "TUnfoldDensity.h"
#include "TUnfoldSys.h"
#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <TMath.h>
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

#ifndef Main_CPP_
#define Main_CPP_
template<typename T> std::vector<T>to_array(const std::string& s)
{
  std::vector<T> result;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, ','))
    result.push_back(boost::lexical_cast<T>(item));
  return result;
}

int
main(int argc, char** argv) {
  HistDrawer Drawer;
  char currentdir[1024];
  getcwd(currentdir, sizeof(currentdir));
  std::string workingdir(currentdir);
  PathHelper path;
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini(std::string(path.GetConfigPath()), pt);
  TString genvar = pt.get<std::string>("vars.gen");
  TString recovar = pt.get<std::string>("vars.reco");
  int split = pt.get<int>("general.split");
  std::vector<double> BinEdgesGen = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
  int nBins_Gen = BinEdgesGen.size() - 1;
  std::vector<double> BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
  int nBins_Reco = BinEdgesReco.size() - 1;
  std::vector<std::string> variation = to_array<std::string>(pt.get<std::string>("general.variation"));
  std::vector<std::string> systematics = to_array<std::string>(pt.get<std::string>("general.systematics"));
  std::vector<std::string> BosonSystematics = to_array<std::string>(pt.get<std::string>("general.BosonSystematics"));
  std::vector<std::string> unfoldedsysts = to_array<std::string>(pt.get<std::string>("general.unfoldedsysts"));

  systematics.insert(std::end(systematics), std::begin(BosonSystematics), std::end(BosonSystematics));
  std::vector<TString> allSystematics;
  allSystematics.insert(std::end(allSystematics), std::begin(systematics), std::end(systematics));

  std::vector<std::string> bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));
  bool fillhistos = true;
  bool DoUnfolding = pt.get<bool>("general.doUnfolding");
  bool FindBinning = pt.get<bool>("general.FindBinning");
  bool runBlind = pt.get<bool>("general.runBlind");
  bool log = true;
  bool drawpull = true;


  // Fill Histos?
  char c;
  std::ifstream histfile(path.GetHistoFilePath());
  if (histfile.good()) {
    std::cout << "Histo ROOT file found here: " << path.GetHistoFilePath() << std::endl;
    std::cout << "Do you want to refill the Histos? (y/n)" << std::endl;
    std::cin >> c;
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
  TH1F* MET_data = histhelper.Get1DHisto("data_Evt_Pt_MET");

  //  variations x BkgNames

  // Backgrounds
  std::vector<std::vector<TH1*>> v_MET_bkgs;
  std::vector<std::vector<TH1*>> v_GenMET_bkgs;
  std::vector<std::vector<TH1*>> v_fakes_bkgs;
  std::vector<std::vector<TH1*>> v_misses_bkgs;
  std::vector<std::vector<TH1*>> v_testmet_bkgs;
  std::vector<std::vector<TH1*>> v_testMETgenBinning_bkgs;
  // std::vector<std::vector<TH1*>> v_GenMETReco_bkgs;
  std::vector<std::vector<TH2*>> v_A_bkgs;

  std::vector<std::vector<TH1*>> v_MET_DummyDatas;
  std::vector<std::vector<TH1*>> v_MET_bkgs_Split;
  std::vector<std::vector<TH1*>> v_GenMET_bkgs_Split;
  std::vector<std::vector<TH1*>> v_fakes_bkgs_Split;
  std::vector<std::vector<TH1*>> v_misses_bkgs_Split;
  std::vector<std::vector<TH1*>> v_testmet_bkgs_Split;
  std::vector<std::vector<TH2*>> v_A_bkgs_Split;

  v_MET_bkgs = histhelper.getAllVariations(bkgnames, "Evt_Pt_MET", {});
  v_GenMET_bkgs = histhelper.getAllVariations(bkgnames, "Evt_Pt_GenMET", {});
  v_fakes_bkgs = histhelper.getAllVariations(bkgnames, "fakes", {});
  v_misses_bkgs = histhelper.getAllVariations(bkgnames, "misses", {});
  v_testmet_bkgs = histhelper.getAllVariations(bkgnames, "TestMET", {});
  v_testMETgenBinning_bkgs = histhelper.getAllVariations(bkgnames, "testMETgenBinning", {});
  // v_GenMETReco_bkgs = histhelper.getAllVariations(bkgnames, "h_GenRecoMET", allSystematics);
  v_A_bkgs = histhelper.getAllVariations2D(bkgnames, "A", allSystematics);

  v_MET_DummyDatas = histhelper.getAllVariations(bkgnames, "DummyData_Split", {});
  v_MET_bkgs_Split = histhelper.getAllVariations(bkgnames, "Evt_Pt_MET_Split", {});
  v_GenMET_bkgs_Split = histhelper.getAllVariations(bkgnames, "Evt_Pt_GenMET_Split", {});
  v_fakes_bkgs_Split = histhelper.getAllVariations(bkgnames, "fakes_Split", {});
  v_misses_bkgs_Split = histhelper.getAllVariations(bkgnames, "misses_Split", {});
  v_testmet_bkgs_Split = histhelper.getAllVariations(bkgnames, "TestMET_Split", {});
  v_A_bkgs_Split = histhelper.getAllVariations2D(bkgnames, "A_Split", {});


  v_testmet_bkgs.at(0)[0]->Print();
  v_testmet_bkgs.at(0)[1]->Print();
  v_testmet_bkgs.at(0)[2]->Print();
  v_testmet_bkgs.at(0)[3]->Print();
  v_testmet_bkgs.at(0)[4]->Print();

  // add up all BKGSamples
  std::vector<TH1F*> MET_all = histhelper.AddAllBkgs(bkgnames, recovar, {});
  std::vector<TH1F*> GenMET_all = histhelper.AddAllBkgs(bkgnames, genvar, {});
  std::vector<TH1F*> fakes_all = histhelper.AddAllBkgs(bkgnames, "fakes", {});
  std::vector<TH1F*> misses_all = histhelper.AddAllBkgs(bkgnames, "misses", {});
  std::vector<TH1F*> TestMET_all = histhelper.AddAllBkgs(bkgnames, "TestMET", {});
  std::vector<TH1F*> GenMetReco_all = histhelper.AddAllBkgs(bkgnames, "h_GenRecoMET", {});
  std::vector<TH1F*> testMETgenBinning_all = histhelper.AddAllBkgs(bkgnames, "testMETgenBinning", {});
  std::vector<TH2*> A_all = histhelper.AddAllBkgs2D(bkgnames, "A", allSystematics);
  std::vector<TH2*> A_equBins_all = histhelper.AddAllBkgs2D(bkgnames, "A_equBins", {});

  // // PseudoData
  std::vector<TH1F*> MET_DummyData_all = histhelper.AddAllBkgs(bkgnames, "DummyData_Split", {});
  std::vector<TH1F*> MET_all_Split = histhelper.AddAllBkgs(bkgnames, recovar + "_Split", {});
  std::vector<TH1F*> GenMET_all_Split = histhelper.AddAllBkgs(bkgnames, genvar + "_Split", {});
  std::vector<TH1F*> fakes_all_Split = histhelper.AddAllBkgs(bkgnames, "fakes_Split", {});
  std::vector<TH1F*> misses_all_Split = histhelper.AddAllBkgs(bkgnames, "misses_Split", {});
  std::vector<TH1F*> TestMET_all_Split = histhelper.AddAllBkgs(bkgnames, "TestMET_Split", {});
  std::vector<TH2*> A_all_Split = histhelper.AddAllBkgs2D(bkgnames, "A_Split", allSystematics);

  std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap;
  std::map<std::string, std::pair<TH1*, int>> nameRecoSampleColorMap;

  std::map<std::string, std::pair<TH1*, int>> nameTestMETSampleColorMap;
  std::map<std::string, std::pair<TH1*, int>> nameTestMETSplitSampleColorMap;

  std::map<std::string, std::pair<TH1*, int>> namefakesSplitSampleColorMap;
  std::map<std::string, std::pair<TH1*, int>> namefakesSampleColorMap;

  std::map<std::string, std::pair<TH1*, int>> namemissesSplitSampleColorMap;
  std::map<std::string, std::pair<TH1*, int>> namemissesSampleColorMap;

  std::vector<int> color = { kViolet + 7, kViolet + 3, kViolet - 1, kViolet - 2, kViolet, kViolet - 7, kGreen, kBlue};
  std::vector<std::string> names = {"#gamma +jets", "QCD", "Single Top", "t#bar{t}", "Diboson", "Z(ll)+jets", "W(l#nu)+jets", "Z(#nu#nu)+jets"};
  int i = 0;
  for (auto& name : names) {
    nameGenSampleColorMap[name] = std::make_pair(v_GenMET_bkgs.at(0)[i], color.at(i));
    nameRecoSampleColorMap[name] = std::make_pair(v_MET_bkgs.at(0)[i], color.at(i));
    v_testmet_bkgs.at(0)[i]->Print();
    nameTestMETSampleColorMap[name] = std::make_pair(v_testmet_bkgs.at(0)[i], color.at(i));
    nameTestMETSplitSampleColorMap[name] = std::make_pair(v_testmet_bkgs_Split.at(0)[i], color.at(i));

    namefakesSampleColorMap[name] = std::make_pair(v_fakes_bkgs.at(0)[i], color.at(i));
    namefakesSplitSampleColorMap[name] = std::make_pair(v_fakes_bkgs_Split.at(0)[i], color.at(i));

    namemissesSampleColorMap[name] = std::make_pair(v_misses_bkgs.at(0)[i], color.at(i));
    namemissesSplitSampleColorMap[name] = std::make_pair(v_misses_bkgs_Split.at(0)[i], color.at(i));

    i++;
  }


  // Subtract Fakes from Data
  std::vector<TH1F*> h_DataMinFakes;
  std::vector<TH1F*> h_DummyDataMinFakes;
  std::vector<TH1F*> RecoMCMinFakes;

  // for (unsigned int i = 0; i < variation.size(); i++) {
  TH1F* tmp = (TH1F*)MET_data->Clone();
  tmp->Add(fakes_all.at(0), -1);
  h_DataMinFakes.push_back(tmp);

  TH1F* tmp3 = (TH1F*)MET_all.at(0)->Clone();
  tmp3->Add(fakes_all.at(0), -1);
  RecoMCMinFakes.push_back(tmp3);

  TH1F* tmp2 = (TH1F*)MET_DummyData_all.at(0)->Clone();
  tmp2->Add(fakes_all_Split.at(0), -1);
  h_DummyDataMinFakes.push_back(tmp2);
  // }
  std::vector<std::vector<char>> v;

  // calculate Underflows
  float RecoUF = A_all.at(0)->Integral(0, 0, 1, nBins_Gen + 1); //inefficency due to misses
  float GenUF = A_all.at(0)->Integral(0, nBins_Reco + 1, 0, 0); //get unfolded

  // float RecoOF = A_all.at(0)->Integral(0, 0, 1, nBins_Gen); //inefficency due to misses
  // float GenOF = A_all.at(0)->Integral(0, nBins_Reco, 0, 0); //get unfolded

  // TH1* RecoMinFakeMC = (TH1F*) MET_all.at(0)->Clone();
  // for(auto& h: v_fakes_bkgs.at(0))
  // RecoMinFakeMC->Add(h,-1);
  // Drawer.DrawDataMC(h_DataMinFakes.at(0), {RecoMinFakeMC}, nameTestMETSampleColorMap, "DataMinFakesvsTestMET", log, !normalize, "#slash{E}_{T}");


  std::cout << "A Gen underflow+overflow (all,0) (gets unfolded): " << GenUF << std::endl;
  std::cout << "A Reco underflow+overflow (0,all) (inefficency due to misses): " << RecoUF << std::endl;
  misses_all.at(0)->Print();
  std::cout << "misses integral: " << misses_all.at(0)->Integral(0, misses_all.at(0)->GetNbinsX() + 1) << std::endl;

  std::cout << "Fake integral: " << fakes_all.at(0)->Integral() << std::endl;

  std::cout << "Data integral: " << MET_data->Integral() << std::endl;
  // std::cout << "Data entries: " << MET_data->GetEntries() << std::endl;
  std::cout << "MC integral: " << MET_all.at(0)->Integral() << std::endl;
  // std::cout << "MC entries: " << MET_all.at(0)->GetEntries() << std::endl;
  std::cout << "Data-Fake integral: " << h_DataMinFakes.at(0)->Integral() << std::endl;
  // std::cout << "Data-Fake entries: " << h_DataMinFakes.at(0)->GetEntries() << std::endl;
  std::cout << "Gen integral: " << GenMET_all.at(0)->Integral(0, GenMET_all.at(0)->GetNbinsX() + 1) << std::endl;
  std::cout << "Gen RecoMET integral: " << GenMetReco_all.at(0)->Integral(0, GenMET_all.at(0)->GetNbinsX() + 1) << std::endl;
  // std::cout << "Gen entries: " << GenMET_all.at(0)->GetEntries() << std::endl;
  std::cout << "Reco (passes GenSelection) integral: " << TestMET_all.at(0)->Integral() << std::endl;
  // std::cout << "Reco (passes GenSelection) entries: " << TestMET_all.at(0)->GetEntries() << std::endl;
  bool normalize = true;
  Drawer.DrawStack(v_fakes_bkgs.at(0), namefakesSampleColorMap, "fakes", log, !normalize, "#slash{E}_{T}");
  // Drawer.DrawStack(v_fakes_bkgs_Split.at(0), namefakesSplitSampleColorMap, "fakeSplit", log, !normalize, "#slash{E}_{T}");

  Drawer.DrawStack(v_misses_bkgs.at(0), namemissesSampleColorMap, "misses", log, !normalize, "#slash{E}_{T}");
  // Drawer.DrawStack(v_misses_bkgs_Split.at(0), namemissesSplitSampleColorMap, "missesSplit", log, !normalize, "#slash{E}_{T}");

  Drawer.DrawRatio(TestMET_all.at(0), MET_all.at(0), "Purity");
  Drawer.DrawRatio(testMETgenBinning_all.at(0), GenMET_all.at(0), "Stability");
  // Drawer.DrawDataMC(h_DummyDataMinFakes.at(0), v_testmet_bkgs_Split.at(0), nameTestMETSplitSampleColorMap, "DummyDataMinFakesvsTestMET", log, !normalize, "#slash{E}_{T}");
  Drawer.DrawDataMC(h_DataMinFakes.at(0), v_testmet_bkgs.at(0), nameTestMETSampleColorMap, "DataMinFakesvsTestMET", log, !normalize, "#slash{E}_{T}");
  // Drawer.DrawDataMC(RecoMCMinFakes.at(0), v_testmet_bkgs.at(0), nameTestMETSampleColorMap, "DataMinFakesvsTestMET", log, !normalize, "#slash{E}_{T}");
  Drawer.Draw2D(A_equBins_all.at(0), "MigrationMatrix_equBins", log, "MET_Reco", "MET_Gen");

  if (FindBinning) {
    BinFinder BinFinder(A_equBins_all.at(0));
    BinFinder.GetBins();
  }

  if (DoUnfolding) {
    std::vector<std::string> allvar;
    allvar.insert(std::end(allvar), std::begin(variation), std::end(variation));
    allvar.insert(std::end(allvar), std::begin(systematics), std::end(systematics));
    allvar.erase(allvar.begin());
//Do the unfolding
    if (runBlind) {
      UnfoldWrapper Wrapper = UnfoldWrapper("MET", "data", A_all, MET_all.at(0), fakes_all.at(0), v_misses_bkgs_Split.at(0), v_MET_bkgs, v_GenMET_bkgs, allvar, bkgnames, BinEdgesGen);
      Wrapper.DoIt();
      // for (auto& histo : GenMET_signal) Wrapper.writer.addToFile(histo);
    }
    else {
      // UnfoldWrapper Wrapper = UnfoldWrapper("MET", "data", A_all, MET_data, fakes_all.at(0), v_MET_bkgs, v_GenMET_bkgs, allvar, bkgnames, BinEdgesGen);
      UnfoldWrapper Wrapper = UnfoldWrapper("MET", "data", A_all, MET_data, fakes_all.at(0), v_misses_bkgs.at(0), v_MET_bkgs, v_GenMET_bkgs, unfoldedsysts, bkgnames, BinEdgesGen);
      Wrapper.DoIt();
      // for (auto& histo : GenMET_signal) Wrapper.writer.addToFile(histo);
    }
    // UnfoldWrapper Wrapper_Split = UnfoldWrapper("MET", "Split", A_all_Split,  MET_DummyData_all.at(0), fakes_all_Split.at(0), v_misses_bkgs_Split.at(0), v_MET_bkgs_Split, v_GenMET_bkgs_Split, allvar, bkgnames, BinEdgesGen);
    // Wrapper_Split.DoIt();
    // for (auto& histo : GenMET_signal_Split) Wrapper_Split.writer.addToFile(histo);

    // TH1F* InputwithSignal = (TH1F*) MET_DummyData_all.at(0)->Clone();
    // MET_signal.at(0)->Scale(0.1);
    // InputwithSignal->Add(MET_signal.at(0));

    // UnfoldWrapper Wrapper_Split_Signal = UnfoldWrapper("MET", "SplitSignal", A_all_Split,  InputwithSignal, fakes_all_Split.at(0), v_MET_bkgs_Split, v_GenMET_bkgs_Split, variation, bkgnames, BinEdgesGen);
    // Wrapper_Split_Signal.DoIt();
// Draw Stuff
    int nVariation = 0;
    bool moveUF = true;
    for (auto& var : allSystematics) {
      Drawer.Draw2D(A_all.at(nVariation), "A_all" + var, log, moveUF, "reconstructed #slash{E}_{T}", "generated #slash{E}_{T}");
      // Drawer.Draw2D(A_all_Split.at(nVariation), "A_all_" + var + "_Split", log, moveUF, "reconstructed #slash{E}_{T}", "generated #slash{E}_{T}");
      nVariation += 1;
    }
  }

  return (0);
}

#endif
