#include "../interface/HistDrawer.hpp"

#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"
// #include "TChain.h"
// #include "TString.h"
// #include <dirent.h>
// #include <unistd.h>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/ini_parser.hpp>


void HistDrawer::Draw1D(TH1F* hist){
	TFile *output = new TFile(path.GetOutputFilePath(), "update");

	output->Close();


}