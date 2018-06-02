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
  nBins_Gen = BinEdgesGen.size() - 1;

  genvar = pt.get<std::string>("vars.gen");
  recovar = pt.get<std::string>("vars.reco");
  variation = pt.get<std::string>("general.variation");
  // nBins_Gen = pt.get<int>("Binning.nBins_Gen");
  nBins_Reco = pt.get<int>("Binning.nBins_Reco");
  xMin_Gen = pt.get<int>("Binning.xMin_Gen");
  xMax_Gen = pt.get<int>("Binning.xMax_Gen");
  xMin_Reco = pt.get<int>("Binning.xMin_Reco");
  xMax_Reco = pt.get<int>("Binning.xMax_Reco");
  split = pt.get<int>("general.split");
  systematics = to_array<std::string>(pt.get<std::string>("general.systematics"));
  BosonSystematics = to_array<std::string>(pt.get<std::string>("general.BosonSystematics"));

  TString WFileName = pt.get<std::string>("Boson.WFileName");
  TString ZFileName = pt.get<std::string>("Boson.ZFileName");
  std::cout << "Config parsed!" << std::endl;


  //append BosonSystematics to "common" systematics
  allSystematics.insert(std::end(allSystematics), std::begin(systematics), std::end(systematics));
  allSystematics.insert(std::end(allSystematics), std::begin(BosonSystematics), std::end(BosonSystematics));

  TString path_to_sf_file_W = path.GetRootFilePathforSlaves() + WFileName;
  TString path_to_sf_file_Z = path.GetRootFilePathforSlaves() + ZFileName;

  fWeightsW = TFile::Open(path_to_sf_file_W);
  hWbosonWeight_nominal =   (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nnn_n");
  hWbosonWeight_QCD1Up =    (TH1D*)fWeightsW->Get("evj_NNLO_NLO_unn_nnn_n");
  hWbosonWeight_QCD1Down =  (TH1D*)fWeightsW->Get("evj_NNLO_NLO_dnn_nnn_n");
  hWbosonWeight_QCD2Up =    (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nun_nnn_n");
  hWbosonWeight_QCD2Down =  (TH1D*)fWeightsW->Get("evj_NNLO_NLO_ndn_nnn_n");
  hWbosonWeight_QCD3Up =    (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnu_nnn_n");
  hWbosonWeight_QCD3Down =  (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnd_nnn_n");
  hWbosonWeight_EW1Up =     (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_unn_n");
  hWbosonWeight_EW1Down =   (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_dnn_n");
  hWbosonWeight_EW2Up =     (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nun_n");
  hWbosonWeight_EW2Down =   (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_ndn_n");
  hWbosonWeight_EW3Up =     (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nnu_n");
  hWbosonWeight_EW3Down =   (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nnd_n");
  hWbosonWeight_MixedUp =   (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nnn_u");
  hWbosonWeight_MixedDown = (TH1D*)fWeightsW->Get("evj_NNLO_NLO_nnn_nnn_d");

  fWeightsZ = TFile::Open(path_to_sf_file_Z);
  hZbosonWeight_nominal =   (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nnn_n");
  hZbosonWeight_QCD1Up =    (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_unn_nnn_n");
  hZbosonWeight_QCD1Down =  (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_dnn_nnn_n");
  hZbosonWeight_QCD2Up =    (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nun_nnn_n");
  hZbosonWeight_QCD2Down =  (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_ndn_nnn_n");
  hZbosonWeight_QCD3Up =    (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnu_nnn_n");
  hZbosonWeight_QCD3Down =  (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnd_nnn_n");
  hZbosonWeight_EW1Up =     (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_unn_n");
  hZbosonWeight_EW1Down =   (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_dnn_n");
  hZbosonWeight_EW2Up =     (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nun_n");
  hZbosonWeight_EW2Down =   (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_ndn_n");
  hZbosonWeight_EW3Up =     (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nnu_n");
  hZbosonWeight_EW3Down =   (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nnd_n");
  hZbosonWeight_MixedUp =   (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nnn_u");
  hZbosonWeight_MixedDown = (TH1D*)fWeightsZ->Get("vvj_NNLO_NLO_nnn_nnn_d");

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

  if (option.Contains("res") || option.Contains("scale")) doadditionalsystematics = false;
  else doadditionalsystematics = true;
  std::cout << "doing additional systematics?? " << doadditionalsystematics << std::endl;

  TRandom* rand = new TRandom();
  // book histos for split distributions
  h_GenSplit = new TH1F(strippedOption + genvar + currentJESJERvar + "_Split", genvar, nBins_Gen, BinEdgesGen.data());
  h_GenSplit->Sumw2();
  GetOutputList()->Add(h_GenSplit);
  h_RecoSplit = new TH1F(strippedOption + recovar + currentJESJERvar + "_Split", recovar, nBins_Reco, xMin_Reco, xMax_Reco);
  h_RecoSplit->Sumw2();
  GetOutputList()->Add(h_RecoSplit);
  h_DummyDataSplit = new TH1F(strippedOption + "DummyData" +  currentJESJERvar + "_Split", "DummyData", nBins_Reco, xMin_Reco, xMax_Reco);
  h_DummyDataSplit->Sumw2();
  GetOutputList()->Add(h_DummyDataSplit);

  ASplit = new TH2D(strippedOption + "A" + currentJESJERvar + "_Split", "A", nBins_Reco, xMin_Reco, xMax_Reco, nBins_Gen, BinEdgesGen.data());
  ASplit->Sumw2();
  GetOutputList()->Add(ASplit);

  // book histos for full distributions
  h_Gen = new TH1F(strippedOption + genvar + currentJESJERvar, genvar, nBins_Gen, BinEdgesGen.data());
  h_Gen->Sumw2();
  GetOutputList()->Add(h_Gen);
  h_Reco = new TH1F(strippedOption + recovar + currentJESJERvar, recovar, nBins_Reco, xMin_Reco, xMax_Reco);
  h_Reco->Sumw2();
  GetOutputList()->Add(h_Reco);

  A = new TH2D(strippedOption + "A" + currentJESJERvar , "A", nBins_Reco, xMin_Reco, xMax_Reco, nBins_Gen, BinEdgesGen.data());
  A->Sumw2();
  GetOutputList()->Add(A);

  if (doadditionalsystematics) {

    for (auto& sys : allSystematics) {
      TH2D* tmp = new TH2D(strippedOption + "A_" + sys, "A", nBins_Reco, xMin_Reco, xMax_Reco, nBins_Gen, BinEdgesGen.data());
      tmp->Sumw2();
      ASys[sys] = tmp;
      GetOutputList()->Add(tmp);

      TH1F* Recotmp = new TH1F(strippedOption + recovar + "_" + sys, recovar, nBins_Reco, xMin_Reco, xMax_Reco);
      Recotmp->Sumw2();
      h_RecoSys[sys] = Recotmp;
      GetOutputList()->Add(Recotmp);

      TH1F* Gentmp = new TH1F(strippedOption + genvar + "_" + sys, genvar, nBins_Gen, BinEdgesGen.data());
      Gentmp->Sumw2();
      h_GenSys[sys] = Gentmp;
      GetOutputList()->Add(Gentmp);



      TH2D* tmpSplit = new TH2D(strippedOption + "A_" + sys + "_Split", "A", nBins_Reco, xMin_Reco, xMax_Reco, nBins_Gen, BinEdgesGen.data());
      tmpSplit->Sumw2();
      ASysSplit[sys] = tmp;
      GetOutputList()->Add(tmpSplit);

      TH1F* RecotmpSplit = new TH1F(strippedOption + recovar + "_" + sys + "_Split", recovar, nBins_Reco, xMin_Reco, xMax_Reco);
      RecotmpSplit->Sumw2();
      h_RecoSysSplit[sys] = RecotmpSplit;
      GetOutputList()->Add(RecotmpSplit);

      TH1F* GentmpSplit = new TH1F(strippedOption + genvar + "_" + sys + "_Split", genvar, nBins_Gen, BinEdgesGen.data());
      GentmpSplit->Sumw2();
      h_GenSysSplit[sys] = GentmpSplit;
      GetOutputList()->Add(GentmpSplit);
    }
  }

  A_equBins = new TH2D(strippedOption + "A_equBins" + currentJESJERvar, "A", nBins_Reco, xMin_Reco, xMax_Reco, nBins_Reco, xMin_Reco, xMax_Reco);
  A_equBins->Sumw2();
  GetOutputList()->Add(A_equBins);

  h_testMET = new TH1F(strippedOption + "TestMET" + currentJESJERvar, "TESTMET", nBins_Reco, xMin_Reco, xMax_Reco);
  h_testMET->Sumw2();
  GetOutputList()->Add(h_testMET);

  h_testMETgenBinning = new TH1F(strippedOption + "testMETgenBinning" + currentJESJERvar, "testMETgenBinning", nBins_Gen, BinEdgesGen.data());
  h_testMETgenBinning->Sumw2();
  GetOutputList()->Add(h_testMETgenBinning);

  h_testMET_Split = new TH1F(strippedOption + "TestMET" + currentJESJERvar + "_Split", "TESTMET", nBins_Reco, xMin_Reco, xMax_Reco);
  h_testMET_Split->Sumw2();
  GetOutputList()->Add(h_testMET_Split);

  h_fake = new TH1F(strippedOption + "fakes" + currentJESJERvar, recovar, nBins_Reco, xMin_Reco, xMax_Reco);
  h_fake->Sumw2();
  GetOutputList()->Add(h_fake);

  h_fake_Split = new TH1F(strippedOption + "fakes" + currentJESJERvar + "_Split", recovar, nBins_Reco, xMin_Reco, xMax_Reco);
  h_fake_Split->Sumw2();
  GetOutputList()->Add(h_fake_Split);

  h_misses = new TH1F(strippedOption + "misses" + currentJESJERvar, genvar, nBins_Gen, BinEdgesGen.data());
  h_misses->Sumw2();
  GetOutputList()->Add(h_misses);

  h_misses_Split = new TH1F(strippedOption + "misses" + currentJESJERvar + "_Split", genvar,  nBins_Gen, BinEdgesGen.data());
  h_misses_Split->Sumw2();
  GetOutputList()->Add(h_misses_Split);

  // Additional Variables
  h_N_Jets = new TH1F(strippedOption + "N_Jets" + currentJESJERvar, "N_Jets", 15, 0, 15);
  h_N_Jets->Sumw2();
  GetOutputList()->Add(h_N_Jets);
  h_Jet_Pt = new TH1F(strippedOption + "Jet_Pt" + currentJESJERvar, "Jets_Pt", 80, 0, 800);
  h_Jet_Pt->Sumw2();
  GetOutputList()->Add(h_Jet_Pt);
  h_Jet_Eta = new TH1F(strippedOption + "Jet_Eta" + currentJESJERvar, "Jet_Eta", 40, -4, 4);
  h_Jet_Eta->Sumw2();
  GetOutputList()->Add(h_Jet_Eta);
  h_Evt_Phi_MET = new TH1F(strippedOption + "Evt_Phi_MET" + currentJESJERvar, "Evt_Phi_MET", 50, -3.2, 3.2);
  h_Evt_Phi_MET->Sumw2();
  GetOutputList()->Add(h_Evt_Phi_MET);
  h_Evt_Phi_GenMET = new TH1F(strippedOption + "Evt_Phi_GenMET" + currentJESJERvar, "Evt_Phi_GenMET", 50, -3.2, 3.2);
  h_Evt_Phi_GenMET->Sumw2();
  GetOutputList()->Add(h_Evt_Phi_GenMET);

  h_W_Pt = new TH1F(strippedOption + "h_W_Pt" + currentJESJERvar, "h_W_Pt", 120, 0, 1200);
  h_W_Pt->Sumw2();
  GetOutputList()->Add(h_W_Pt);

  h_Z_Pt = new TH1F(strippedOption + "h_Z_Pt" + currentJESJERvar, "h_Z_Pt", 120, 0, 1200);
  h_Z_Pt->Sumw2();
  GetOutputList()->Add(h_Z_Pt);

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

  //////////////////////
  // Add weights here!!//
  //////////////////////
  bool isWlnuSample = option.Contains("w_lnu_jets");
  bool isZnunuSample = option.Contains("z_nunu_jets");
  WbosonWeight_nominal   = hWbosonWeight_nominal->GetBinContent(hWbosonWeight_nominal->FindBin(*W_Pt));
  ZbosonWeight_nominal   = hZbosonWeight_nominal->GetBinContent(hZbosonWeight_nominal->FindBin(*Z_Pt));

  WbosonWeight_QCD1Up    = hWbosonWeight_QCD1Up->GetBinContent(hWbosonWeight_QCD1Up->FindBin(*W_Pt));
  WbosonWeight_QCD1Down  = hWbosonWeight_QCD1Down->GetBinContent(hWbosonWeight_QCD1Down->FindBin(*W_Pt));
  WbosonWeight_QCD2Up    = hWbosonWeight_QCD2Up->GetBinContent(hWbosonWeight_QCD2Up->FindBin(*W_Pt));
  WbosonWeight_QCD2Down  = hWbosonWeight_QCD2Down->GetBinContent(hWbosonWeight_QCD2Down->FindBin(*W_Pt));
  WbosonWeight_QCD3Up    = hWbosonWeight_QCD3Up->GetBinContent(hWbosonWeight_QCD3Up->FindBin(*W_Pt));
  WbosonWeight_QCD3Down  = hWbosonWeight_QCD3Down->GetBinContent(hWbosonWeight_QCD3Down->FindBin(*W_Pt));
  WbosonWeight_EW1Up     = hWbosonWeight_EW1Up->GetBinContent(hWbosonWeight_EW1Up->FindBin(*W_Pt));
  WbosonWeight_EW1Down   = hWbosonWeight_EW1Down->GetBinContent(hWbosonWeight_EW1Down->FindBin(*W_Pt));
  WbosonWeight_EW2Up     = hWbosonWeight_EW2Up->GetBinContent(hWbosonWeight_EW2Up->FindBin(*W_Pt));
  WbosonWeight_EW2Down   = hWbosonWeight_EW2Down->GetBinContent(hWbosonWeight_EW2Down->FindBin(*W_Pt));
  WbosonWeight_EW3Up     = hWbosonWeight_EW3Up->GetBinContent(hWbosonWeight_EW3Up->FindBin(*W_Pt));
  WbosonWeight_EW3Down   = hWbosonWeight_EW3Down->GetBinContent(hWbosonWeight_EW3Down->FindBin(*W_Pt));
  WbosonWeight_MixedUp   = hWbosonWeight_MixedUp->GetBinContent(hWbosonWeight_MixedUp->FindBin(*W_Pt));
  WbosonWeight_MixedDown = hWbosonWeight_MixedDown->GetBinContent(hWbosonWeight_MixedDown->FindBin(*W_Pt));


  ZbosonWeight_QCD1Up    = hZbosonWeight_QCD1Up->GetBinContent(hZbosonWeight_QCD1Up->FindBin(*Z_Pt));
  ZbosonWeight_QCD1Down  = hZbosonWeight_QCD1Down->GetBinContent(hZbosonWeight_QCD1Down->FindBin(*Z_Pt));
  ZbosonWeight_QCD2Up    = hZbosonWeight_QCD2Up->GetBinContent(hZbosonWeight_QCD2Up->FindBin(*Z_Pt));
  ZbosonWeight_QCD2Down  = hZbosonWeight_QCD2Down->GetBinContent(hZbosonWeight_QCD2Down->FindBin(*Z_Pt));
  ZbosonWeight_QCD3Up    = hZbosonWeight_QCD3Up->GetBinContent(hZbosonWeight_QCD3Up->FindBin(*Z_Pt));
  ZbosonWeight_QCD3Down  = hZbosonWeight_QCD3Down->GetBinContent(hZbosonWeight_QCD3Down->FindBin(*Z_Pt));
  ZbosonWeight_EW1Up     = hZbosonWeight_EW1Up->GetBinContent(hZbosonWeight_EW1Up->FindBin(*Z_Pt));
  ZbosonWeight_EW1Down   = hZbosonWeight_EW1Down->GetBinContent(hZbosonWeight_EW1Down->FindBin(*Z_Pt));
  ZbosonWeight_EW2Up     = hZbosonWeight_EW2Up->GetBinContent(hZbosonWeight_EW2Up->FindBin(*Z_Pt));
  ZbosonWeight_EW2Down   = hZbosonWeight_EW2Down->GetBinContent(hZbosonWeight_EW2Down->FindBin(*Z_Pt));
  ZbosonWeight_EW3Up     = hZbosonWeight_EW3Up->GetBinContent(hZbosonWeight_EW3Up->FindBin(*Z_Pt));
  ZbosonWeight_EW3Down   = hZbosonWeight_EW3Down->GetBinContent(hZbosonWeight_EW3Down->FindBin(*Z_Pt));
  ZbosonWeight_MixedUp   = hZbosonWeight_MixedUp->GetBinContent(hZbosonWeight_MixedUp->FindBin(*Z_Pt));
  ZbosonWeight_MixedDown = hZbosonWeight_MixedDown->GetBinContent(hZbosonWeight_MixedDown->FindBin(*Z_Pt));

  BosonSystematicWeights = {
    {"WbosonWeight_QCD1Up", WbosonWeight_QCD1Up},
    {"WbosonWeight_QCD1Down", WbosonWeight_QCD1Down},
    {"WbosonWeight_QCD2Up", WbosonWeight_QCD2Up},
    {"WbosonWeight_QCD2Down", WbosonWeight_QCD2Down},
    {"WbosonWeight_QCD3Up", WbosonWeight_QCD3Up},
    {"WbosonWeight_QCD3Down", WbosonWeight_QCD3Down},
    {"WbosonWeight_EW1Up", WbosonWeight_EW1Up},
    {"WbosonWeight_EW1Down", WbosonWeight_EW1Down},
    {"WbosonWeight_EW2Up", WbosonWeight_EW2Up},
    {"WbosonWeight_EW2Down", WbosonWeight_EW2Down},
    {"WbosonWeight_EW3Up", WbosonWeight_EW3Up},
    {"WbosonWeight_EW3Down", WbosonWeight_EW3Down},
    {"WbosonWeight_MixedUp", WbosonWeight_MixedUp},
    {"WbosonWeight_MixedDown", WbosonWeight_MixedDown},

    {"ZbosonWeight_QCD1Up", ZbosonWeight_QCD1Up},
    {"ZbosonWeight_QCD1Down", ZbosonWeight_QCD1Down},
    {"ZbosonWeight_QCD2Up", ZbosonWeight_QCD2Up},
    {"ZbosonWeight_QCD2Down", ZbosonWeight_QCD2Down},
    {"ZbosonWeight_QCD3Up", ZbosonWeight_QCD3Up},
    {"ZbosonWeight_QCD3Down", ZbosonWeight_QCD3Down},
    {"ZbosonWeight_EW1Up", ZbosonWeight_EW1Up},
    {"ZbosonWeight_EW1Down", ZbosonWeight_EW1Down},
    {"ZbosonWeight_EW2Up", ZbosonWeight_EW2Up},
    {"ZbosonWeight_EW2Down", ZbosonWeight_EW2Down},
    {"ZbosonWeight_EW3Up", ZbosonWeight_EW3Up},
    {"ZbosonWeight_EW3Down", ZbosonWeight_EW3Down},
    {"ZbosonWeight_MixedUp", ZbosonWeight_MixedUp},
    {"ZbosonWeight_MixedDown", ZbosonWeight_MixedDown},
  };

  weight_ = (*Weight_XS) * (*Weight_CSV) * (*Weight_PU) * (*Weight_GEN_nom);
  // if (*Weight_GenValue > 0)
  //   weight_ *= 1;
  // else
  //   weight_ *= -1;
  // weight_ = *Weight;
  double random = rand.Rndm();


  if (!option.Contains("data")) weight_ *= 35.91823;
  if (isZnunuSample) {
    weight_ *= 3 * 0.971;
    if (*Z_Pt > 30) {
      weight_ *= (ZbosonWeight_nominal);
      // std::cout << "applying additional Z Boson Weight " <<  ZbosonWeight_nominal << std::endl;
    }
  }
  if (isWlnuSample) {
    if (*W_Pt > 30) {
      weight_ *= (WbosonWeight_nominal);
      // std::cout << "applying additional W Boson Weight " <<  WbosonWeight_nominal << std::endl;
    }
  }
  // Calculate split
  // std::cout << "WARNING split > 50, therefore not working correctly ->
  // Proceeding with split =50" << std::endl;
  float split_ = split / 100.;
  // std::cout << split_ << std::endl;
  bool isPseudoData = random < split_;
  bool triggered = *Triggered_HLT_PFMET170_X || *Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X || *Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X || *Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X || *Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X;
  if (*Miss) {
    // if (!(*recoSelected)) {
    // if (*GenMonoJetSelection && *GenLeptonVetoSelection && *GenBTagVetoSelection && *GenPhotonVetoSelection && *GenMETSelection && *GenmonoVselection) {

    A->Fill(-10, *var_gen, weight_);
    h_misses->Fill(*var_gen, weight_);

    h_Gen->Fill(*var_gen, weight_); // Fill GenMET also, if its a miss, since they belong to the desired Gen PhaseSpace
    if (doadditionalsystematics) { //fill systematics only in nominal case
      for (auto& sys : allSystematics ) {
        // std::cout << "checking systematic " << sys << std::endl;
        if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
          float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
          ASys.find(sys)->second->Fill(-10, *var_gen, tmpweight );
          h_GenSys.find(sys)->second->Fill( *var_gen, tmpweight );
        }
        else if (!BosonSystematicWeights.count(sys)) {
          float tmpweight = weight_ * *(sysweights.find(sys)->second);
          if (sys == "PUup" || sys == "PUdown") {
            tmpweight /= *(sysweights.find(sys)->second);
          }
          ASys.find(sys)->second->Fill(-10, *var_gen, tmpweight );
          h_GenSys.find(sys)->second->Fill( *var_gen, tmpweight );
        }
      }
    }

    if (isPseudoData) {
      ASplit->Fill(-10, *var_gen, weight_);
      h_misses_Split->Fill(*var_gen, weight_);
      h_GenSplit->Fill(*var_gen, weight_); // Fill GenMET also, if its a miss, since they belong to the desired Gen PhaseSpace

      if (doadditionalsystematics) { //fill systematics only in nominal case
        for (auto& sys : allSystematics ) {
          if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
            float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
            ASysSplit.find(sys)->second->Fill(-10, *var_gen, tmpweight );
            h_GenSysSplit.find(sys)->second->Fill(*var_gen, tmpweight );
          }
          else if (!BosonSystematicWeights.count(sys)) {
            float tmpweight = weight_ * *(sysweights.find(sys)->second);
            if (sys == "PUup" || sys == "PUdown") {
              tmpweight /= *(sysweights.find(sys)->second);
            }
            ASysSplit.find(sys)->second->Fill(-10, *var_gen, tmpweight );
            h_GenSysSplit.find(sys)->second->Fill( *var_gen, tmpweight );
          }
        }
      }
    }
    // }
  }
  if (triggered) {
    if (*recoSelected) {
      //don't touch genselection for "normal" reco histograms
      if (isPseudoData) {
        h_RecoSplit->Fill(*var_reco, weight_);
        h_DummyDataSplit->Fill(*var_reco, weight_);

        if (doadditionalsystematics) {  //fill systematics only in nominal case
          for (auto& sys : allSystematics ) {
            if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
              float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
              h_RecoSysSplit.find(sys)->second->Fill(*var_reco, tmpweight );
            }
            else if (!BosonSystematicWeights.count(sys)) {
              float tmpweight = weight_ * *(sysweights.find(sys)->second);
              if (sys == "PUup" || sys == "PUdown") {
                tmpweight /= *(sysweights.find(sys)->second);
              }
              h_RecoSysSplit.find(sys)->second->Fill(*var_reco, tmpweight );

            }
          }
        }
      }
      h_Reco->Fill(*var_reco, weight_);
      if (doadditionalsystematics) {  //fill systematics only in nominal case
        for (auto& sys : allSystematics ) {
          if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
            float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
            h_RecoSys.find(sys)->second->Fill(*var_reco, tmpweight );
          }
          else if (!BosonSystematicWeights.count(sys)) {
            float tmpweight = weight_ * *(sysweights.find(sys)->second);
            if (sys == "PUup" || sys == "PUdown") {
              tmpweight /= *(sysweights.find(sys)->second);
            }
            h_RecoSys.find(sys)->second->Fill(*var_reco, tmpweight );
          }
        }
      }

      // Additional Variables
      h_N_Jets->Fill(*N_Jets, weight_);
      h_Jet_Pt->Fill(*Jet_Pt, weight_);
      h_Jet_Eta->Fill(*Jet_Eta, weight_);
      h_Evt_Phi_MET->Fill(*Evt_Phi_MET, weight_);
      h_Evt_Phi_GenMET->Fill(*Evt_Phi_GenMET, weight_);

      h_W_Pt->Fill(*W_Pt, weight_);
      h_Z_Pt->Fill(*Z_Pt, weight_);


      if (*Fake) { //is a fake
        // if ( !*GenMonoJetSelection || !*GenLeptonVetoSelection || !*GenBTagVetoSelection || !*GenPhotonVetoSelection || !*GenMETSelection || !*GenmonoVselection) { //is a fake; GenPhotonVeto bugged?!
        if (!isPseudoData) {
          h_fake_Split->Fill(*var_reco, weight_);
        }
        h_fake->Fill(*var_reco, weight_);
        if (!*GenMonoJetSelection) failedGenMonoJetSelection += 1 * weight_;
        if (!*GenLeptonVetoSelection) failedGenLeptonVetoSelection += 1 * weight_;
        if (!*GenBTagVetoSelection) failedGenBTagVetoSelection += 1 * weight_;
        if (!*GenPhotonVetoSelection) failedGenPhotonVetoSelection += 1 * weight_;
        if (!*GenMETSelection) failedGenMETSelection += 1 * weight_;
        if (!*GenmonoVselection) failedGenmonoVselection += 1 * weight_;
      }
      // else if (*genSelected) { //is not a fake
      else { //is not a fake
        if (!isPseudoData) {
          h_GenSplit->Fill(*var_gen, weight_);
          h_testMET_Split->Fill(*var_reco, weight_);
          h_testMETgenBinning->Fill(*var_reco, weight_);
          ASplit->Fill(*var_reco, *var_gen, weight_);

          if (doadditionalsystematics) { //fill systematics only in nominal case
            for (auto& sys : allSystematics ) {
              if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
                float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
                ASysSplit.find(sys)->second->Fill(*var_reco, *var_gen, tmpweight );
                h_GenSysSplit.find(sys)->second->Fill(*var_gen, tmpweight );
              }
              else if (!BosonSystematicWeights.count(sys)) {
                float tmpweight = weight_ * *(sysweights.find(sys)->second);
                if (sys == "PUup" || sys == "PUdown") {
                  tmpweight /= *(sysweights.find(sys)->second);
                }
                ASysSplit.find(sys)->second->Fill(*var_reco, *var_gen, tmpweight );
                h_GenSysSplit.find(sys)->second->Fill(*var_gen, tmpweight );
              }
            }
          }
        }
        h_Gen->Fill(*var_gen, weight_);
        h_testMET->Fill(*var_reco, weight_);
        A_equBins->Fill(*var_reco, *var_gen, weight_);
        A->Fill(*var_reco, *var_gen, weight_);

        if (doadditionalsystematics) { //fill systematics only in nominal case
          for (auto& sys : allSystematics ) {
            if (BosonSystematicWeights.count(sys) && (isZnunuSample || isWlnuSample)) {
              float tmpweight = weight_ * BosonSystematicWeights.find(sys)->second;
              ASys.find(sys)->second->Fill(*var_reco, *var_gen, tmpweight );
              h_GenSys.find(sys)->second->Fill( *var_gen, tmpweight );
            }
            else if (!BosonSystematicWeights.count(sys)) {
              float tmpweight = weight_ * *(sysweights.find(sys)->second);
              if (sys == "PUup" || sys == "PUdown") {
                tmpweight /= *(sysweights.find(sys)->second);
              }
              ASys.find(sys)->second->Fill(*var_reco, *var_gen, tmpweight );
              h_GenSys.find(sys)->second->Fill( *var_gen, tmpweight );
            }
          }
        }
      }
    }
  }
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