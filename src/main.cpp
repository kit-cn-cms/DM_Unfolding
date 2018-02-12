#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


#include "../interface/HistMaker.hpp"
#include "../interface/Unfolder.hpp"

using namespace std;


int main()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("Config/DMConfig.ini", pt);

	string path = pt.get<std::string>("MCSample.path");
	std::cout << path << std::endl;

	bool fillhistos = pt.get<bool>("general.fillhistos");
	HistMaker histomaker;

	if (fillhistos) {
		histomaker.MakeHistos();
	}
	TH2F* A = histomaker.Get2DHisto("A");
	TH1F* data = histomaker.Get1DHisto("Data");


	Unfolder Unfolder;
	TUnfoldDensity* unfold = Unfolder.Unfold(A, data);
	unfold->Print();
	Unfolder.GetOutput(unfold);


//Testing stuff
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");

	TH1F* gen = histomaker.Get1DHisto(genvar);
	TH1F* reco = histomaker.Get1DHisto(recovar);

	gen->Print();
	reco->Print();
	A->Print();
	data->Print();



	return (0);
}