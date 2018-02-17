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


#include "MCSelector.hpp"
#include <TH2.h>
#include <TH1.h>

#include <TStyle.h>
#include <TCanvas.h>

// #include "../interface/HistMaker.hpp"



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
   file = new TFile("file.root","RECREATE");
   // GetOutputList()->Add(file);

}

void MCSelector::SlaveBegin(TTree * /*tree*/)
{

   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   //file = new TFile("file.root","RECREATE");
   h_GenMET = new TH1F("h_GenMET","h_GenMET",20,0.,1000.);
   h_GenMET->Sumw2();
   GetOutputList()->Add(h_GenMET);

   h_RecoMET = new TH1F("h_RecoMET","h_RecoMET",20,0.,1000.);
   h_RecoMET->Sumw2();
   GetOutputList()->Add(h_RecoMET);   

   GetOutputList()->Print();

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
   fReader.SetLocalEntry(entry);

   h_GenMET->Fill(*Evt_Pt_GenMET);
   h_RecoMET->Fill(*Evt_Pt_MET);



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
   

   GetOutputList()->Print();

   // fOutput->FindObject(h_GenMET)->Print();
   // file->Close();

   h_GenMET = dynamic_cast<TH1F*>(fOutput->FindObject("h_GenMET"));
   h_RecoMET = dynamic_cast<TH1F*>(fOutput->FindObject("h_RecoMET"));
   file->WriteTObject(h_RecoMET);

   if (h_GenMET) {
      // Play with the tree 
      h_GenMET->Print();
      file->WriteTObject(h_GenMET);
      std::cout << "found histo"<< std::endl;
      
   } else {
      Error("Terminate", "h_GenMET object missing");
   }

   std::cout << "Master finished" << std::endl;


}