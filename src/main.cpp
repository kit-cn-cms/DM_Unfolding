#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


#include "../interface/HistMaker.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"


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
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);


//Testing stuff
	TString genvar = pt.get<string>("vars.gen");
	TString recovar = pt.get<string>("vars.reco");

	TH1F* gen = histomaker.Get1DHisto(genvar);
	TH1F* reco = histomaker.Get1DHisto(recovar);

	gen->Print();
	reco->Print();
	A->Print();
	data->Print();

	HistDrawer Drawer;
	Drawer.Draw1D(reco, recovar);
	Drawer.Draw1D(gen, genvar);
	Drawer.Draw1D(std::get<0>(unfold_output), recovar+"unfolded");
	Drawer.Draw1D(std::get<1>(unfold_output), recovar+"foldedback");
	Drawer.Draw2D(A, "A");
	Drawer.DrawRatio(std::get<0>(unfold_output),gen,"unfolded_Gen","unfolded/Gen");
	Drawer.DrawRatio(std::get<1>(unfold_output),data,"foldedback_data","foldedback/data");





	return (0);
}