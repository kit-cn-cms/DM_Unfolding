#include "../interface/HistMaker.hpp"

using namespace std;


std::vector<TString> HistMaker::GetInputFileList(TString path , TString type)
{
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
	h_Gen->Print();
	// h_pt_dummyData = TH1F("h_pt_dummyData", "h_pt_dummyData", 50, 0, 1000)

	TH2D* A = new TH2D("A", "A", nBins_Reco, xMin, xMax, nBins_Gen, xMin, xMax);

	h_Gen->Sumw2();
	h_Reco->Sumw2();
	A->Sumw2();

	h_Gen->Write();
	h_Reco->Write();
	A->Write();

	histos->Close();

	cout << "All Histos SetUp!" << endl;
}

void HistMaker::ParseConfig() {
	cout << "Parsing Config..." << endl;
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);

	genvar = pt.get<string>("vars.gen");
	// std::cout << genvar << std::endl;

	recovar = pt.get<string>("vars.reco");
	// std::cout << recovar << std::endl;

	path = pt.get<string>("Sample1.path");
	// std::cout << path << std::endl;

	variation = pt.get<string>("Sample1.variation");
	// std::cout << path << std::endl;

	nBins_Gen = pt.get<int>("Binning.nBins_Gen");
	// std::cout << nBins_Gen << std::endl;

	nBins_Reco = pt.get<int>("Binning.nBins_Reco");
	// std::cout << nBins_Reco << std::endl;

	xMin = pt.get<int>("Binning.xMin");
	// std::cout << xMin << std::endl;

	xMax = pt.get<int>("Binning.xMax");
	// std::cout << xMax << std::endl;

	nMax = pt.get<int>("general.maxEvents");

	cout << "Config parsed!" << endl;

}

TChain* HistMaker::ChainFiles() {
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

void HistMaker::FillHistos(TChain* chain) {
	float var_gen;
	chain->SetBranchAddress(genvar, &var_gen);
	float var_reco;
	chain->SetBranchAddress(recovar, &var_reco);


	TH1F* h_Gen = GetHisto(genvar);
	TH1F* h_Reco = GetHisto(recovar);
	TH1F* A = GetHisto("A");

	//Loop over all Events and Fill Histogram
	double nentries = chain->GetEntries();
	cout << "total number of events: " << nentries << endl;
	for (long iEntry = 0; iEntry < nentries; iEntry++) {
		if (iEntry % 10000 == 0) cout << "analyzing event " << iEntry << endl;
		if (iEntry > nMax && nMax > 0) break;
		chain->GetEntry(iEntry);

		h_Gen->Fill(var_gen);
		h_Reco->Fill(var_reco);
		A->Fill(var_reco, var_gen);
	}

	cout << "All Histos filled!" << endl;
	//Write Filles Histos to File
	TFile *histos = new TFile(GetHistoFilePath(), "recreate");
	histos->Map();

	//Write Filles Histos to File

	h_Reco->Write();
	h_Gen->Write();
	A->Write();
	histos->Close();

}

void HistMaker::MakeHistos() {
	ParseConfig();
	cout << path << endl;
	GetInputFileList(path, variation);
	SetUpHistos();
	TChain* chain = ChainFiles();
	return FillHistos(chain);
}



TH1F* HistMaker::GetHisto(TString name) {
	TFile *file = new TFile(GetHistoFilePath(), "update");
	TH1F* hist = (TH1F*)file->Get(name);
	// file ->Close();
	return hist;
}