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
	
	string path = pt.get<std::string>("Sample1.path");
	std::cout << path << std::endl;

	HistMaker histomaker;
	histomaker.MakeHistos();

	// histomaker.ParseConfig();
	// histomaker.SetUpHistos();
	// histomaker.ChainFiles();


	return (0);
}