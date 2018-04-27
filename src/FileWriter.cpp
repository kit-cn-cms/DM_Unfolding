#include <../interface/FileWriter.hpp>


FileWriter::FileWriter(TString filename) {
	thisFile = path.GetROOTdirPath() + filename + ".root";
	file = new TFile(thisFile, "RECREATE");
	file->Close();
}

void FileWriter::addToFile(TH1* histo) {
	file = new TFile(thisFile, "UPDATE");
	histo->Write();
	file->Close();
}