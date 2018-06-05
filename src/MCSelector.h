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
  TString strippedOption;
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
  std::vector<std::string> systematics;
  std::vector<std::string> BosonSystematics;
  std::vector<std::string> allSystematics;
  std::vector<double> BinEdgesGen;
  std::vector<double> BinEdgesReco;
  bool doadditionalsystematics = true;

  double failedGenMonoJetSelection = 0.;
  double failedGenPhotonVetoSelection = 0.;
  double failedGenLeptonVetoSelection = 0.;
  double failedGenBTagVetoSelection = 0.;
  double failedGenMETSelection = 0.;
  double failedGenmonoVselection = 0.;

  TFile *fWeightsW = 0;
  TFile *fWeightsZ = 0;

  // book histos
  // Full Sample
  TH1F* h_Reco = 0;
  std::map<std::string, TH1F*> h_RecoSys;
  TH1F* h_Gen = 0;
  std::map<std::string, TH1F*> h_GenSys;
  TH1F* h_Data = 0;
  std::map<std::string, TH2D*> ASys;
  TH2D* A = 0;
  TH2D* A_equBins = 0;
  // Split Sample
  TH1F* h_DummyDataSplit = 0;
  TH1F* h_RecoSplit = 0;
  std::map<std::string, TH1F*> h_RecoSysSplit;
  TH1F* h_GenSplit = 0;
  std::map<std::string, TH1F*> h_GenSysSplit;
  TH2D* ASplit = 0;
  std::map<std::string, TH2D*> ASysSplit;
  TH1F* h_testMET = 0;
  TH1F* h_testMETgenBinning = 0;

  TH1F* h_testMET_Split = 0;
  TH1F* h_fake = 0;
  TH1F* h_fake_Split = 0;
  TH1F* h_misses = 0;
  TH1F* h_misses_Split = 0;

  // Additional Histos
  TH1F* h_N_Jets = 0;
  TH1F* h_Jet_Pt = 0;
  TH1F* h_Jet_Eta = 0;
  TH1F* h_Evt_Phi_MET = 0;
  TH1F* h_Evt_Phi_GenMET = 0;
  TH1F* h_W_Pt = 0;
  TH1F* h_Z_Pt = 0;

  // histograms containing the scale factors for electron and muon channel separately
  TH1D* hWbosonWeight_nominal = 0;
  TH1D* hWbosonWeight_QCD1Up = 0;
  TH1D* hWbosonWeight_QCD1Down = 0;
  TH1D* hWbosonWeight_QCD2Up = 0;
  TH1D* hWbosonWeight_QCD2Down = 0;
  TH1D* hWbosonWeight_QCD3Up = 0;
  TH1D* hWbosonWeight_QCD3Down = 0;
  TH1D* hWbosonWeight_EW1Up = 0;
  TH1D* hWbosonWeight_EW1Down = 0;
  TH1D* hWbosonWeight_EW2Up = 0;
  TH1D* hWbosonWeight_EW2Down = 0;
  TH1D* hWbosonWeight_EW3Up = 0;
  TH1D* hWbosonWeight_EW3Down = 0;
  TH1D* hWbosonWeight_MixedUp = 0;
  TH1D* hWbosonWeight_MixedDown = 0;
  TH1D* hWbosonWeight_AlphaUp = 0;
  TH1D* hWbosonWeight_AlphaDown = 0;
  TH1D* hWbosonWeight_muRUp = 0;
  TH1D* hWbosonWeight_muRDown = 0;
  TH1D* hWbosonWeight_muFUp = 0;
  TH1D* hWbosonWeight_muFDown = 0;


  TH1D* hZbosonWeight_nominal = 0;
  TH1D* hZbosonWeight_QCD1Up = 0;
  TH1D* hZbosonWeight_QCD1Down = 0;
  TH1D* hZbosonWeight_QCD2Up = 0;
  TH1D* hZbosonWeight_QCD2Down = 0;
  TH1D* hZbosonWeight_QCD3Up = 0;
  TH1D* hZbosonWeight_QCD3Down = 0;
  TH1D* hZbosonWeight_EW1Up = 0;
  TH1D* hZbosonWeight_EW1Down = 0;
  TH1D* hZbosonWeight_EW2Up = 0;
  TH1D* hZbosonWeight_EW2Down = 0;
  TH1D* hZbosonWeight_EW3Up = 0;
  TH1D* hZbosonWeight_EW3Down = 0;
  TH1D* hZbosonWeight_MixedUp = 0;
  TH1D* hZbosonWeight_MixedDown = 0;
  TH1D* hZbosonWeight_AlphaUp = 0;
  TH1D* hZbosonWeight_AlphaDown = 0;
  TH1D* hZbosonWeight_muRUp = 0;
  TH1D* hZbosonWeight_muRDown = 0;
  TH1D* hZbosonWeight_muFUp = 0;
  TH1D* hZbosonWeight_muFDown = 0;


  double WbosonWeight_nominal = 1;
  double WbosonWeight_QCD1Up = 0;
  double WbosonWeight_QCD1Down = 0;
  double WbosonWeight_QCD2Up = 0;
  double WbosonWeight_QCD2Down = 0;
  double WbosonWeight_QCD3Up = 0;
  double WbosonWeight_QCD3Down = 0;
  double WbosonWeight_EW1Up = 0;
  double WbosonWeight_EW1Down = 0;
  double WbosonWeight_EW2Up = 0;
  double WbosonWeight_EW2Down = 0;
  double WbosonWeight_EW3Up = 0;
  double WbosonWeight_EW3Down = 0;
  double WbosonWeight_MixedUp = 0;
  double WbosonWeight_MixedDown = 0;
  double WbosonWeight_AlphaUp = 0;
  double WbosonWeight_AlphaDown = 0;
  double WbosonWeight_muRUp = 0;
  double WbosonWeight_muRDown = 0;
  double WbosonWeight_muFUp = 0;
  double WbosonWeight_muFDown = 0;


  double ZbosonWeight_nominal = 1;
  double ZbosonWeight_QCD1Up = 0;
  double ZbosonWeight_QCD1Down = 0;
  double ZbosonWeight_QCD2Up = 0;
  double ZbosonWeight_QCD2Down = 0;
  double ZbosonWeight_QCD3Up = 0;
  double ZbosonWeight_QCD3Down = 0;
  double ZbosonWeight_EW1Up = 0;
  double ZbosonWeight_EW1Down = 0;
  double ZbosonWeight_EW2Up = 0;
  double ZbosonWeight_EW2Down = 0;
  double ZbosonWeight_EW3Up = 0;
  double ZbosonWeight_EW3Down = 0;
  double ZbosonWeight_MixedUp = 0;
  double ZbosonWeight_MixedDown = 0;
  double ZbosonWeight_AlphaUp = 0;
  double ZbosonWeight_AlphaDown = 0;
  double ZbosonWeight_muRUp = 0;
  double ZbosonWeight_muRDown = 0;
  double ZbosonWeight_muFUp = 0;
  double ZbosonWeight_muFDown = 0;
  /////////////////////////////
  // Choose/Add Variables here//
  /////////////////////////////
  // Readers to access the data (delete the ones you do not need).
  TTreeReaderValue<Float_t> var_reco = { fReader, "Evt_Pt_MET" };
  TTreeReaderValue<Float_t> var_gen = { fReader, "Evt_Pt_GenMET" };

  TTreeReaderValue<Float_t> W_Pt = { fReader, "W_Pt" };
  TTreeReaderValue<Float_t> Z_Pt = { fReader, "Z_Pt" };

  TTreeReaderValue<Long64_t> N_Jets = { fReader, "N_Jets" };
  Float_t varDeltaPhi_Jet_MET[100];
  TBranch *BrDeltaPhi_Jet_MET;

  TTreeReaderValue<Float_t> Weight_XS = { fReader, "Weight_XS" };
  TTreeReaderValue<Float_t> Weight_GenValue = { fReader, "Weight_GenValue" };
  TTreeReaderValue<Float_t> Weight_GEN_nom = { fReader, "Weight_GEN_nom" };
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

  TTreeReaderValue<Long64_t> Miss = {fReader, "Miss" };
  TTreeReaderValue<Long64_t> Fake = {fReader, "Fake" };
  TTreeReaderValue<Long64_t> recoSelected = {fReader, "recoSelected" };
  TTreeReaderValue<Long64_t> genSelected = {fReader, "genSelected" };

  TTreeReaderValue<Long64_t> GenMETSelection = { fReader, "GenMETSelection" };
  TTreeReaderValue<Long64_t> GenBTagVetoSelection = { fReader, "GenBTagVetoSelection" };
  TTreeReaderValue<Long64_t> GenMonoJetSelection = { fReader, "GenMonoJetSelection" };
  TTreeReaderValue<Long64_t> GenLeptonVetoSelection = {fReader, "GenLeptonVetoSelection" };
  TTreeReaderValue<Long64_t> GenmonoVselection = {fReader, "GenmonoVselection" };
  TTreeReaderValue<Long64_t> GenPhotonVetoSelection = {fReader, "GenPhotonVetoSelection" };

  TTreeReaderValue<Long64_t> Triggered_HLT_PFMET170_X = {fReader, "Triggered_HLT_PFMET170_X"};
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X" };

  float weight_ = 1;

  std::map<std::string, TTreeReaderValue<Float_t> > sysweights = {
    {"Weight_PUUp", ((MCSelector*)this)->MCSelector::Weight_PUup},
    {"Weight_PUDown", ((MCSelector*)this)->MCSelector::Weight_PUdown},
    {"Weight_scale_variation_muRUp", ((MCSelector*)this)->MCSelector::Weight_MuRup},
    {"Weight_scale_variation_muRDown", ((MCSelector*)this)->MCSelector::Weight_MuRdown},
    {"Weight_scale_variation_muFUp", ((MCSelector*)this)->MCSelector::Weight_MuFup},
    {"Weight_scale_variation_muFDown", ((MCSelector*)this)->MCSelector::Weight_MuFdown},
    {"CMS_btag_lfUp", ((MCSelector*)this)->MCSelector::Weight_CSVLFup},
    {"CMS_btag_lfDown", ((MCSelector*)this)->MCSelector::Weight_CSVLFdown},
    {"CMS_btag_hfUp", ((MCSelector*)this)->MCSelector::Weight_CSVHFup},
    {"CMS_btag_hfDown", ((MCSelector*)this)->MCSelector::Weight_CSVHFdown},
    {"CMS_btag_hfstats1Up", ((MCSelector*)this)->MCSelector::Weight_CSVHFStats1up},
    {"CMS_btag_hfstats1Down", ((MCSelector*)this)->MCSelector::Weight_CSVHFStats1down},
    {"CMS_btag_lfstats1Up", ((MCSelector*)this)->MCSelector::Weight_CSVLFStats1up},
    {"CMS_btag_lfstats1Down", ((MCSelector*)this)->MCSelector::Weight_CSVLFStats1down},
    {"CMS_btag_hfstats2Up", ((MCSelector*)this)->MCSelector::Weight_CSVHFStats2up},
    {"CMS_btag_hfstats2Down", ((MCSelector*)this)->MCSelector::Weight_CSVHFStats2down},
    {"CMS_btag_lfstats2Up", ((MCSelector*)this)->MCSelector::Weight_CSVLFStats2up},
    {"CMS_btag_lfstats2Down", ((MCSelector*)this)->MCSelector::Weight_CSVLFStats2down},
    {"CMS_btag_cferr1Up", ((MCSelector*)this)->MCSelector::Weight_CSVCErr1up},
    {"CMS_btag_cferr1Down", ((MCSelector*)this)->MCSelector::Weight_CSVCErr1down},
    {"CMS_btag_cferr2Up", ((MCSelector*)this)->MCSelector::Weight_CSVCErr2up},
    {"CMS_btag_cferr2Down", ((MCSelector*)this)->MCSelector::Weight_CSVCErr2down}
  };

  std::map<std::string, double> BosonSystematicWeights;

  // Additional Variables
  TTreeReaderValue<Float_t> Jet_Pt = { fReader, "Jet_Pt" };
  TTreeReaderValue<Float_t> Jet_Eta = { fReader, "Jet_Eta" };

  TTreeReaderValue<Float_t> Evt_Phi_MET = { fReader, "Evt_Phi_MET" };
  TTreeReaderValue<Float_t> Evt_Phi_GenMET = { fReader, "Evt_Phi_GenMET" };



  TFile* histofile = 0;
  MCSelector(TTree * = 0) { }
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
//    // changes
//    // to the generated code, but the routine can be extended by the
//    // user if needed. The return value is currently not used.

//    return kTRUE;
// }

// #endif // #ifdef MCSelector_cxx