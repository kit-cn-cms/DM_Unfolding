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

   genvar = pt.get<std::string>("vars.gen");
   recovar = pt.get<std::string>("vars.reco");
   variation = pt.get<std::string>("general.variation");
   nBins_Gen = pt.get<int>("Binning.nBins_Gen");
   nBins_Reco = pt.get<int>("Binning.nBins_Reco");
   xMin = pt.get<int>("Binning.xMin");
   xMax = pt.get<int>("Binning.xMax");
   nMax = pt.get<int>("general.maxEvents");
   splitSignal = pt.get<bool>("general.splitSignal");
   split = pt.get<int>("general.split");
   std::cout << "Config parsed!" << std::endl;

   // book histos
   h_Gen = new TH1F(genvar, genvar, nBins_Gen, xMin, xMax);
   h_Gen->Sumw2();
   GetOutputList()->Add(h_Gen);
   h_Reco = new TH1F(recovar, recovar, nBins_Gen, xMin, xMax);
   h_Reco->Sumw2();
   GetOutputList()->Add(h_Reco);
   h_Data = new TH1F("Data", "Data", nBins_Reco, xMin, xMax);
   h_Data->Sumw2();
   GetOutputList()->Add(h_Data);
   h_DummyData = new TH1F("DummyData", "DummyData", nBins_Reco, xMin, xMax);
   h_DummyData->Sumw2();
   GetOutputList()->Add(h_DummyData);
   A = new TH2D("A", "A", nBins_Reco, xMin, xMax, nBins_Gen, xMin, xMax);
   A->Sumw2();
   GetOutputList()->Add(A);

   bkgname = option;
   // std::vector<TH1F*> h_bkg_vec;
   h_bkg = new TH1F(bkgname, bkgname, nBins_Reco, xMin, xMax);
   h_bkg->Sumw2();
   GetOutputList()->Add(h_bkg);


   std::cout << "All Histos SetUp!" << std::endl;

   //Working Stuff
   h_GenMET = new TH1F("h_GenMET", "h_GenMET", 20, 0., 1000.);
   GetOutputList()->Add(h_GenMET);



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
   weight_ =  (*Weight_XS) * (*Weight_GenValue) * (*Weight_PU);

   //couts for debugging
   // std::cout << weight_ << std::endl;
   // std::cout << *var_reco << std::endl;
   // std::cout << *var_gen << std::endl;


   if (option == "signal") {

      //Loop over all SignalEvents and Fill SignalHistograms

      //Calculate split
      if (split > 50) {
         std::cout << "WARNING split > 50, therefore not working correctly -> Proceeding with split =50" << std::endl;
         split = 50;
      }
      int split_ = 100 / split;
      //Fill Events
      // std::cout << "Filling Signal Events..." << std::endl;

      if (splitSignal) {         // split MC Sample for studies
         std::cout << *var_reco << std::endl;
         if (entry % split_ != 0) {
            A->Fill(*var_reco, *var_gen, weight_);
         }
         else  {
            h_DummyData->Fill(*var_reco, weight_);
            h_Gen->Fill(*var_gen, weight_);
            h_Reco->Fill(*var_reco, weight_);
         }
      }
      else {               // use full MC Sample
         h_Gen->Fill(*var_gen, weight_);
         h_Reco->Fill(*var_reco, weight_);
         A->Fill(*var_reco, *var_gen, weight_);
         h_DummyData->Fill(*var_reco, weight_);
      }
   }

   if (option == "data") {

      //Loop over all DataEvents to Fill DataHisto

      // std::cout << "Filling Data Events..." << std::endl;
      //Fill Events
      h_Data->Fill(*var_reco, weight_);
   }


   // if (std::find(bkgnames.begin(), bkgnames.end(), option) != bkgnames.end()) {
   else {
      // std::cout << "Filling BKG Events..." << std::endl;
      //Fill Events
      h_bkg->Fill(*var_reco, weight_);
   }

   //Working Stuff
   h_GenMET->Fill(*Evt_Pt_GenMET);

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
   if (option == "signal")
   {
      h_Gen = dynamic_cast<TH1F*>(fOutput->FindObject(h_Gen));
      histofile->WriteTObject(h_Gen);
      h_Reco = dynamic_cast<TH1F*>(fOutput->FindObject(h_Reco));
      histofile->WriteTObject(h_Reco);
      A = dynamic_cast<TH2D*>(fOutput->FindObject(A));
      histofile->WriteTObject(A);
      h_DummyData = dynamic_cast<TH1F*>(fOutput->FindObject(h_DummyData));
      histofile->WriteTObject(h_DummyData);
   }
   if (option == "data") {
      h_Data = dynamic_cast<TH1F*>(fOutput->FindObject(h_Data));
      histofile->WriteTObject(h_Data);
   }
   // if (std::find(bkgnames.begin(), bkgnames.end(), option) != bkgnames.end()) {
   // if(option=="Zjet"){
   else {
      std::cout << "WRiting Bkg" << std::endl;
      h_bkg = dynamic_cast<TH1F*>(fOutput->FindObject(h_bkg));
      h_bkg->Print();
      if (h_bkg) std::cout << "found h_bkg" << std::endl;
      else  Error("Terminate", "h_GenMET object missing");
      histofile->WriteTObject(h_bkg);
   }

   // for (const TString& name : bkgnames) {
   //    h_tmp = dynamic_cast<TH1F*>(fOutput->FindObject(name));
   //    GetOutputList()->Add(h_tmp);
   // }

   histofile->Close();
   std::cout << "Master finished" << std::endl;


}