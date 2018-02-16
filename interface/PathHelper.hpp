#ifndef PathHelper_HPP_
#define PathHelper_HPP_


#include "TString.h"

class PathHelper
{
public:
	TString GetOutputFilePath();
	TString GetHistoFilePath();
	TString GetConfigPath(TString name);
	TString GetPdfPath();





};


#endif
