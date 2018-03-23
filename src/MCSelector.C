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
#include <TH2.h>
#include <TH1.h>

#include <TStyle.h>
#include <TCanvas.h>
#include "TParameter.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
#include "TRandom3.h"

// #include "../interface/HistMaker.hpp"
// #include "PathHelper.hpp"
// #include "../interface/HistMaker.hpp"

template<typename T>
std::vector<T> to_array(const std::string& s)
{
   std::vector<T> result;
   std::stringstream ss(s);
   std::string item;
   while (std::getline(ss, item, ',')) result.push_back(boost::lexical_cast<T>(item));
   return result;
}

// #ifdef MCSelector_cxx
void MCSelector::Init(TTree *tree)
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


Bool_t MCSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


// #endif // #ifdef MCSelector_cxx



void MCSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
   TString option = GetOption();

   // TParameter<TString> *p =
   // dynamic_cast<TParameter<TString>*>(fInput->FindObject("outputpath"));
   // outpath = (p) ? (TString) p->GetVal() : outpath;
   histofile = new TFile(path.GetHistoFilePath(), "UPDATE");
}


void MCSelector::SlaveBegin(TTree * /*tree*/)
{

   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   std::cout << "Processing: " << option << std::endl;
   std::cout << "Setting up Histos..." << std::endl;
   std::cout << "Parsing Hist Config..." << std::endl;
   ConfigPath = path.GetConfigPathforSlaves();
   boost::property_tree::ptree pt;
   boost::property_tree::ini_parser::read_ini(std::string(ConfigPath), pt);

   bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));
   BinEdgesGen = to_array<double>(pt.get<std::string>("Binning.BinEdgesGen"));
   BinEdgesReco = to_array<double>(pt.get<std::string>("Binning.BinEdgesReco"));
   nBins_Reco = BinEdgesReco.size() - 1;
   nBins_Gen = BinEdgesGen.size() - 1;


   genvar = pt.get<std::string>("vars.gen");
   recovar = pt.get<std::string>("vars.reco");
   variation = pt.get<std::string>("general.variation");
   // nBins_Gen = pt.get<int>("Binning.nBins_Gen");
   // nBins_Reco = pt.get<int>("Binning.nBins_Reco");
   xMin_Gen = pt.get<int>("Binning.xMin_Gen");
   xMax_Gen = pt.get<int>("Binning.xMax_Gen");
   xMin_Reco = pt.get<int>("Binning.xMin_Reco");
   xMax_Reco = pt.get<int>("Binning.xMax_Reco");
   split = pt.get<int>("general.split");

   std::cout << "Config parsed!" << std::endl;

   TRandom* rand = new TRandom();
   // book histos for split distributions
   h_GenSplit = new TH1F(genvar + "_" + option + "_Split", genvar, nBins_Gen, BinEdgesGen.data());
   h_GenSplit->Sumw2();
   GetOutputList()->Add(h_GenSplit);
   h_RecoSplit = new TH1F(recovar + "_" + option + "_Split", recovar, nBins_Reco, BinEdgesReco.data());
   h_RecoSplit->Sumw2();
   GetOutputList()->Add(h_RecoSplit);
   h_DummyDataSplit = new TH1F("DummyData_" + option + "_Split", "DummyData", nBins_Reco, BinEdgesReco.data());
   h_DummyDataSplit->Sumw2();
   GetOutputList()->Add(h_DummyDataSplit);
   ASplit = new TH2D("A_" + option + "_Split", "A", nBins_Reco, BinEdgesReco.data(), nBins_Gen, BinEdgesGen.data());
   ASplit->Sumw2();
   GetOutputList()->Add(ASplit);


   // book histos for full distributions
   h_Gen = new TH1F(genvar + "_" + option, genvar, nBins_Gen, BinEdgesGen.data());
   h_Gen->Sumw2();
   GetOutputList()->Add(h_Gen);
   h_Reco = new TH1F(recovar + "_" + option, recovar, nBins_Reco, BinEdgesReco.data());
   h_Reco->Sumw2();
   GetOutputList()->Add(h_Reco);
   A = new TH2D("A_" + option, "A", nBins_Reco, BinEdgesReco.data(), nBins_Gen, BinEdgesGen.data());
   A->Sumw2();
   GetOutputList()->Add(A);

   h_testMET = new TH1F("TestMET" + option, "TESTMET",  nBins_Reco, BinEdgesReco.data());
   h_testMET->Sumw2();
   GetOutputList()->Add(h_testMET);

   h_testMET_Split = new TH1F("TestMET" + option + "_Split", "TESTMET",  nBins_Reco, BinEdgesReco.data());
   h_testMET_Split->Sumw2();
   GetOutputList()->Add(h_testMET_Split);

   h_fake = new TH1F("fakes_" + option, recovar, nBins_Reco, BinEdgesReco.data());
   h_fake->Sumw2();
   GetOutputList()->Add(h_fake);

   h_fake_Split = new TH1F("fakes_" + option + "_Split", recovar, nBins_Reco, BinEdgesReco.data());
   h_fake_Split->Sumw2();
   GetOutputList()->Add(h_fake_Split);

   //Additional Variables
   h_N_Jets = new TH1F("N_Jets_" + option, "N_Jets", 15, 0, 15);
   h_N_Jets->Sumw2();
   GetOutputList()->Add(h_N_Jets);
   h_Jet_Pt = new TH1F("Jet_Pt_" + option, "Jets_Pt", 80, 0, 800);
   h_Jet_Pt->Sumw2();
   GetOutputList()->Add(h_Jet_Pt);
   h_Jet_Eta = new TH1F("Jet_Eta_" + option, "Jet_Eta", 40, -4, 4);
   h_Jet_Eta->Sumw2();
   GetOutputList()->Add(h_Jet_Eta);
   h_Evt_Phi_MET = new TH1F("Evt_Phi_MET_" + option, "Evt_Phi_MET", 50, -3.2, 3.2);
   h_Evt_Phi_MET->Sumw2();
   GetOutputList()->Add(h_Evt_Phi_MET);
   h_Evt_Phi_GenMET = new TH1F("Evt_Phi_GenMET_" + option, "Evt_Phi_GenMET", 50, -3.2, 3.2);
   h_Evt_Phi_GenMET->Sumw2();
   GetOutputList()->Add(h_Evt_Phi_GenMET);


   std::cout << "All Histos SetUp!" << std::endl;

}




Bool_t MCSelector::Process(Long64_t entry)
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
   //Add weights here!!//
   //////////////////////
   weight_ =  (*Weight_XS)*(*Weight_CSV)*(*Weight_PU);
   if (*Weight_GenValue > 0)weight_ *= 1;
   else weight_ *= -1;
   // weight_ = *Weight;
   if (!option.Contains("data")) weight_ *= 35.9 ;
   if (option.Contains("Zjet")) weight_ *= 3*0.971;

   //Calculate split
   // std::cout << "WARNING split > 50, therefore not working correctly -> Proceeding with split =50" << std::endl;
   float split_ = split / 100.;
   // std::cout << split_ << std::endl;
   double random = rand.Rndm();


   if ( (!*GenLeptonVetoSelection  || !*GenBTagVetoSelection || !*GenMonoJetSelection || !*GenLeptonVetoSelection ) && *var_gen > 250) {
      if (random >= split_ ) {
         h_fake_Split->Fill(*var_reco, weight_);
      }
      h_fake->Fill(*var_reco, weight_);
   }
   else {
      if (random >= split_) {
         h_GenSplit->Fill(*var_gen, weight_);
         h_testMET_Split->Fill(*var_reco, weight_);
         ASplit->Fill(*var_reco, *var_gen, weight_);
      }
      h_Gen->Fill(*var_gen, weight_);
      h_testMET->Fill(*var_reco, weight_);
      A->Fill(*var_reco, *var_gen, weight_);
   }

   if (random >= split_) {
      h_RecoSplit->Fill(*var_reco, weight_);
      h_DummyDataSplit->Fill(*var_reco, weight_);
   }
   h_Reco->Fill(*var_reco, weight_);



   //Additional Variables
   h_N_Jets->Fill(*N_Jets, weight_);
   h_Jet_Pt->Fill(*Jet_Pt, weight_);
   h_Jet_Eta->Fill(*Jet_Eta, weight_);
   h_Evt_Phi_MET->Fill(*Evt_Phi_MET, weight_);
   h_Evt_Phi_GenMET->Fill(*Evt_Phi_GenMET, weight_);

   weight_ = 0;
   return kTRUE;
}

void MCSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
   std::cout << " Slave finished" << std::endl;

}

void MCSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   std::cout << "Input List:" << std::endl;
   GetInputList()->ls();
   std::cout << "Output List:" << std::endl;
   GetOutputList()->ls();
   TString option = GetOption();

   //Full Sample
   A = dynamic_cast<TH2D*>(fOutput->FindObject(A));
   histofile->WriteTObject(A);
   h_Gen = dynamic_cast<TH1F*>(fOutput->FindObject(h_Gen));
   histofile->WriteTObject(h_Gen);
   h_Reco = dynamic_cast<TH1F*>(fOutput->FindObject(h_Reco));
   histofile->WriteTObject(h_Reco);
   //Split Sample
   h_DummyDataSplit = dynamic_cast<TH1F*>(fOutput->FindObject(h_DummyDataSplit));
   histofile->WriteTObject(h_DummyDataSplit);
   ASplit = dynamic_cast<TH2D*>(fOutput->FindObject(ASplit));
   histofile->WriteTObject(ASplit);
   h_GenSplit = dynamic_cast<TH1F*>(fOutput->FindObject(h_GenSplit));
   histofile->WriteTObject(h_GenSplit);
   h_RecoSplit = dynamic_cast<TH1F*>(fOutput->FindObject(h_RecoSplit));
   histofile->WriteTObject(h_RecoSplit);
   h_testMET = dynamic_cast<TH1F*>(fOutput->FindObject(h_testMET));
   histofile->WriteTObject(h_testMET);
   h_testMET_Split = dynamic_cast<TH1F*>(fOutput->FindObject(h_testMET_Split));
   histofile->WriteTObject(h_testMET_Split);
   h_fake = dynamic_cast<TH1F*>(fOutput->FindObject(h_fake));
   histofile->WriteTObject(h_fake);
   h_fake_Split = dynamic_cast<TH1F*>(fOutput->FindObject(h_fake_Split));
   histofile->WriteTObject(h_fake_Split);

   //Additional Variables
   h_N_Jets = dynamic_cast<TH1F*>(fOutput->FindObject(h_N_Jets));
   histofile->WriteTObject(h_N_Jets);
   h_Jet_Pt = dynamic_cast<TH1F*>(fOutput->FindObject(h_Jet_Pt));
   histofile->WriteTObject(h_Jet_Pt);
   h_Jet_Eta = dynamic_cast<TH1F*>(fOutput->FindObject(h_Jet_Eta));
   histofile->WriteTObject(h_Jet_Eta);
   h_Evt_Phi_MET = dynamic_cast<TH1F*>(fOutput->FindObject(h_Evt_Phi_MET));
   histofile->WriteTObject(h_Evt_Phi_MET);
   h_Evt_Phi_GenMET = dynamic_cast<TH1F*>(fOutput->FindObject(h_Evt_Phi_GenMET));
   histofile->WriteTObject(h_Evt_Phi_GenMET);

   histofile->Close();
   std::cout << "Master finished" << std::endl;

}