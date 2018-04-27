#ifndef FileWriter_HPP_
#define FileWriter_HPP_

#include <TString.h>
#include <TH1.h>
#include <PathHelper.hpp>
#include <TFile.h>


class FileWriter
{
public:
	FileWriter(TString filename);
	~FileWriter(){};
    void addToFile(TH1*);

	PathHelper path;
    TFile* file;
    TString thisFile;
	
};


#endif
