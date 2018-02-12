#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "TFile.h"


#include "../interface/HistMaker.hpp"

using namespace std;


int main()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);
<<<<<<< HEAD

=======
	
>>>>>>> de19a9a899ab5752a1c8c7351fe4f77f8e0dbae7
	string path = pt.get<std::string>("Sample1.path");
	std::cout << path << std::endl;

	HistMaker histomaker;
	histomaker.MakeHistos();

<<<<<<< HEAD
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");

	TH1F* gen=histomaker.GetHisto(genvar);
	TH1F* reco=histomaker.GetHisto(recovar);
	TH1F* A=histomaker.GetHisto("A");



	gen->Print();
	int n =gen->Integral();
	cout << n << endl;
	reco->Print();
	A->Print();
=======
	// histomaker.ParseConfig();
	// histomaker.SetUpHistos();
	// histomaker.ChainFiles();
>>>>>>> de19a9a899ab5752a1c8c7351fe4f77f8e0dbae7


	return (0);
}