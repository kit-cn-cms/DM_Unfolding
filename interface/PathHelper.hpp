#ifndef PathHelper_HPP_
#define PathHelper_HPP_


#include "TString.h"

class PathHelper
{
public:
	TString GetOutputFilePath();
	TString GetHistoFilePath();
	TString GetROOTdirPath();
	TString GetConfigPath();
	TString GetConfigPathforSlaves();

	TString GetPdfPath();
	TString GetSourcePath() ;
	TString GetIncludePath() ;
	TString GetRootFilesPath();
	TString GetRootFilePathforSlaves();





};


#endif
