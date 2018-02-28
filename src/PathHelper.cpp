#include "PathHelper.hpp"

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
#include <boost/program_options.hpp>


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

	TString filepath = workingdir + "/.." + "/rootfiles/histos.root";
	return filepath;
}

TString PathHelper::GetConfigPath( ) {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);
	////////////////////////////////////////////
	/////EDIT THIS FOR DIFFERENT CONFIGS!!!/////
	////////////////////////////////////////////
	string Config = "DMConfig";
	////////////////////////////////////////////
	TString filepath = workingdir + "/../" + "/Config/" + Config + ".ini";
	return filepath;
}
TString PathHelper::GetConfigPathforSlaves() {
	////////////////////////////////////////////
	/////EDIT THIS FOR DIFFERENT CONFIGS!!!/////
	////////////////////////////////////////////
	return "/nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/Config/DMConfig.ini";
}


TString PathHelper::GetPdfPath() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/pdfs/" ;
	return filepath;
}

TString PathHelper::GetSourcePath() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/src/";
	return filepath;
}

TString PathHelper::GetIncludePath() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/interface/";
	return filepath;
}

TString PathHelper::GetRootFilesPath() {
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/.." + "/rootfiles/";
	return filepath;
}



