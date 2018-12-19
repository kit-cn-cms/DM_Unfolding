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

  TString GenVariableName;
  TString RecoVariableName;

  // book histos
  // Full Sample
  TH1F* h_Reco = 0;
  std::map<std::string, TH1F*> h_RecoSys;
  TH1F* h_Gen = 0;
  std::map<std::string, TH1F*> h_GenSys;
  TH1F* h_Data = 0;
  TH2D* A = 0;
  std::map<std::string, TH2D*> ASys;
  TH2D* A_equBins = 0;
  std::map<std::string, TH2D*> A_equBinsSys;

  // Split Sample
  TH1F* h_DummyDataSplit = 0;
  std::map<std::string, TH1F*> h_DummyDataSplitSys;
  TH1F* h_RecoSplit = 0;
  std::map<std::string, TH1F*> h_RecoSysSplit;
  TH1F* h_GenSplit = 0;
  std::map<std::string, TH1F*> h_GenSysSplit;
  TH2D* ASplit = 0;
  std::map<std::string, TH2D*> ASysSplit;
  TH1F* h_test = 0;
  std::map<std::string, TH1F*> h_testSys;
  TH1F* h_testgenBinning = 0;
  std::map<std::string, TH1F*> h_testgenBinningSys;
  TH1F* h_GenReco = 0;
  std::map<std::string, TH1F*> h_GenRecoSys;

  TH1F* h_test_Split = 0;
  std::map<std::string, TH1F*> h_test_SplitSys;
  TH1F* h_fake = 0;
  std::map<std::string, TH1F*> h_fakeSys;
  TH1F* h_fake_Split = 0;
  std::map<std::string, TH1F*> h_fake_SplitSys;
  TH1F* h_misses = 0;
  std::map<std::string, TH1F*> h_missesSys;
  TH1F* h_misses_Split = 0;
  std::map<std::string, TH1F*> h_misses_SplitSys;

  // Additional Histos
  TH1F* h_N_Jets = 0;
  std::map<std::string, TH1F*> h_N_JetsSys;
  TH1F* h_Jet_Pt = 0;
  std::map<std::string, TH1F*> h_Jet_PtSys;
  TH1F* h_Jet_Eta = 0;
  std::map<std::string, TH1F*> h_Jet_EtaSys;
  TH1F* h_Jet_Phi = 0;
  std::map<std::string, TH1F*> h_Jet_PhiSys;

  TH1F* h_Jet_Pt_0 = 0;
  std::map<std::string, TH1F*> h_Jet_Pt_0Sys;
  TH1F* h_Jet_Eta_0 = 0;
  std::map<std::string, TH1F*> h_Jet_Eta_0Sys;
  TH1F* h_Jet_Phi_0 = 0;
  std::map<std::string, TH1F*> h_Jet_Phi_0Sys;

  TH1F* h_N_JetsAK8 = 0;
  std::map<std::string, TH1F*> h_N_JetsAK8Sys;
  TH1F* h_Jet_PtAK8 = 0;
  std::map<std::string, TH1F*> h_Jet_PtAK8Sys;
  TH1F* h_Jet_EtaAK8 = 0;
  std::map<std::string, TH1F*> h_Jet_EtaAK8Sys;
  TH1F* h_Jet_PhiAK8 = 0;
  std::map<std::string, TH1F*> h_Jet_PhiAK8Sys;

  TH1F* h_Jet_PtAK8_0 = 0;
  std::map<std::string, TH1F*> h_Jet_PtAK8_0Sys;
  TH1F* h_Jet_EtaAK8_0 = 0;
  std::map<std::string, TH1F*> h_Jet_EtaAK8_0Sys;
  TH1F* h_Jet_PhiAK8_0 = 0;
  std::map<std::string, TH1F*> h_Jet_PhiAK8_0Sys;


  TH1F* h_Jet_Chf = 0;
  std::map<std::string, TH1F*> h_Jet_ChfSys;
  TH1F* h_Jet_Nhf = 0;
  std::map<std::string, TH1F*> h_Jet_NhfSys;



  TH1F* h_Evt_Phi_MET = 0;
  std::map<std::string, TH1F*> h_Evt_Phi_METSys;
  TH1F* h_Evt_Phi_GenMET = 0;
  std::map<std::string, TH1F*> h_Evt_Phi_GenMETSys;
  TH1F* h_W_Pt = 0;
  std::map<std::string, TH1F*> h_W_PtSys;
  TH1F* h_Z_Pt = 0;
  std::map<std::string, TH1F*> h_Z_PtSys;

  TH1F* h_N_looseLeptons = 0;
  std::map<std::string, TH1F*> h_N_looseLeptonsSys;

  TH1F* h_MET = 0;
  std::map<std::string, TH1F*> h_METSys;

  TH1F* h_CaloMET = 0;
  std::map<std::string, TH1F*> h_CaloMETSys;

  TH1F* h_CaloMET_PFMET_ratio = 0;
  std::map<std::string, TH1F*> h_CaloMET_PFMET_ratioSys;

  TH1F* h_dPhi_Jet_MET = 0;
  std::map<std::string, TH1F*> h_dPhi_Jet_METSys;

  TH1F* h_GenMET = 0;
  std::map<std::string, TH1F*> h_GenMETSys;

  TH1F* h_HadrRecoil = 0;
  std::map<std::string, TH1F*> h_HadrRecoilSys;

  TH1F* h_GenHadrRecoil = 0;
  std::map<std::string, TH1F*> h_GenHadrRecoilSys;

  TH1F* h_HadrRecoil_Phi = 0;
  std::map<std::string, TH1F*> h_HadrRecoil_PhiSys;

  TH1F* h_GenHadrRecoil_Phi = 0;
  std::map<std::string, TH1F*> h_GenHadrRecoil_PhiSys;
  /////////////////////////////
  // Choose/Add Variables here//
  /////////////////////////////
  // Readers to access the data (delete the ones you do not need).
  TTreeReaderValue<Float_t> MET = { fReader, "Evt_Pt_MET" };
  TTreeReaderValue<Float_t> GenMET = { fReader, "Evt_Pt_GenMET" };

  TTreeReaderValue<Float_t> Hadr_Recoil_Pt = { fReader, "Hadr_Recoil_Pt" };
  TTreeReaderValue<Float_t> Hadr_Recoil_Phi = { fReader, "Hadr_Recoil_Phi" };

  TTreeReaderValue<Float_t> Gen_Hadr_Recoil_Pt = { fReader, "Gen_Hadr_Recoil_Pt" };
  TTreeReaderValue<Float_t> Gen_Hadr_Recoil_Phi = { fReader, "Gen_Hadr_Recoil_Phi" };

  TTreeReaderValue<Float_t> W_Pt = { fReader, "W_Pt" };
  TTreeReaderValue<Float_t> Z_Pt = { fReader, "Z_Pt" };

  TTreeReaderValue<Long64_t> N_Jets = { fReader, "N_Jets" };
  TTreeReaderValue<Long64_t> N_LooseLeptons = { fReader, "N_LooseLeptons" };
  TTreeReaderValue<Long64_t> N_LooseElectrons = { fReader, "N_LooseElectrons" };
  TTreeReaderValue<Long64_t> N_LooseMuons = { fReader, "N_LooseMuons" };
  TTreeReaderValue<Long64_t> N_LooseTaus = { fReader, "N_LooseTaus" };

  TTreeReaderValue<Float_t> CaloMET = { fReader, "CaloMET" };
  TTreeReaderValue<Float_t> CaloMET_PFMET_ratio = { fReader, "CaloMET_PFMET_ratio" };
  TTreeReaderValue<Float_t> DeltaPhi_Jet_MET = { fReader, "DeltaPhi_Jet_MET" };

  // Additional Variables
  TTreeReaderValue<Float_t> Jet_Pt = { fReader, "Jet_Pt" };
  TTreeReaderValue<Float_t> Jet_Eta = { fReader, "Jet_Eta" };
  TTreeReaderValue<Float_t> Jet_Phi = { fReader, "Jet_Phi" };
  TTreeReaderValue<Float_t> Jet_Chf = { fReader, "Jet_Chf" };
  TTreeReaderValue<Float_t> Jet_Nhf = { fReader, "Jet_Nhf" };

  TTreeReaderValue<Long64_t> N_JetsAK8 = { fReader, "N_JetsAK8" };
  TTreeReaderValue<Float_t> Jet_PtAK8 = { fReader, "Jet_PtAK8" };
  TTreeReaderValue<Float_t> Jet_EtaAK8 = { fReader, "Jet_EtaAK8" };
  TTreeReaderValue<Float_t> Jet_PhiAK8 = { fReader, "Jet_PhiAK8" };

  TTreeReaderValue<Float_t> Evt_Phi_MET = { fReader, "Evt_Phi_MET" };
  TTreeReaderValue<Float_t> Evt_Phi_GenMET = { fReader, "Evt_Phi_GenMET" };


  Float_t varDeltaPhi_Jet_MET[100];
  TBranch *BrDeltaPhi_Jet_MET;

  Float_t varJet_Pt[100];
  TBranch *BrJet_Pt;

  Float_t varJet_Eta[100];
  TBranch *BrJet_Eta;

  Float_t varJet_Phi[100];
  TBranch *BrJet_Phi;

  Float_t varJet_CHS[100];
  TBranch *BrJet_CHS;

  Float_t varJet_NHF[100];
  TBranch *BrJet_NHF;

  Float_t varAK8Jet_Pt[100];
  TBranch *BrAK8Jet_Pt;

  Float_t varAK8Jet_Eta[100];
  TBranch *BrAK8Jet_Eta;

  Float_t varAK8Jet_Phi[100];
  TBranch *BrAK8Jet_Phi;

  TTreeReaderValue<Float_t> Weight_XS = { fReader, "Weight_XS" };
  TTreeReaderValue<Float_t> Weight_GenValue = { fReader, "Weight_GenValue" };
  TTreeReaderValue<Float_t> Weight_GEN_nom = { fReader, "Weight_GEN_nom" };
  TTreeReaderValue<Float_t> Weight_PU = { fReader, "Weight_pu69p2" };
  TTreeReaderValue<Float_t> Weight_CSV = { fReader, "Weight_CSV" };

  TTreeReaderValue<Float_t> Weight_PUup = { fReader, "Weight_pu69p2Up" };
  TTreeReaderValue<Float_t> Weight_PUdown = { fReader, "Weight_pu69p2Down" };

  TTreeReaderValue<Float_t> Weight_PDFup = { fReader, "Weight_LHA_292200_up" };
  TTreeReaderValue<Float_t> Weight_PDFdown = { fReader, "Weight_LHA_292200_down" };

  TTreeReaderValue<Float_t> Weight_MuRup = { fReader, "Weight_scale_variation_muR_2p0_muF_1p0" };
  TTreeReaderValue<Float_t> Weight_MuRdown = { fReader, "Weight_scale_variation_muR_0p5_muF_1p0" };
  TTreeReaderValue<Float_t> Weight_MuFup = { fReader, "Weight_scale_variation_muR_1p0_muF_2p0" };
  TTreeReaderValue<Float_t> Weight_MuFdown = { fReader, "Weight_scale_variation_muR_1p0_muF_0p5" };


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

  TTreeReaderValue<Float_t> BosonWeight_nominal = { fReader, "BosonWeight_nominal" };
  TTreeReaderValue<Float_t> BosonWeight_QCD1Up = { fReader, "BosonWeight_QCD1Up" };
  TTreeReaderValue<Float_t> BosonWeight_QCD1Down = { fReader, "BosonWeight_QCD1Down" };
  TTreeReaderValue<Float_t> BosonWeight_QCD2Up = { fReader, "BosonWeight_QCD2Up" };
  TTreeReaderValue<Float_t> BosonWeight_QCD2Down = { fReader, "BosonWeight_QCD2Down" };
  TTreeReaderValue<Float_t> BosonWeight_QCD3Up = { fReader, "BosonWeight_QCD3Up" };
  TTreeReaderValue<Float_t> BosonWeight_QCD3Down = { fReader, "BosonWeight_QCD3Down" };
  TTreeReaderValue<Float_t> BosonWeight_EW1Up = { fReader, "BosonWeight_EW1Up" };
  TTreeReaderValue<Float_t> BosonWeight_EW1Down = { fReader, "BosonWeight_EW1Down" };
  TTreeReaderValue<Float_t> BosonWeight_EW2Up = { fReader, "BosonWeight_EW2Up" };
  TTreeReaderValue<Float_t> BosonWeight_EW2Down = { fReader, "BosonWeight_EW2Down" };
  TTreeReaderValue<Float_t> BosonWeight_EW3Up = { fReader, "BosonWeight_EW3Up" };
  TTreeReaderValue<Float_t> BosonWeight_EW3Down = { fReader, "BosonWeight_EW3Down" };
  TTreeReaderValue<Float_t> BosonWeight_MixedUp = { fReader, "BosonWeight_MixedUp" };
  TTreeReaderValue<Float_t> BosonWeight_MixedDown = { fReader, "BosonWeight_MixedDown" };
  TTreeReaderValue<Float_t> BosonWeight_AlphaUp = { fReader, "BosonWeight_AlphaUp" };
  TTreeReaderValue<Float_t> BosonWeight_AlphaDown = { fReader, "BosonWeight_AlphaDown" };
  TTreeReaderValue<Float_t> BosonWeight_muRUp = { fReader, "BosonWeight_muRUp" };
  TTreeReaderValue<Float_t> BosonWeight_muRDown = { fReader, "BosonWeight_muRDown" };
  TTreeReaderValue<Float_t> BosonWeight_muFUp = { fReader, "BosonWeight_muFUp" };
  TTreeReaderValue<Float_t> BosonWeight_muFDown = { fReader, "BosonWeight_muFDown" };


  // TTreeReaderValue<Long64_t> Miss = {fReader, "Miss" };
  // TTreeReaderValue<Long64_t> Fake = {fReader, "Fake" };
  TTreeReaderValue<Long64_t> recoSelected = {fReader, "recoSelected" };
  // TTreeReaderValue<Long64_t> genSelected = {fReader, "genSelected" };

  TTreeReaderValue<Long64_t> GenMETSelection = { fReader, "GenMETSelection" };
  TTreeReaderValue<Long64_t> GenBTagVetoSelection = { fReader, "GenBTagVetoSelection" };
  TTreeReaderValue<Long64_t> GenMonoJetSelection = { fReader, "GenMonoJetSelection" };
  TTreeReaderValue<Long64_t> GenLeptonVetoSelection = {fReader, "GenLeptonVetoSelection" };
  TTreeReaderValue<Long64_t> GenmonoVselection = {fReader, "GenmonoVselection" };
  TTreeReaderValue<Long64_t> GenPhotonVetoSelection = {fReader, "GenPhotonVetoSelection" };


  TTreeReaderValue<Long64_t> VertexSelection = { fReader, "VertexSelection" };
  TTreeReaderValue<Long64_t> FilterSelection = { fReader, "FilterSelection" };
  TTreeReaderValue<Long64_t> METSelection = { fReader, "METSelection" };
  TTreeReaderValue<Long64_t> BTagVetoSelection = { fReader, "BTagVetoSelection" };
  TTreeReaderValue<Long64_t> MonoJetSelection = { fReader, "MonoJetSelection" };
  TTreeReaderValue<Long64_t> LeptonVetoSelection = {fReader, "LeptonVetoSelection" };
  TTreeReaderValue<Long64_t> monoVselection = {fReader, "monoVselection" };
  TTreeReaderValue<Long64_t> PhotonVetoSelection = {fReader, "PhotonVetoSelection" };

  TTreeReaderValue<Long64_t> Triggered_HLT_PFMET170_X = {fReader, "Triggered_HLT_PFMET170_X"};
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X" };
  TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X = { fReader, "Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X" };

  float weight_ = 1;

  std::map<std::string, TTreeReaderValue<Float_t> > sysweights = {
    {"Weight_PUUp", ((MCSelector*)this)->MCSelector::Weight_PUup},
    {"Weight_PUDown", ((MCSelector*)this)->MCSelector::Weight_PUdown},
    {"Weight_PDFUp", ((MCSelector*)this)->MCSelector::Weight_PDFup},
    {"Weight_PDFDown", ((MCSelector*)this)->MCSelector::Weight_PDFdown},
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
    {"CMS_btag_cferr2Down", ((MCSelector*)this)->MCSelector::Weight_CSVCErr2down},

    // {"BosonWeight_nominal", ((MCSelector*)this)->MCSelector::BosonWeight_nominal}
    {"BosonWeight_QCD1Up", ((MCSelector*)this)->MCSelector::BosonWeight_QCD1Up},
    {"BosonWeight_QCD1Down", ((MCSelector*)this)->MCSelector::BosonWeight_QCD1Down},
    {"BosonWeight_QCD2Up", ((MCSelector*)this)->MCSelector::BosonWeight_QCD2Up},
    {"BosonWeight_QCD2Down", ((MCSelector*)this)->MCSelector::BosonWeight_QCD2Down},
    {"BosonWeight_QCD3Up", ((MCSelector*)this)->MCSelector::BosonWeight_QCD3Up},
    {"BosonWeight_QCD3Down", ((MCSelector*)this)->MCSelector::BosonWeight_QCD3Down},
    {"BosonWeight_EW1Up", ((MCSelector*)this)->MCSelector::BosonWeight_EW1Up},
    {"BosonWeight_EW1Down", ((MCSelector*)this)->MCSelector::BosonWeight_EW1Down},
    {"BosonWeight_EW2Up", ((MCSelector*)this)->MCSelector::BosonWeight_EW2Up},
    {"BosonWeight_EW2Down", ((MCSelector*)this)->MCSelector::BosonWeight_EW2Down},
    {"BosonWeight_EW3Up", ((MCSelector*)this)->MCSelector::BosonWeight_EW3Up},
    {"BosonWeight_EW3Down", ((MCSelector*)this)->MCSelector::BosonWeight_EW3Down},
    {"BosonWeight_MixedUp", ((MCSelector*)this)->MCSelector::BosonWeight_MixedUp},
    {"BosonWeight_MixedDown", ((MCSelector*)this)->MCSelector::BosonWeight_MixedDown},
    {"BosonWeight_AlphaUp", ((MCSelector*)this)->MCSelector::BosonWeight_AlphaUp},
    {"BosonWeight_AlphaDown", ((MCSelector*)this)->MCSelector::BosonWeight_AlphaDown},
    {"BosonWeight_scale_variation_muRUp", ((MCSelector*)this)->MCSelector::BosonWeight_muRUp},
    {"BosonWeight_scale_variation_muRDown", ((MCSelector*)this)->MCSelector::BosonWeight_muRDown},
    {"BosonWeight_scale_variation_muFUp", ((MCSelector*)this)->MCSelector::BosonWeight_muFUp},
    {"BosonWeight_scale_variation_muFDown", ((MCSelector*)this)->MCSelector::BosonWeight_muFDown}

  };

  std::map<std::string, double> BosonSystematicWeights;

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