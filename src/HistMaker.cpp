#include "../interface/HistMaker.hpp"

using namespace std;


void HistMaker::GetInputFileList(const char* path , std::string type)
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
				if ( strstr( hFile->d_name, "nominal_Tree.root" ))
					printf( "found an .root file: %s \n", hFile->d_name );
			}
			else if (type == "JESup") {
				if ( strstr( hFile->d_name, "JESup_Tree.root" ))
					printf( "found an .root file: %s \n", hFile->d_name );
			}
			else if (type == "JESdown") {
				if ( strstr( hFile->d_name, "JESdown_Tree.root" ))
					printf( "found an .root file: %s \n", hFile->d_name );
			}
			else if (type == "JERup") {
				if ( strstr( hFile->d_name, "JERup_Tree.root" ))
					printf( "found an .root file: %s \n", hFile->d_name );
			}
			else if (type == "JERdown") {
				if ( strstr( hFile->d_name, "JERdown_Tree.root" ))
					printf( "found an .root file: %s \n", hFile->d_name );
			}
		}
		closedir( dirFile );
	}
}

void HistMaker::SetUpHistos() {
	// # create File to Save Histos
// f_histos = TFile("histos.root", "RECREATE")
	// TFile* histos = new TFile("../rootfiles/histos.root", "RECREATE");
	TFile histos("../rootfiles/histos.root", "CREATE");
	// return histos;
// # book histos
// h_pt_MET = TH1F("h_pt_MET", "h_pt_MET", 50, 0, 1000)
// h_pt_GenMET = TH1F("h_pt_GenMET", "h_pt_GenMET", 25, 0, 1000)
// h_pt_dummyData = TH1F("h_pt_dummyData", "h_pt_dummyData", 50, 0, 1000)

// A_pt = TH2F("A_pt", "A_pt", 50, 0, 1000, 25, 0, 1000)
// h_pt_GenMET.Sumw2()
// h_pt_GenMET.Sumw2()
// A_pt.Sumw2()
}