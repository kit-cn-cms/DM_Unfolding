#include "../interface/PathHelper.hpp"

#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TH2D.h"
// #include "TChain.h"
// #include "TString.h"
// #include <dirent.h>
// #include <unistd.h>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/ini_parser.hpp>


using namespace std;

TString PathHelper::GetOutputFilePath()
{
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/rootfiles/output.root";
	return filepath;
}

TString PathHelper::GetHistoFilePath()
{
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." +"/rootfiles/histos.root";
	return filepath;
}

TString PathHelper::GetConfigPath(TString name) {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/Config/" + name + ".ini";
	return filepath;
}

TString PathHelper::GetPdfPath() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/pdfs/" ;
	return filepath;
}


