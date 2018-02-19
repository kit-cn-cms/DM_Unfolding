//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Feb 10 11:06:02 2018 by ROOT version 6.06/01
// from TChain MVATree/
//////////////////////////////////////////////////////////

#ifndef PlotSelector_h
#define PlotSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include "PathHelper.hpp"
#include "TH2D.h"


// Headers needed by this particular selector


class MCSelector : public TSelector {
private:
   TH1F* h_GenMET = 0;
   TH1F* h_RecoMET = 0;

   TString outpath = "0";
   TString histofilepath = "0";

   PathHelper path;


public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   TString variation;
   int nBins_Gen;
   int nBins_Reco;
   int xMin;
   int xMax;
   int split;
   double nMax;
   bool useData;
   TString genvar;
   TString recovar;
   std::vector<std::string> MCPath;
   std::vector<std::string> DataPath;
   std::vector<std::string> bkgnames;
   std::map<std::string, std::vector<std::string>> BkgPaths;
   std::map<std::string, std::vector<TString>> BkgFilelists;
   std::map<std::string, TChain*> BkgChains;
   std::vector<std::string> weights;

   std::vector<TH1F*> h_bkg_vec;
   // book histos
   TH1F* h_Reco = 0;
   TH1F* h_Gen = 0;
   TH1F* h_Data = 0;
   TH1F* h_tmp = 0;
   TH2D* A = 0;




   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<Float_t> Evt_Pt_MET = {fReader, "Evt_Pt_MET"};
   TTreeReaderValue<Float_t> Evt_Pt_GenMET = {fReader, "Evt_Pt_GenMET"};
   TTreeReaderValue<Float_t> Weight_XS = {fReader, "Weight_XS"};
   TTreeReaderValue<Float_t> Weight_GenValue = {fReader, "Weight_GenValue"};
   TFile* histofile = 0;

   // PathHelper path;




   MCSelector(TTree * /*tree*/ = 0) { }
   virtual ~MCSelector() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();





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


//    fReader.SetTree(tree);
// }

// Bool_t MCSelector::Notify()
// {
//    // The Notify() function is called when a new file is opened. This
//    // can be either for a new TTree in a TChain or when when a new TTree
//    // is started when using PROOF. It is normally not necessary to make changes
//    // to the generated code, but the routine can be extended by the
//    // user if needed. The return value is currently not used.

//    return kTRUE;
// }


// #endif // #ifdef MCSelector_cxx