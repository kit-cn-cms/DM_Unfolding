//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Feb 10 11:06:02 2018 by ROOT version 6.06/01
// from TChain MVATree/
//////////////////////////////////////////////////////////

#ifndef PlotSelector_h
#define PlotSelector_h

#include "PathHelper.hpp"
#include "TH2D.h"
#include "TRandom3.h"
#include "boost/lexical_cast.hpp"
#include <TChain.h>
#include <TFile.h>
#include <TROOT.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Headers needed by this particular selector

class MCSelector : public TSelector
{
private:
  TH1F* h_GenMET = 0;

  TString outpath = "0";
  TString histofilepath = "0";
  TString ConfigPath = "0";

  PathHelper path;

public:
  TTreeReader fReader; //! the tree reader
  TTree* fChain = 0;   //! pointer to the analyzed TTree or TChain

  TString variation;
  int nBins_Gen;
  int nBins_Reco;
  int xMin_Gen;
  int xMax_Gen;
  int xMin_Reco;
  int xMax_Reco;
  int split;
  double nMax;
  TRandom rand;
  bool splitSignal;
  TString genvar;
  TString recovar;
  std::vector<std::string> bkgnames;
  std::vector<double> BinEdgesGen;
  std::vector<double> BinEdgesReco;

  // book histos
  // Full Sample
  TH1F* h_Reco = 0;
  TH1F* h_Gen = 0;
  TH1F* h_Data = 0;
  TH2D* A = 0;
  TH2D* A_equBins = 0;
  // Split Sample
  TH1F* h_DummyDataSplit = 0;
  TH1F* h_RecoSplit = 0;
  TH1F* h_GenSplit = 0;
  TH2D* ASplit = 0;
  TH1F* h_testMET = 0;
  TH1F* h_testMETgenBinning = 0;

  TH1F* h_testMET_Split = 0;
  TH1F* h_fake = 0;
  TH1F* h_fake_Split = 0;

  // Additional Histos
  TH1F* h_N_Jets = 0;
  TH1F* h_Jet_Pt = 0;
  TH1F* h_Jet_Eta = 0;
  TH1F* h_Evt_Phi_MET = 0;
  TH1F* h_Evt_Phi_GenMET = 0;
  /////////////////////////////
  // Choose/Add Variables here//
  /////////////////////////////
  // Readers to access the data (delete the ones you do not need).
  TTreeReaderValue<Float_t> var_reco = { fReader, "Evt_Pt_MET" };
  TTreeReaderValue<Float_t> var_gen = { fReader, "Evt_Pt_GenMET" };

  TTreeReaderValue<Float_t> Weight_XS = { fReader, "Weight_XS" };
  TTreeReaderValue<Float_t> Weight_GenValue = { fReader, "Weight_GenValue" };
  TTreeReaderValue<Float_t> Weight_PU = { fReader, "Weight_PU" };
  TTreeReaderValue<Float_t> Weight_CSV = { fReader, "Weight_CSV" };

  TTreeReaderValue<Float_t> Weight_PUup = { fReader, "Weight_pu69p2Up" };
  TTreeReaderValue<Float_t> Weight_PUdown = { fReader, "Weight_pu69p2Down" };

  TTreeReaderValue<Float_t> Weight_MuRup = { fReader, "Weight_scale_variation_muR_2p0_muF_1p0" };
  TTreeReaderValue<Float_t> Weight_MuRdown = { fReader, "Weight_scale_variation_muR_0p5_muF_1p0" };
  TTreeReaderValue<Float_t> Weight_MuFup = { fReader, "Weight_scale_variation_muR_1p0_muF_0p5" };
  TTreeReaderValue<Float_t> Weight_MuFdown = { fReader, "Weight_scale_variation_muR_1p0_muF_2p0" };


  TTreeReaderValue<Float_t> Weight_CSVLFup = { fReader, "Weight_CSVLFup" };
  TTreeReaderValue<Float_t> Weight_CSVLFdown = { fReader, "Weight_CSVLFdown" };
  TTreeReaderValue<Float_t> Weight_CSVHFup = { fReader, "Weight_CSVHFup" };
  TTreeReaderValue<Float_t> Weight_CSVHFdown = { fReader, "Weight_CSVHFdown" };
  TTreeReaderValue<Float_t> Weight_CSVHFStats1up = { fReader, "Weight_CSVHFStats1up" };
  TTreeReaderValue<Float_t> Weight_CSVHFStats1down = { fReader, "Weight_CSVHFStats1down" };
  TTreeReaderValue<Float_t> Weight_CSVLFStats1up = { fReader, "Weight_CSVLFStats1up" };
  TTreeReaderValue<Float_t> Weight_CSVLFStats1down = { fReader, "Weight_CSVLFStats1down" };
  TTreeReaderValue<Float_t> Weight_CSVHFStats2up = { fReader, "Weight_CSVHFStats2up" };
  TTreeReaderValue<Float_t> Weight_CSVHFStats2down = { fReader, "Weight_CSVHFStats2down" };
  TTreeReaderValue<Float_t> Weight_CSVLFStats2up = { fReader, "Weight_CSVLFStats2up" };
  TTreeReaderValue<Float_t> Weight_CSVLFStats2down = { fReader, "Weight_CSVLFStats2down" };
  TTreeReaderValue<Float_t> Weight_CSVCErr1up = { fReader, "Weight_CSVCErr1up" };
  TTreeReaderValue<Float_t> Weight_CSVCErr1down = { fReader, "Weight_CSVCErr1down" };
  TTreeReaderValue<Float_t> Weight_CSVCErr2up = { fReader, "Weight_CSVCErr2up" };
  TTreeReaderValue<Float_t> Weight_CSVCErr2down = { fReader, "Weight_CSVCErr2down" };

  TTreeReaderValue<Long64_t> GenMETSelection = { fReader, "GenMETSelection" };
  TTreeReaderValue<Long64_t> GenBTagVetoSelection = { fReader, "GenBTagVetoSelection" };
  TTreeReaderValue<Long64_t> GenMonoJetSelection = { fReader, "GenMonoJetSelection" };
  TTreeReaderValue<Long64_t> GenLeptonVetoSelection = {fReader, "GenLeptonVetoSelection" };

  TTreeReaderValue<Long64_t> Triggered_HLT_PFMET170_X = {fReader, "Triggered_HLT_PFMET170_X"};
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X" };

  float weight_ = 1;

  // Additional Variables
  TTreeReaderValue<Long64_t> N_Jets = { fReader, "N_Jets" };
  TTreeReaderValue<Float_t> Jet_Pt = { fReader, "Jet_Pt" };
  TTreeReaderValue<Float_t> Jet_Eta = { fReader, "Jet_Eta" };

  TTreeReaderValue<Float_t> Evt_Phi_MET = { fReader, "Evt_Phi_MET" };
  TTreeReaderValue<Float_t> Evt_Phi_GenMET = { fReader, "Evt_Phi_GenMET" };

  TFile* histofile = 0;

  MCSelector(TTree* /*tree*/ = 0) {}
  virtual ~MCSelector() {}
  virtual Int_t Version() const { return 2; }
  virtual void Begin(TTree* tree);
  virtual void SlaveBegin(TTree* tree);
  virtual void Init(TTree* tree);
  virtual Bool_t Notify();
  virtual Bool_t Process(Long64_t entry);
  virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0)
  {
    return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
  }
  virtual void SetOption(const char* option) { fOption = option; }
  virtual void SetObject(TObject* obj) { fObject = obj; }
  virtual void SetInputList(TList* input) { fInput = input; }
  virtual TList* GetOutputList() const { return fOutput; }
  virtual void SlaveTerminate();
  virtual void Terminate();

  ClassDef(MCSelector, 0);
};

#endif

// #ifdef MCSelector_cxx
// void MCSelector::Init(TTree *tree)
// {
//    // The Init() function is called when the selector needs to initialize
//    // a new tree or chain. Typically here the reader is initialized.
//    // It is normally not necessary to make changes to the generated
//    // code, but the routine can be extended by the user if needed.
//    // Init() will be called many times when running on PROOF
//    // (once per file to be processed).
//    // tree->Print();

// }

// Bool_t MCSelector::Notify()
// {
//    // The Notify() function is called when a new file is opened. This
//    // can be either for a new TTree in a TChain or when when a new TTree
//    // is started when using PROOF. It is normally not necessary to make
//    changes
//    // to the generated code, but the routine can be extended by the
//    // user if needed. The return value is currently not used.

//    return kTRUE;
// }

// #endif // #ifdef MCSelector_cxx