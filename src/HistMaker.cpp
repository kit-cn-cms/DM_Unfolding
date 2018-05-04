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
	std::vector<TChain*> SignalChains;
	std::vector<TChain*> DataChains;
	std::vector<std::vector<TChain*>> BkgChainsVariations;
	std::vector<TChain*> BkgChains;

	for (auto& var : variation) {
		cout << "chaining files for " << var << " variation" << endl;

		cout << "Getting Signal Files:" << endl;
		std::vector<TString> SignalFilelist = GetInputFileList(SignalPath, var);
		SignalChains.push_back(ChainFiles(SignalFilelist));

		cout << "Getting Data Files:" << endl;
		std::vector<TString> DataFilelist = GetInputFileList(DataPath, var);
		DataChains.push_back(ChainFiles(DataFilelist));

		for (auto& name : bkgnames) {
			cout << "Getting BKG files for " << name << endl;
			std::vector<TString> filelist = GetInputFileList(BkgPaths[name], var);
			cout << filelist.at(0) << endl;;
			TChain* tmp_chain = ChainFiles(filelist);

			if (name == "diboson" || name == "singletop") {
				long nevents = tmp_chain->GetEntries();
				TTree* tmpFriendTree = CreateFriendTree(LHAFixBranchesLong, LHAFixBranchesFloat, nevents, name + TString("Friend"));
				TChain* tmpFriendChain = new TChain( name + TString("Friend"));
				tmpFriendChain->Add(path.GetRootFilesPath() + name + TString("Friend.root"));
				tmp_chain -> AddFriend(tmpFriendChain);
			}

			BkgChains.push_back(tmp_chain);
		}
		BkgChainsVariations.push_back(BkgChains);
		BkgChains.clear();
	}

	long data_events = DataChains.at(0)->GetEntries();
	TTree* FriendTree = CreateFriendTree(additionalBranchNamesLong, additionalBranchNamesFloat, data_events, TString("TreeFriend"));
	TChain* FriendChain = new TChain("TreeFriend");
	FriendChain->Add(path.GetRootFilesPath() + "TreeFriend.root");
	for (auto& chain : DataChains) chain->AddFriend(FriendChain);
	for (auto& chain : SignalChains) chain->AddFriend(FriendChain);

	int nVariation = 0;
	for (auto& varchains : BkgChainsVariations) {
		int j = 0;
		for (auto& chain : varchains) {
			if (variation.at(nVariation) != "nominal") {
				chain->AddFriend(FriendChain);
			}
		}
		nVariation += 1;
	}
	//Reset Histofile
	std::remove(path.GetHistoFilePath());
	FillHistos(SignalChains, DataChains, BkgChainsVariations);
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
	additionalBranchNamesLong = to_array<std::string>(pt.get<std::string>("tree.additionalBranchNamesLong"));
	additionalBranchNamesFloat = to_array<std::string>(pt.get<std::string>("tree.additionalBranchNamesFloat"));

	LHAFixBranchesLong = to_array<std::string>(pt.get<std::string>("tree.LHAFixBranchesLong"));
	LHAFixBranchesFloat = to_array<std::string>(pt.get<std::string>("tree.LHAFixBranchesFloat"));

	for (const std::string& name : bkgnames) {
		BkgPaths[name];
		std::vector<std::string> tmp = to_array<std::string>(pt.get<std::string>(name + ".path"));
		for (const std::string&  paths : tmp) {
			BkgPaths[name].push_back(paths);
		}
	}
	genvar = pt.get<string>("vars.gen");
	recovar = pt.get<string>("vars.reco");
	variation = to_array<std::string>(pt.get<std::string>("general.variation"));
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
						// printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESup") {
					if ( strstr( hFile->d_name, "JESup_Tree.root" )) {
						// printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESdown") {
					if ( strstr( hFile->d_name, "JESdown_Tree.root" )) {
						// printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERup") {
					if ( strstr( hFile->d_name, "JERup_Tree.root" )) {
						// printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = samplepath + path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERdown") {
					if ( strstr( hFile->d_name, "JERdown_Tree.root" )) {
						// printf( "found an .root file: %s \n", hFile->d_name );
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
	// cout << "Setting up TChain" << endl;
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
	// cout << "TChain SetUp!" << endl;
	return chain;
}

TTree* HistMaker::CreateFriendTree(std::vector<string> BranchNamesLong, std::vector<string> BranchNamesFloat, long n_Events, TString label) {
	TFile* friendfile = new TFile(path.GetRootFilesPath() + label + ".root", "RECREATE");
	TTree* TreeFriend = new TTree(label, label);
	TreeFriend->SetEntries(n_Events);
	Long64_t longval = 1;
	Float_t floatval = 1;
	for (auto const& name : BranchNamesLong) {
		TBranch *branch = TreeFriend->Branch(name.c_str(), &longval, (name + "/L").c_str());
	}
	for (auto const& name : BranchNamesFloat) {
		TBranch *branch = TreeFriend->Branch(name.c_str(), &floatval, (name + "/F").c_str());
	}
	for (int j = 0; j < n_Events; j++) {
		longval = 1;
		floatval = 1;
		TreeFriend->Fill();
	}

	TreeFriend->Write();
	friendfile->Close();
	return TreeFriend;
}

void HistMaker::FillHistos(std::vector<TChain*> SignalChains, std::vector<TChain*> DataChains, std::vector<std::vector<TChain*>> BkgChainsVariations) {
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
	Bool_t notOnClient = kFALSE;
	Bool_t uniqueWorkers = kTRUE;
	pl->Load(path.GetIncludePath() + "PathHelper.hpp+", notOnClient, uniqueWorkers);
	pl->Load(path.GetSourcePath() + "PathHelper.cpp+", notOnClient, uniqueWorkers);
	pl->Load(path.GetSourcePath() + "MCSelector.C+", notOnClient, uniqueWorkers);

	MCSelector *sel = new MCSelector(); // This is my custom selector class
	//Set Custom InputParameter (not used for now)
	pl->SetParameter("outputpath", (TString)path.GetOutputFilePath());
	TH1F* h_Gen = histhelper.Get1DHisto(genvar);
	pl->AddInput(h_Gen);
	//Process Chains
	//Data
	int nVariation = 0;
	for (auto& chain : DataChains) {
		chain->SetProof();
		chain->Process(sel, "data_" + TString(variation.at(nVariation)));
		pl->ClearCache();
		nVariation += 1;
		delete chain;
	}
	//Signal
	nVariation = 0;
	for (auto& chain : SignalChains) {
		chain->SetProof();
		chain->Process(sel, "signal_" + TString(variation.at(nVariation)));
		pl->ClearCache();
		nVariation += 1;
		delete chain;
	}
	//MC Backgrounds
	nVariation = 0;
	for (auto& varchains : BkgChainsVariations) {
		int j = 0;
		for (auto& chain : varchains) {
			chain->SetProof();
			chain->Process(sel, TString(bkgnames.at(j)) + "_" + variation.at(nVariation));
			pl->ClearCache();
			j += 1;
			delete chain;
		}
		nVariation += 1;
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



