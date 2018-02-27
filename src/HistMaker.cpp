#include "../interface/HistMaker.hpp"

#include <iostream>
#include <TEnv.h>
#include <TProofLog.h>
#include "TCanvas.h"
#include "TH2D.h"

#include <dirent.h>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
#include "MCSelector.h"
#include "TProof.h"
#include <curses.h>
#include <stdio.h>


using namespace std;

void HistMaker::MakeHistos() {
	ParseConfig();
	cout << "Getting Signal Files:" << endl;
	std::vector<TString> SignalFilelist = GetInputFileList(SignalPath, variation);
	TChain* SignalChain = ChainFiles(SignalFilelist);
	cout << "Getting Data Files:" << endl;
	std::vector<TString> DataFilelist = GetInputFileList(DataPath, variation);
	TChain* DataChain = ChainFiles(DataFilelist);
	std::vector<TString> tmp;

	cout << "Getting BKG Files:" << endl;
	for (const std::string& name : bkgnames) {
		BkgFilelists[name];
		tmp = GetInputFileList(BkgPaths[name], variation);
		for (const TString& file : tmp) {
			BkgFilelists[name].push_back(file);
		}
	}
	TChain* tmp_chain;
	for (const std::string& name : bkgnames) {
		tmp_chain = ChainFiles(BkgFilelists[name]);
		BkgChains.insert( std::make_pair( name, tmp_chain ));
	}
	return FillHistos(SignalChain, DataChain, BkgChains);
}

template<typename T>
std::vector<T> to_array(const std::string& s)
{
	std::vector<T> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, ',')) result.push_back(boost::lexical_cast<T>(item));
	return result;
}

void HistMaker::ParseConfig() {
	cout << "Parsing Hist Config..." << endl;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(string(path.GetConfigPath()), pt);
	samplepath = std::string(pt.get<std::string>("SamplePath.path"));
	SignalPath = to_array<std::string>(pt.get<std::string>("SignalSample.path"));
	DataPath = to_array<std::string>(pt.get<std::string>("DataSample.path"));
	bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));

	for (const std::string& name : bkgnames) {
		BkgPaths[name];
		std::vector<std::string> tmp = to_array<std::string>(pt.get<std::string>(name + ".path"));
		for (const std::string&  paths : tmp) {
			BkgPaths[name].push_back(paths);
		}
	}
	genvar = pt.get<string>("vars.gen");
	recovar = pt.get<string>("vars.reco");
	variation = pt.get<string>("general.variation");	
	useBatch = pt.get<bool>("general.useBatch");

	cout << "Config parsed!" << endl;
}

std::vector<TString> HistMaker::GetInputFileList(std::vector<std::string> paths , TString type)
{
	std::vector<TString> filelist;
	for (const TString& path : paths) {

		DIR* dirFile = opendir( samplepath + path );
		cout << "opening " << samplepath + path << endl;
		if ( dirFile )
		{
			struct dirent* hFile;
			errno = 0;
			while (( hFile = readdir( dirFile )) != NULL )
			{
				if ( !strcmp( hFile->d_name, "."  )) continue;
				if ( !strcmp( hFile->d_name, ".." )) continue;

				// in linux hidden files all start with '.'
				// if ( gIgnoreHidden && ( hFile->d_name[0] == '.' )) continue;

				// dirFile.name is the name of the file. Do whatever string comparison
				// you want here. Something like:
				if (type == "nominal") {
					if ( strstr( hFile->d_name, "nominal_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESup") {
					if ( strstr( hFile->d_name, "JESup_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESdown") {
					if ( strstr( hFile->d_name, "JESdown_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERup") {
					if ( strstr( hFile->d_name, "JERup_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERdown") {
					if ( strstr( hFile->d_name, "JERdown_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
			}
		}
		closedir( dirFile );

	}

	return filelist;
}





TChain* HistMaker::ChainFiles(std::vector<TString> filelist) {
	cout << "Setting up TChain" << endl;
	TChain* chain = new TChain("MVATree");
	for (const TString& fileName : filelist) {
		if ( fileName.EndsWith(".root") ) {
			chain->Add(fileName);
		}
		else {
			ifstream filelist;
			filelist.open(fileName);
			int nOpenedFiles = 0;
			if ( filelist.is_open() ) {
				TString name = "";
				while ( !filelist.eof() ) {
					filelist >> name;
					if ( filelist.eof() ) break;
					chain->Add(name);
					nOpenedFiles++;
				}
			} else {
				std::cerr << "ERROR opening file '" << fileName << "'\n";
				exit(1);
			}
			filelist.close();
		}
	}
	cout << "TChain SetUp!" << endl;
	return chain;
}

void HistMaker::FillHistos(TChain * SignalChain, TChain * DataChain, std::map<std::string, TChain*> BkgChains) {
	//Start Timer to measure Time in Selector
	cout << "Start Timer for Filling Histo Procedure..." << endl;
	TStopwatch watch;
	watch.Start();
	//SetUp TProof
	TProof *pl = TProof::Open("workers=10");
	if (useBatch) {
		pl->Close();
		TString connect = gSystem->GetFromPipe("pod-info -c");
		TProof *pl = TProof::Open(connect);
	}
	//Load necessary Macros
	pl->Load(path.GetIncludePath() + "PathHelper.hpp+");
	pl->Load(path.GetSourcePath() + "PathHelper.cpp+");
	pl->Load(path.GetSourcePath() + "MCSelector.C+");

	MCSelector *sel = new MCSelector(); // This is my custom selector class
	//Set Custom InputParameter (not used for now)
	pl->SetParameter("outputpath", (TString)path.GetOutputFilePath());
	TH1F* h_Gen = histhelper.Get1DHisto(genvar);
	pl->AddInput(h_Gen);
	//Process Chains
	std::remove(path.GetHistoFilePath()); // delete file
	DataChain->SetProof();
	DataChain->Process(sel, "data");
	pl->ClearCache();

	SignalChain->SetProof();
	SignalChain->Process(sel, "signal");
	pl->ClearCache();


	for (const std::string& name : bkgnames) {
		TChain* chain_tmp = BkgChains.find(name)->second;
		chain_tmp->SetProof();
		chain_tmp->Process(sel, TString(name));
		pl->ClearCache();
	}

	//Log SlaveSessions
	TProofLog *p = TProof::Mgr("lite://")->GetSessionLogs();
	p->Save("*", "filewithlogs.txt");
	pl->ClearCache();
	pl->Close();

	//Stop Timer
	cout << "Time for Filling Histo Procedure:" << endl;
	watch.Stop();
	watch.Print();
}



