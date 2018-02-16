#include "../interface/HistMaker.hpp"

#include <iostream>
#include "TCanvas.h"
#include "TH2D.h"

#include <dirent.h>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "boost/lexical_cast.hpp"
// #include "../interface/MCSelector.hpp"


using namespace std;


std::vector<TString> HistMaker::GetInputFileList(std::vector<std::string> paths , TString type)
{
	std::vector<TString> filelist;
	for (const TString& path : paths) {

		DIR* dirFile = opendir( path );
		cout << "opening" << path << endl;
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
						TString fullpath = path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESup") {
					if ( strstr( hFile->d_name, "JESup_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JESdown") {
					if ( strstr( hFile->d_name, "JESdown_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERup") {
					if ( strstr( hFile->d_name, "JERup_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = path + fileName;
						filelist.push_back( fullpath );
					}
				}
				else if (type == "JERdown") {
					if ( strstr( hFile->d_name, "JERdown_Tree.root" )) {
						printf( "found an .root file: %s \n", hFile->d_name );
						TString path_ = path;
						TString fileName = hFile->d_name;
						TString fullpath = path + fileName;
						filelist.push_back( fullpath );
					}
				}
			}
		}
		closedir( dirFile );

	}

	return filelist;
}



void HistMaker::SetUpHistos() {
	cout << "Setting up Histos..." << endl;
	//create File to Save Histos

	TFile *histos = new TFile(path.GetHistoFilePath(), "recreate");

	// book histos
	TH1F* h_Reco = new TH1F(recovar, recovar, nBins_Reco, xMin, xMax);
	TH1F* h_Gen = new TH1F(genvar, genvar, nBins_Gen, xMin, xMax);
	TH1F* h_Data = new TH1F("Data", "Data", nBins_Reco, xMin, xMax);

	std::vector<TH1F*> h_bkg_vec;
	for (const TString& name : bkgnames) {
		TH1F* h_tmp = new TH1F(name, name, nBins_Reco, xMin, xMax);
		h_tmp->Sumw2();
		h_tmp->Write();
		delete h_tmp;
	}

	TH2D* A = new TH2D("A", "A", nBins_Reco, xMin, xMax, nBins_Gen, xMin, xMax);

	h_Gen->Sumw2();
	h_Reco->Sumw2();
	A->Sumw2();


	h_Gen->Write();
	h_Reco->Write();
	h_Data->Write();
	A->Write();

	histos->Close();

	cout << "All Histos SetUp!" << endl;
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
	boost::property_tree::ini_parser::read_ini(string(path.GetConfigPath("DMConfig")), pt);

	weights = to_array<std::string>(pt.get<std::string>("general.weights"));
	MCPath = to_array<std::string>(pt.get<std::string>("MCSample.path"));
	DataPath = to_array<std::string>(pt.get<std::string>("DataSample.path"));
	bkgnames = to_array<std::string>(pt.get<std::string>("Bkg.names"));

	for (const std::string& name : bkgnames) {
		BkgPaths[name];
		std::vector<std::string> tmp = to_array<std::string>(pt.get<std::string>(name + ".path"));
		for (const std::string&  paths : tmp) {
			BkgPaths[name].push_back(paths);
		}
	}

	// cout << BkgPaths[bkgnames.at(1)][0] << endl;
	// cout << BkgPaths[bkgnames.at(1)][1] << endl;
	// cout << BkgPaths[bkgnames.at(1)][2] << endl;
	genvar = pt.get<string>("vars.gen");
	recovar = pt.get<string>("vars.reco");
	variation = pt.get<string>("general.variation");
	nBins_Gen = pt.get<int>("Binning.nBins_Gen");
	nBins_Reco = pt.get<int>("Binning.nBins_Reco");
	xMin = pt.get<int>("Binning.xMin");
	xMax = pt.get<int>("Binning.xMax");
	nMax = pt.get<int>("general.maxEvents");
	useData = pt.get<bool>("general.useData");
	split = pt.get<int>("general.split");
	cout << "Config parsed!" << endl;

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

void HistMaker::FillHistos(TChain * MCChain, TChain * DataChain, std::map<std::string, TChain*> BkgChains) {
	TH1F* h_Gen = Get1DHisto(genvar);
	TH1F* h_Reco = Get1DHisto(recovar);
	TH1F* h_Data = Get1DHisto("Data");
	TH2F* A = Get2DHisto("A");

	//Loop over all MCEvents and Fill MCHistograms
	float var_gen;
	MCChain->SetBranchAddress(genvar, &var_gen);
	float var_reco;
	MCChain->SetBranchAddress(recovar, &var_reco);
	std::vector<float> varweight (weights.size());
	for (std::vector<std::string>::iterator it = weights.begin(); it != weights.end(); ++it) {
		MCChain->SetBranchAddress(TString(*it), &varweight.at(it - weights.begin()));
	}

	cout << "Filling MC Events..." << endl;
	double nentries = MCChain->GetEntries();
	cout << "total number of MC events: " << nentries << endl;

	if (split > 50) {
		cout << "WARNING split > 50, therefore not working correctly -> Proceeding with split =50" << endl;
		split = 50;
	}
	int split_ = 100 / split;
	float weight_ = 1;
	for (long iEntry = 0; iEntry < nentries; iEntry++) {
		if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
		if (iEntry > nMax && nMax > 0) break;
		MCChain->GetEntry(iEntry);
		weight_ = 1;
		for (std::vector<float>::iterator it = varweight.begin(); it != varweight.end(); ++it) {
			weight_ *= *it;
		}
		if (!useData) {			// split MC Sample for studies
			if (iEntry % split_ != 0) {
				A->Fill(var_reco, var_gen, weight_);
			}
			else  {
				h_Data->Fill(var_reco, weight_);
				h_Gen->Fill(var_gen, weight_);
				h_Reco->Fill(var_reco, weight_);
			}
		}
		else {					// use full MC Sample
			h_Gen->Fill(var_gen, weight_);
			h_Reco->Fill(var_reco, weight_);
			A->Fill(var_reco, var_gen, weight_);
		}
	}


	//Loop over all DataEvents to Fill DataHisto
	if (useData) {
		for (std::vector<std::string>::iterator it = weights.begin(); it != weights.end(); ++it) {
			DataChain->SetBranchAddress(TString(*it), &varweight.at(it - weights.begin()));
		}
		float var;
		DataChain->SetBranchAddress(recovar, &var);
		cout << "Filling Data Events..." << endl;
		double nentries = DataChain->GetEntries();
		cout << "total number of Data events: " << nentries << endl;
		for (long iEntry = 0; iEntry < nentries; iEntry++) {
			if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
			if (iEntry > nMax && nMax > 0) break;
			DataChain->GetEntry(iEntry);
			weight_ = 1;
			for (std::vector<float>::iterator it = varweight.begin(); it != varweight.end(); ++it) {
				weight_ *= *it;
			}

			h_Data->Fill(var, weight_);
		}
	}

	//Loop over all BkgEvents
	std::vector<TH1F*> h_bkg_vec;
	for (const std::string& name : bkgnames) {
		cout << "Filling BkgHistos: " << name << endl;

		TH1F* h_tmp = Get1DHisto(name);
		TChain* chain_tmp = BkgChains.find(name)->second;
		float var;
		chain_tmp->SetBranchAddress(recovar, &var);

		for (std::vector<std::string>::iterator it = weights.begin(); it != weights.end(); ++it) {
			chain_tmp->SetBranchAddress(TString(*it), &varweight.at(it - weights.begin()));
		}
		nentries = chain_tmp->GetEntries();
		cout << "total number of " << name << " events: " << nentries << endl;

		for (long iEntry = 0; iEntry < nentries; iEntry++) {
			if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
			if (iEntry > nMax && nMax > 0) break;
			chain_tmp->GetEntry(iEntry);
			weight_ = 1;
			for (std::vector<float>::iterator it = varweight.begin(); it != varweight.end(); ++it) {
				weight_ *= *it;
			}
			h_tmp->Fill(var,weight_);
		}
		h_bkg_vec.push_back(h_tmp);
		delete chain_tmp;
	}
	TFile *histos = new TFile(path.GetHistoFilePath(), "recreate");

	cout << "All Histos filled!" << endl;
	// Write Filled Histos to File
	for (unsigned int i = 0; i < h_bkg_vec.size(); ++i)
	{
		h_bkg_vec.at(i)->Write();
	}
	h_Reco->Write();
	h_Gen->Write();
	A->Write();
	h_Data->Write();
	histos->Close();

}

void HistMaker::MakeHistos() {
	ParseConfig();
	// MCSelector MCSelector_;
	// MCSelector_.Print();
	std::vector<TString> MCFilelist = GetInputFileList(MCPath, variation);
	TChain* MCChain = ChainFiles(MCFilelist);
	std::vector<TString> DataFilelist = GetInputFileList(DataPath, variation);
	TChain* DataChain = ChainFiles(DataFilelist);
	std::vector<TString> tmp;

	// MCChain->Process("MCSelector.cpp");qq
	// MCSelector_.Process();

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

	SetUpHistos();
	return FillHistos(MCChain, DataChain, BkgChains);
}



TH1F* HistMaker::Get1DHisto(TString name) {
	TFile *file = new TFile(path.GetHistoFilePath(), "open");
	TH1F* hist = (TH1F*)file->Get(name);
	// file ->Close();
	return hist;
}

TH2F* HistMaker::Get2DHisto(TString name) {
	TFile *file = new TFile(path.GetHistoFilePath(), "update");
	TH2F* hist = (TH2F*)file->Get(name);
	// file ->Close();
	return hist;
}