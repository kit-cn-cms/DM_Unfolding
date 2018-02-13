#include "../interface/HistMaker.hpp"

using namespace std;


std::vector<TString> HistMaker::GetInputFileList(TString path , TString type)
{
	std::vector<TString> filelist;
	DIR* dirFile = opendir( path );
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
	return filelist;
}

TString GetHistoFilePath()
{
	char currentdir[1024];
	getcwd(currentdir, sizeof(currentdir));
	string workingdir(currentdir);

	TString filepath = workingdir + "/rootfiles/histos.root";
	return filepath;
}


void HistMaker::SetUpHistos() {
	cout << "Setting up Histos..." << endl;
	//create File to Save Histos

	TFile *histos = new TFile(GetHistoFilePath(), "recreate");

	// book histos
	TH1F* h_Reco = new TH1F(recovar, recovar, nBins_Reco, xMin, xMax);
	TH1F* h_Gen = new TH1F(genvar, genvar, nBins_Gen, xMin, xMax);
	TH1F* h_Data = new TH1F("Data", "Data", nBins_Reco, xMin, xMax);

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

void HistMaker::ParseConfig() {
	cout << "Parsing Hist Config..." << endl;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);

	MCPath = pt.get<string>("MCSample.path");
	DataPath = pt.get<string>("DataSample.path");

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

void HistMaker::FillHistos(TChain* MCChain, TChain* DataChain) {



	TH1F* h_Gen = Get1DHisto(genvar);
	TH1F* h_Reco = Get1DHisto(recovar);
	TH1F* h_Data = Get1DHisto("Data");
	TH2F* A = Get2DHisto("A");

	//Loop over all MCEvents and Fill MCHistograms
	float var_gen;
	MCChain->SetBranchAddress(genvar, &var_gen);
	float var_reco;
	MCChain->SetBranchAddress(recovar, &var_reco);

	cout << "Filling MC Events..." << endl;
	double nentries = MCChain->GetEntries();
	cout << "total number of MC events: " << nentries << endl;

	if (split > 50) {
		cout << "WARNING split > 50, therefore not working correctly -> Proceeding with split =50" << endl;
		split = 50;
	}
	int split_ = 100 / split;

	for (long iEntry = 0; iEntry < nentries; iEntry++) {
		if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
		if (iEntry > nMax && nMax > 0) break;
		MCChain->GetEntry(iEntry);

		if (!useData) {			// split MC Sample for studies
			if (iEntry % split_ != 0) {
				A->Fill(var_reco, var_gen);
			}
			else  {
				h_Data->Fill(var_reco);
				h_Gen->Fill(var_gen);
				h_Reco->Fill(var_reco);
			}
		}
		else {					// use full MC Sample
			h_Gen->Fill(var_gen);
			h_Reco->Fill(var_reco);
			A->Fill(var_reco, var_gen);
		}
	}

	//Loop over all DataEvents to Fill DataHisto
	if (useData) {
		float var;
		DataChain->SetBranchAddress(recovar, &var);
		cout << "Filling Data Events..." << endl;
		double nentries = DataChain->GetEntries();
		cout << "total number of Data events: " << nentries << endl;
		for (long iEntry = 0; iEntry < nentries; iEntry++) {
			if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
			if (iEntry > nMax && nMax > 0) break;
			DataChain->GetEntry(iEntry);

			h_Data->Fill(var);
		}
	}

	cout << "All Histos filled!" << endl;
	//Write Filles Histos to File
	TFile *histos = new TFile(GetHistoFilePath(), "recreate");

	//Write Filles Histos to File
	h_Reco->Write();
	h_Gen->Write();
	A->Write();
	h_Data->Write();
	histos->Close();

}

void HistMaker::MakeHistos() {
	ParseConfig();
	std::vector<TString> MCFilelist = GetInputFileList(MCPath, variation);
	TChain* MCChain = ChainFiles(MCFilelist);
	std::vector<TString> DataFilelist = GetInputFileList(DataPath, variation);
	TChain* DataChain = ChainFiles(DataFilelist);
	SetUpHistos();
	return FillHistos(MCChain, DataChain);
}



TH1F* HistMaker::Get1DHisto(TString name) {
	TFile *file = new TFile(GetHistoFilePath(), "update");
	TH1F* hist = (TH1F*)file->Get(name);
	// file ->Close();
	return hist;
}

TH2F* HistMaker::Get2DHisto(TString name) {
	TFile *file = new TFile(GetHistoFilePath(), "update");
	TH2F* hist = (TH2F*)file->Get(name);
	// file ->Close();
	return hist;
}