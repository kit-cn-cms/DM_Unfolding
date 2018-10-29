#define MCSelector_cxx
// The class definition in MCSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("MCSelector.C")
// root> T->Process("MCSelector.C","some options")
// root> T->Process("MCSelector.C+")
//

#include "MCSelector.h"
#include <TH1.h>
#include <TH2.h>

#include "TParameter.h"
#include "TRandom3.h"
#include "boost/lexical_cast.hpp"
#include <TCanvas.h>
#include <TStyle.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// #include "../interface/HistMaker.hpp"
// #include "PathHelper.hpp"
// #include "../interface/HistMaker.hpp"

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

// #ifdef MCSelector_cxx
void
MCSelector::Init(TTree* tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  // tree->Print();
  fReader.SetTree(tree);
  // tree->SetBranchAddress("DeltaPhi_Jet_MET", &varDeltaPhi_Jet_MET, &BrDeltaPhi_Jet_MET);
}

Bool_t
MCSelector::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}

// #endif // #ifdef MCSelector_cxx

void
MCSelector::Begin(TTree* /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  TString option = GetOption();
  histofile = new TFile(path.GetHistoFilePath(), "UPDATE");

  // TParameter<TString> *p =
  // dynamic_cast<TParameter<TString>*>(fInput->FindObject("outputpath"));
  // outpath = (p) ? (TString) p->GetVal() : outpath;
}

void
MCSelector::SlaveBegin(TTree* /*tree*/)
{

  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TH1::AddDirectory(kFALSE);
  TString option = GetOption();
  std::cout << "Processing: " << option << std::endl;
  std::cout << "Setting up Histos..." << std::endl;
  std::cout << "Parsing Hist Config..." << std::endl;
  ConfigPath = path.GetConfigPathforSlaves();
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini(std::string(ConfigPath), pt);

  bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));
  BinEdgesGen = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
  if (BinEdgesGen.size() == 3) {
    std::vector<double> tmp = BinEdgesGen;
    BinEdgesGen.clear();
    double BinWidth = abs(tmp.at(2) - tmp.at(1)) / tmp.at(0);
    for (int i = 0; i <= tmp.at(0); i++) {
      BinEdgesGen.push_back(tmp.at(1) + i * BinWidth);
    }
  }
  BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesReco"));
  if (BinEdgesReco.size() == 3) {
    std::vector<double> tmp = BinEdgesReco;
    BinEdgesReco.clear();
    double BinWidth = abs(tmp.at(2) - tmp.at(1)) / tmp.at(0);
    for (int i = 0; i <= tmp.at(0); i++) {
      BinEdgesReco.push_back(tmp.at(1) + i * BinWidth);
    }
  }
  nBins_Gen = BinEdgesGen.size() - 1;
  nBins_Reco = BinEdgesReco.size() - 1;

  variation = pt.get<std::string>("general.variation");
  // nBins_Gen = pt.get<int>("Binning.nBins_Gen");
  // nBins_Reco = pt.get<int>("Binning.nBins_Reco");
  // xMin_Gen = pt.get<int>("Binning.xMin_Gen");
  // xMax_Gen = pt.get<int>("Binning.xMax_Gen");
  // xMin_Reco = pt.get<int>("Binning.xMin_Reco");
  // xMax_Reco = pt.get<int>("Binning.xMax_Reco");
  split = pt.get<int>("general.split");
  systematics = to_array<std::string>(pt.get<std::string>("general.systematics"));
  BosonSystematics = to_array<std::string>(pt.get<std::string>("general.BosonSystematics"));
  std::cout << "Config parsed!" << std::endl;


  //append BosonSystematics to "common" systematics
  allSystematics.insert(std::end(allSystematics), std::begin(systematics), std::end(systematics));
  allSystematics.insert(std::end(allSystematics), std::begin(BosonSystematics), std::end(BosonSystematics));

  std::string currentJESJERvar = "";
  if (option.Contains("_CMS_scale_jUp")) {
    strippedOption = option.Copy().ReplaceAll("_CMS_scale_jUp", "");
    currentJESJERvar = "_CMS_scale_jUp";
  }
  else if (option.Contains("_CMS_scale_jDown")) {
    strippedOption = option.Copy().ReplaceAll("_CMS_scale_jDown", "");
    currentJESJERvar = "_CMS_scale_jDown";
  }
  else if (option.Contains("_CMS_res_jUp")) {
    strippedOption = option.Copy().ReplaceAll("_CMS_res_jUp", "");
    currentJESJERvar = "_CMS_res_jUp";
  }
  else if (option.Contains("_CMS_res_jDown")) {
    strippedOption = option.Copy().ReplaceAll("_CMS_res_jDown", "");
    currentJESJERvar = "_CMS_res_jDown";
  }
  else strippedOption = option;

  if (option.Contains("res") || option.Contains("scale") || option.Contains("data")) doadditionalsystematics = false;
  else doadditionalsystematics = true;

  std::cout << "doing additional systematics?? " << doadditionalsystematics << std::endl;

  TRandom* rand = new TRandom();

  auto bookSysHistos = [this](std::map<std::string, TH1F*> &histMap, TString name, TString var, int nBins, std::vector<double> BinEdges) {
    if (doadditionalsystematics) {
      for (auto& sys : allSystematics) {
        TH1F* tmp = new TH1F(name + sys, var, nBins, BinEdges.data());
        tmp->Sumw2();
        histMap[sys] = tmp;
        GetOutputList()->Add(histMap[sys]);
      }
    }
  };

  auto bookSysHistosequBins = [this](std::map<std::string, TH1F*> &histMap, TString name, TString var, int nBins, double xmin, double xmax) {
    if (doadditionalsystematics) {
      for (auto& sys : allSystematics) {
        TH1F* tmp = new TH1F(name + sys, var, nBins, xmin, xmax);
        tmp->Sumw2();
        histMap[sys] = tmp;
        GetOutputList()->Add(histMap[sys]);
      }
    }
  };

  auto bookSysHistos2D = [this](std::map<std::string, TH2D*> &histMap, TString name, TString var, int nBinsx, std::vector<double> BinEdgesx, int nBinsy, std::vector<double> BinEdgesy) {
    if (doadditionalsystematics) {
      for (auto& sys : allSystematics) {
        TH2D* tmp = new TH2D(name + sys, var, nBinsx, BinEdgesx.data(), nBinsy, BinEdgesy.data());
        tmp->Sumw2();
        histMap[sys] =  tmp;
        GetOutputList()->Add(histMap[sys] );
      }
    }
  };

  auto bookSysHistos2DequBins = [this](std::map<std::string, TH2D*> &histMap, TString name, TString var, int nBinsx, double xmin, double xmax, int nBinsy, double ymin, double ymax) {
    if (doadditionalsystematics) {
      for (auto& sys : allSystematics) {
        TH2D* tmp = new TH2D(name + sys, var, nBinsx, xmin, xmax, nBinsy, ymin, ymax);
        tmp->Sumw2();
        histMap[sys] = tmp;
        GetOutputList()->Add(histMap[sys]);
      }
    }
  };

  // book histos for split distributions
  h_GenSplit = new TH1F(strippedOption + "GenMET" + "_Split" + currentJESJERvar , "GenMET", nBins_Gen, BinEdgesGen.data());
  h_GenSplit->Sumw2();
  GetOutputList()->Add(h_GenSplit);
  bookSysHistos(h_GenSysSplit, strippedOption + "GenMET" + "_Split_" , "GenMET", nBins_Gen, BinEdgesGen);

  h_RecoSplit = new TH1F(strippedOption + "MET" + "_Split" + currentJESJERvar, "MET", nBins_Reco, BinEdgesReco.data());
  h_RecoSplit->Sumw2();
  GetOutputList()->Add(h_RecoSplit);
  bookSysHistos(h_RecoSysSplit, strippedOption + "MET" + "_Split_" , "MET", nBins_Reco, BinEdgesReco);


  h_DummyDataSplit = new TH1F(strippedOption + "DummyData_Split" +  currentJESJERvar, "DummyData", nBins_Reco, BinEdgesReco.data());
  h_DummyDataSplit->Sumw2();
  GetOutputList()->Add(h_DummyDataSplit);
  bookSysHistos(h_DummyDataSplitSys, strippedOption + "DummyData_Split_" , "DummyData", nBins_Reco, BinEdgesReco);


  ASplit = new TH2D(strippedOption + "A_Split" + currentJESJERvar, "A", nBins_Reco, BinEdgesReco.data(), nBins_Gen, BinEdgesGen.data());
  ASplit->Sumw2();
  GetOutputList()->Add(ASplit);
  bookSysHistos2D(ASysSplit, strippedOption + "A_Split_", "A", nBins_Reco, BinEdgesReco, nBins_Gen, BinEdgesGen);


  // book histos for full distributions
  h_Gen = new TH1F(strippedOption + "GenMET" + currentJESJERvar, "GenMET", nBins_Gen, BinEdgesGen.data());
  h_Gen->Sumw2();
  GetOutputList()->Add(h_Gen);
  bookSysHistos(h_GenSys, strippedOption + "GenMET" + "_" , "GenMET", nBins_Gen, BinEdgesGen);

  h_Reco = new TH1F(strippedOption + "MET" + currentJESJERvar, "MET", nBins_Reco, BinEdgesReco.data());
  h_Reco->Sumw2();
  GetOutputList()->Add(h_Reco);
  bookSysHistos(h_RecoSys, strippedOption + "MET" + "_" , "MET", nBins_Reco, BinEdgesReco);

  h_HadrRecoil = new TH1F(strippedOption + "Hadr_Recoil_Pt" + currentJESJERvar, "Hadr_Recoil_Pt", nBins_Reco, BinEdgesReco.data());
  h_HadrRecoil->Sumw2();
  GetOutputList()->Add(h_HadrRecoil);
  bookSysHistos(h_HadrRecoilSys, strippedOption + "Hadr_Recoil_Pt_" , "Hadr_Recoil_Pt", nBins_Reco, BinEdgesReco);


  A = new TH2D(strippedOption + "A" + currentJESJERvar , "A", nBins_Reco, BinEdgesReco.data(), nBins_Gen, BinEdgesGen.data());
  A->Sumw2();
  GetOutputList()->Add(A);
  bookSysHistos2D(ASys, strippedOption + "A_", "A", nBins_Reco, BinEdgesReco, nBins_Gen, BinEdgesGen);


  A_equBins = new TH2D(strippedOption + "A_equBins" + currentJESJERvar, "A", 250, 250, 2000, 250, 250, 2000);
  A_equBins->Sumw2();
  GetOutputList()->Add(A_equBins);
  bookSysHistos2DequBins(A_equBinsSys, strippedOption + "A_equBins_", "A",  250, 250, 2000, 250, 250, 2000);


  h_testMET = new TH1F(strippedOption + "TestMET" + currentJESJERvar, "TESTMET", nBins_Reco, BinEdgesReco.data());
  h_testMET->Sumw2();
  GetOutputList()->Add(h_testMET);
  bookSysHistos(h_testMETSys, strippedOption + "TestMET_" , "TESTMET", nBins_Reco, BinEdgesReco);

  h_GenRecoMET = new TH1F(strippedOption + "h_GenRecoMET" + currentJESJERvar, "h_GenRecoMET", nBins_Reco, BinEdgesReco.data());
  h_GenRecoMET->Sumw2();
  GetOutputList()->Add(h_GenRecoMET);
  bookSysHistos(h_GenRecoMETSys, strippedOption + "h_GenRecoMET_" , "h_GenRecoMET", nBins_Reco, BinEdgesReco);


  h_testMETgenBinning = new TH1F(strippedOption + "testMETgenBinning" + currentJESJERvar, "testMETgenBinning", nBins_Gen, BinEdgesGen.data());
  h_testMETgenBinning->Sumw2();
  GetOutputList()->Add(h_testMETgenBinning);
  bookSysHistos(h_testMETgenBinningSys, strippedOption + "testMETgenBinning_" , "testMETgenBinning", nBins_Gen, BinEdgesGen);

  h_testMET_Split = new TH1F(strippedOption + "TestMET_Split" + currentJESJERvar, "TESTMET", nBins_Reco, BinEdgesReco.data());
  h_testMET_Split->Sumw2();
  GetOutputList()->Add(h_testMET_Split);
  bookSysHistos(h_testMET_SplitSys, strippedOption + "TestMET_Split_" , "TESTMET", nBins_Reco, BinEdgesReco);

  h_fake = new TH1F(strippedOption + "fakes" + currentJESJERvar, "fakes", nBins_Reco, BinEdgesReco.data());
  h_fake->Sumw2();
  GetOutputList()->Add(h_fake);
  bookSysHistos(h_fakeSys, strippedOption + "fakes_" , "fakes", nBins_Reco, BinEdgesReco);

  h_fake_Split = new TH1F(strippedOption + "fakes_Split" + currentJESJERvar , "MET", nBins_Reco, BinEdgesReco.data());
  h_fake_Split->Sumw2();
  GetOutputList()->Add(h_fake_Split);
  bookSysHistos(h_fake_SplitSys, strippedOption + "fakes_Split_" , "fakes", nBins_Reco, BinEdgesReco);

  h_misses = new TH1F(strippedOption + "misses" + currentJESJERvar, "misses", nBins_Gen, BinEdgesGen.data());
  h_misses->Sumw2();
  GetOutputList()->Add(h_misses);
  bookSysHistos(h_missesSys, strippedOption + "misses_" , "misses", nBins_Gen, BinEdgesGen);

  h_misses_Split = new TH1F(strippedOption + "misses_Split" + currentJESJERvar, "GenMET",  nBins_Gen, BinEdgesGen.data());
  h_misses_Split->Sumw2();
  GetOutputList()->Add(h_misses_Split);
  bookSysHistos(h_misses_SplitSys, strippedOption + "misses_Split_" , "misses", nBins_Gen, BinEdgesGen);

  // Additional Variables
  h_N_Jets = new TH1F(strippedOption + "N_Jets" + currentJESJERvar, "N_Jets", 15, 0, 15);
  h_N_Jets->Sumw2();
  GetOutputList()->Add(h_N_Jets);
  bookSysHistosequBins(h_N_JetsSys, strippedOption + "N_Jets" , "N_Jets", 15, 0, 15);

  h_Jet_Pt = new TH1F(strippedOption + "Jet_Pt" + currentJESJERvar, "Jet_Pt", 80, 0, 800);
  h_Jet_Pt->Sumw2();
  GetOutputList()->Add(h_Jet_Pt);
  bookSysHistosequBins(h_Jet_PtSys, strippedOption + "Jet_Pt_" , "Jet_Pt", 80, 0, 800);

  h_Jet_Eta = new TH1F(strippedOption + "Jet_Eta" + currentJESJERvar, "Jet_Eta", 40, -4, 4);
  h_Jet_Eta->Sumw2();
  GetOutputList()->Add(h_Jet_Eta);
  bookSysHistosequBins(h_Jet_EtaSys, strippedOption + "Jet_Eta_" , "Jet_Eta", 40, -4, 4);

  h_Evt_Phi_MET = new TH1F(strippedOption + "Evt_Phi_MET" + currentJESJERvar, "Evt_Phi_MET", 50, -3.2, 3.2);
  h_Evt_Phi_MET->Sumw2();
  GetOutputList()->Add(h_Evt_Phi_MET);
  bookSysHistosequBins(h_Evt_Phi_METSys, strippedOption + "Evt_Phi_MET_" , "Evt_Phi_MET", 40, -4, 4);

  h_Evt_Phi_GenMET = new TH1F(strippedOption + "Evt_Phi_GenMET" + currentJESJERvar, "Evt_Phi_GenMET", 50, -3.2, 3.2);
  h_Evt_Phi_GenMET->Sumw2();
  GetOutputList()->Add(h_Evt_Phi_GenMET);
  bookSysHistosequBins(h_Evt_Phi_GenMETSys, strippedOption + "Evt_Phi_GenMET_" , "Evt_Phi_GenMET", 50, -3.2, 3.2);

  h_W_Pt = new TH1F(strippedOption + "h_W_Pt" + currentJESJERvar, "h_W_Pt", 60, 0, 1200);
  h_W_Pt->Sumw2();
  GetOutputList()->Add(h_W_Pt);
  bookSysHistosequBins(h_W_PtSys, strippedOption + "h_W_Pt_" , "h_W_Pt", 60, 0, 1200);

  h_Z_Pt = new TH1F(strippedOption + "h_Z_Pt" + currentJESJERvar, "h_Z_Pt", 60, 0, 1200);
  h_Z_Pt->Sumw2();
  GetOutputList()->Add(h_Z_Pt);
  bookSysHistosequBins(h_Z_PtSys, strippedOption + "h_Z_Pt_" , "h_Z_Pt", 60, 0, 1200);

  h_HadrRecoil_Phi = new TH1F(strippedOption + "Hadr_Recoil_Phi" + currentJESJERvar, "Hadr_Recoil_Phi", 50, -3.2, 3.2);
  h_HadrRecoil_Phi->Sumw2();
  GetOutputList()->Add(h_HadrRecoil_Phi);
  bookSysHistosequBins(h_HadrRecoil_PhiSys, strippedOption + "Hadr_Recoil_Phi_" , "Hadr_Recoil_Phi", 40, -4, 4);


  std::cout << "All Histos SetUp!" << std::endl;
}

Bool_t
MCSelector::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // When processing keyed objects with PROOF, the object is already loaded
  // and is available via the fObject pointer.
  //
  // This function should contain the \"body\" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.
  TString option = GetOption();
  fReader.SetLocalEntry(entry);

  // BrDeltaPhi_Jet_MET->GetEntry(entry);
  //////////////////////
  // Add weights here!!//
  //////////////////////
  bool isWlnuSample = option.Contains("w_lnu_jets");
  bool isZnunuSample = option.Contains("z_nunu_jets");
  bool isZllSample = option.Contains("z_ll_jets");

  bool applyBosonWeights = isWlnuSample or isZnunuSample or isZllSample;


  weight_ = (*Weight_XS) * (*Weight_CSV) * (*Weight_PU) * (*Weight_GEN_nom);

  if (applyBosonWeights) weight_ *= *BosonWeight_nominal;

  double random = rand.Rndm();
  float split_ = split / 100.;
  bool isPseudoData = random < split_;

  if (!option.Contains("data")) {
    weight_ *= 35.91823;
  }

  if (isZnunuSample) {
    weight_ *= 3 * 0.971;
  }
  if (isWlnuSample) {
  }
  if (option.Contains("signal")) {
    weight_ *= 0.7826;
  }

  auto fillSys = [this, applyBosonWeights](std::map<std::string, TH1F*> &histMap, double var) {
    if (doadditionalsystematics) { //fill systematics only in nominal case
      for (auto& sys : allSystematics ) {
        // std::cout << sys << std::endl;
        float tmpweight = weight_ * *(sysweights.find(sys)->second);
        if (TString(sys).Contains("Boson") and applyBosonWeights) tmpweight /= *BosonWeight_nominal;
        if (TString(sys).Contains("PU")) tmpweight /= *Weight_PU;
        else if (TString(sys).Contains("PDF")) {
          if (*(sysweights.find(sys)->second) > 0) {
            tmpweight = weight_ * *(sysweights.find(sys)->second);
          }
          else {
            tmpweight = weight_;
          }
        }
        else if (TString(sys).Contains("mu")) {
          if (applyBosonWeights) tmpweight = weight_ * fabs(*(sysweights.find(sys)->second)) * (*(sysweights.find("Boson" + sys)->second)) / *BosonWeight_nominal;
          else tmpweight = weight_ * fabs(*(sysweights.find(sys)->second));
        }
        histMap.find(sys)->second->Fill( var, tmpweight );
        // std::cout <<"did " << sys << std::endl;
      }
    }
  };

  auto fillSys2D = [this, applyBosonWeights](std::map<std::string, TH2D*> &histMap, double varx, double vary) {
    if (doadditionalsystematics) { //fill systematics only in nominal case
      for (auto& sys : allSystematics ) {
        float tmpweight = weight_ * *(sysweights.find(sys)->second);
        if (TString(sys).Contains("Boson") and applyBosonWeights) tmpweight /= *BosonWeight_nominal;
        if (TString(sys).Contains("PU")) tmpweight /= *Weight_PU;
        else if (TString(sys).Contains("PDF")) {
          if (*(sysweights.find(sys)->second) > 0) {
            tmpweight = weight_ * *(sysweights.find(sys)->second);
          }
          else {
            tmpweight = weight_;
          }
        }
        else if (TString(sys).Contains("mu")) {
          if (applyBosonWeights) tmpweight = weight_ * fabs(*(sysweights.find(sys)->second)) * (*(sysweights.find("Boson" + sys)->second)) / *BosonWeight_nominal;
          else tmpweight = weight_ * fabs(*(sysweights.find(sys)->second));
        }
        histMap.find(sys)->second->Fill( varx, vary, tmpweight );
      }
    }
  };

  bool triggered = *Triggered_HLT_PFMET170_X || *Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X || *Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X || *Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X || *Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X;

  bool gensel = *GenMET > BinEdgesGen.at(0) && *GenMETSelection  && *GenMonoJetSelection && *GenLeptonVetoSelection && *GenBTagVetoSelection && *GenPhotonVetoSelection &&  *GenmonoVselection;
  // bool gensel = *GenMET > BinEdgesGen.at(0) && *GenMETSelection  && *GenMonoJetSelection && *GenLeptonVetoSelection;
  bool recosel = triggered && *MET > BinEdgesReco.at(0) && *METSelection && *MonoJetSelection &&  *LeptonVetoSelection && *BTagVetoSelection && *PhotonVetoSelection && *monoVselection && *FilterSelection && *VertexSelection;

  bool miss = gensel && !recosel;

  // if ((abs(*MET - *GenMET) < 150) or (option.Contains("data"))) {
  if (gensel) {
    h_Gen->Fill(*GenMET, weight_);
    fillSys(h_GenSys, *GenMET);
    h_GenRecoMET->Fill(*MET, weight_);
    fillSys(h_GenRecoMETSys, *MET);

    if (!isPseudoData) {
      h_GenSplit->Fill(*GenMET, weight_);
      fillSys(h_GenSysSplit, *GenMET);
    }

    if (recosel) {
      h_testMET->Fill(*MET, weight_);
      fillSys(h_testMETSys, *MET);
      h_testMETgenBinning->Fill(*MET, weight_);
      fillSys(h_testMETgenBinningSys, *MET);

      if (!isPseudoData) {
        h_testMET_Split->Fill(*MET, weight_);
        fillSys(h_testMET_SplitSys, *MET);
      }
    }

  }

  if (miss) { //is a miss
    A->Fill(-10, *GenMET, weight_);
    fillSys2D(ASys, -10, *GenMET );
    h_misses->Fill(*GenMET, weight_);
    fillSys(h_missesSys, *GenMET);
    if (!isPseudoData) {
      h_misses_Split->Fill(*GenMET, weight_);
      fillSys(h_misses_SplitSys, *GenMET);

      ASplit->Fill(-10, *GenMET, weight_);
      fillSys2D(ASysSplit, -10, *GenMET );
    }
  }


  if (recosel) { // "normal" selection on reco level
    h_Reco->Fill(*MET, weight_);
    fillSys(h_RecoSys, *MET);
    // Additional Variables
    h_N_Jets->Fill(*N_Jets, weight_);
    fillSys(h_RecoSys, *N_Jets);

    h_Jet_Pt->Fill(*Jet_Pt, weight_);
    fillSys(h_Jet_PtSys, *Jet_Pt);

    h_Jet_Eta->Fill(*Jet_Eta, weight_);
    fillSys(h_Jet_EtaSys, *Jet_Eta);

    h_Evt_Phi_MET->Fill(*Evt_Phi_MET, weight_);
    fillSys(h_Evt_Phi_METSys, *Evt_Phi_MET);

    h_Evt_Phi_GenMET->Fill(*Evt_Phi_GenMET, weight_);
    fillSys(h_Evt_Phi_GenMETSys, *Evt_Phi_GenMET);

    h_W_Pt->Fill(*W_Pt, weight_);
    fillSys(h_W_PtSys, *W_Pt);

    h_Z_Pt->Fill(*Z_Pt, weight_);
    fillSys(h_Z_PtSys, *Z_Pt);

    h_HadrRecoil->Fill(*Hadr_Recoil_Pt, weight_);
    fillSys(h_HadrRecoilSys, *Hadr_Recoil_Pt);

    h_HadrRecoil_Phi->Fill(*Hadr_Recoil_Phi, weight_);
    fillSys(h_HadrRecoil_PhiSys, *Hadr_Recoil_Phi);

    if (isPseudoData) {
      h_RecoSplit->Fill(*MET, weight_);
      fillSys(h_RecoSysSplit, *MET);

      h_DummyDataSplit->Fill(*MET, weight_);
      fillSys(h_DummyDataSplitSys, *MET);
    }

    if (!option.Contains("data")) { // dont ask for gen stuff in case of data
      if (!gensel) { //is a fake
        h_fake->Fill(*MET, weight_);
        fillSys(h_fakeSys, *MET);
        if (!*GenMonoJetSelection) failedGenMonoJetSelection += 1 * weight_;
        if (!*GenLeptonVetoSelection) failedGenLeptonVetoSelection += 1 * weight_;
        if (!*GenBTagVetoSelection) failedGenBTagVetoSelection += 1 * weight_;
        if (!*GenPhotonVetoSelection) failedGenPhotonVetoSelection += 1 * weight_;
        if (!*GenMETSelection) failedGenMETSelection += 1 * weight_;
        if (!*GenmonoVselection) failedGenmonoVselection += 1 * weight_;

        if (!isPseudoData) {
          h_fake_Split->Fill(*MET, weight_);
          fillSys(h_fake_SplitSys, *MET);
        }
      }
      else if (gensel) { //is not a fake
        A_equBins->Fill(*MET, *GenMET, weight_);
        fillSys2D(A_equBinsSys, *MET, *GenMET );
        A->Fill(*MET, *GenMET, weight_);
        fillSys2D(ASys, *MET, *GenMET );
      }
    }
  }
  // }
  return kTRUE;
}


void
MCSelector::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.
  std::cout << "failedGenMonoJetSelection " << failedGenMonoJetSelection << std::endl;
  std::cout << "failedGenLeptonVetoSelection " << failedGenLeptonVetoSelection << std::endl;
  std::cout << "failedGenBTagVetoSelection " << failedGenBTagVetoSelection << std::endl;
  std::cout << "failedGenMETSelection " << failedGenMETSelection << std::endl;
  std::cout << "failedGenmonoVselection " << failedGenmonoVselection << std::endl;
  std::cout << "failedGenPhotonVetoSelection " << failedGenPhotonVetoSelection << std::endl;
  std::cout << " Slave finished" << std::endl;
}

void
MCSelector::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  std::cout << "Input List:" << std::endl;
  GetInputList()->ls();
  std::cout << "Output List:" << std::endl;
  GetOutputList()->ls();
  TString option = GetOption();

  for (const auto && obj : * (GetOutputList())) {
    // obj->Write();
    histofile->WriteTObject(obj);
  }

  histofile->Close();
  delete histofile;
  std::cout << "Master finished" << std::endl;
}