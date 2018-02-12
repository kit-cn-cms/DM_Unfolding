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

	string path = pt.get<std::string>("MCSample.path");
	std::cout << path << std::endl;

	HistMaker histomaker;
	histomaker.MakeHistos();

//Testing stuff
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");

	TH1F* gen = histomaker.GetHisto(genvar);
	TH1F* reco = histomaker.GetHisto(recovar);
	TH1F* A = histomaker.GetHisto("A");
	TH1F* data = histomaker.GetHisto("Data");

	gen->Print();
	reco->Print();
	A->Print();
	data->Print();



	return (0);
}