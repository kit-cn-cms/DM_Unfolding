#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
// #include "TUnfold.h"
#include "TUnfoldDensity.h"
#include "TH2D.h"
#include "TSpline.h"
#include "TGraph.h"
#include "math.h"
#include "TStyle.h"
// #include "gstyle.h"
// #include <sys/stat.h>
#include <dirent.h>


using namespace std;




class HistMaker
{
public:
	void GetInputFileList(const char* path, std::string type);
	void SetUpHistos();
	TFile histos;



};

