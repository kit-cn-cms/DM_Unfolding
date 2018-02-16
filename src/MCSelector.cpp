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


#include "../interface/MCSelector.hpp"
#include <TH2.h>
#include <TStyle.h>
#include "../interface/HistMaker.hpp"




void MCSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void MCSelector::SlaveBegin(TTree * /*tree*/)
{

   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   HistMaker histmaker;
   histmaker.ParseConfig();

      TH1F* h_Gen = histmaker.Get1DHisto(genvar);
   TH1F* h_Reco = histmaker.Get1DHisto(recovar);
   TH1F* h_Data = histmaker.Get1DHisto("Data");
   TH2F* A = histmaker.Get2DHisto("A");




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

   fReader.SetEntry(entry);

   // float var_gen;
   // MCChain->SetBranchAddress(genvar, &var_gen);
   // float var_reco;
   // MCChain->SetBranchAddress(recovar, &var_reco);
   std::vector<float> varweight (weights.size());
   // for (std::vector<std::string>::iterator it = weights.begin(); it != weights.end(); ++it) {
   //    MCChain->SetBranchAddress(TString(*it), &varweight.at(it - weights.begin()));
   // }
   HistMaker histmaker;
   
   histmaker.ParseConfig();


   std::cout << "MC event: << entry" << std::endl;

   std::cout << genvar << std::endl;

   if(entry==10) Abort("End for now");

   return kTRUE;
}

void MCSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void MCSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}