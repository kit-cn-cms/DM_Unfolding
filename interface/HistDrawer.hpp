#ifndef HistDrawer_HPP_
#define HistDrawer_HPP_


#include "TH1F.h"

#include "../interface/PathHelper.hpp"

class HistDrawer
{
public:
	void Draw1D(TH1F* hist);

	PathHelper path;


};

#endif