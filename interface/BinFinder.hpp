#ifndef BinFinder_HPP_
#define BinFinder_HPP_

#include "TH2.h"
#include "../interface/HistDrawer.hpp"
#include "../interface/PathHelper.hpp"


class BinFinder
{
public:
	HistDrawer Drawer;
	PathHelper path;

	TH2* matrix;
	double nBins;
	double xMin;
	double xMax;
	int rounding;

	BinFinder(TH2* Matrix);
	TH1* GetSlice(int bin);
	std::vector<TH1*> GetSlices();
	std::vector<double> FitSlices(std::vector<TH1*> slices);
	std::vector<int> CalculateBinEdges(std::vector<double> sigmas);

	std::vector<int> GetBins();



};

#endif