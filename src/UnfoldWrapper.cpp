#include "../interface/UnfoldWrapper.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/FileWriter.hpp"
#include "../interface/PathHelper.hpp"

#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>


UnfoldWrapper::UnfoldWrapper(TString varName_, TString label_, std::vector<TH2*> A_, TH1F* data_, TH1* fakes_, std::vector<TH1*> misses_, std::vector<std::vector<TH1*>> MC_, std::vector<std::vector<TH1*>> GenMC_, std::vector<std::string> variations_, std::vector<std::string> bkgnames_, std::vector<double> BinEdgesGen_): writer(label_) {
	varName = varName_;
	label = label_;
	A = A_;
	data = data_;
	fakes = fakes_;
	misses = misses_;
	MC = MC_;
	GenMC = GenMC_;
	variations = variations_;
	bkgnames = bkgnames_;
	BinEdgesGen = BinEdgesGen;
}


void UnfoldWrapper::DoIt() {
// Data Input
	std::cout << "#########################################################################" << std::endl;
	std::cout << "##################Unfolding with label: " << label << "##################" << std::endl;
	std::cout << "#########################################################################" << std::endl;

	Unfolder Unfolder;
	HistDrawer Drawer;
	PathHelper path;

	bool log = true;
	bool drawpull = true;
	bool normalize = true;

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(std::string(path.GetConfigPath()), pt);
	TString genvar = pt.get<std::string>("vars.gen");
	TString recovar = pt.get<std::string>("vars.reco");
	bool AddMissesbyHand = pt.get<bool>("Unfolding.AddMissesbyHand");

	std::map<std::string, std::pair<TH1*, int>> nameGenSampleColorMap;
	std::map<std::string, std::pair<TH1*, int>> nameRecoSampleColorMap;
	std::vector<int> color = { kViolet + 7, kViolet + 3, kViolet - 1, kViolet - 2, kViolet, kViolet - 7, kGreen, kBlue};
	std::vector<std::string> names = {"#gamma +jets", "QCD", "Single Top", "t#bar{t}", "Diboson", "Z(ll)+jets", "W(l#nu)+jets", "Z(#nu#nu)+jets"};
	int i = 0;
	for (auto& name : names) {
		nameGenSampleColorMap[name] = std::make_pair(GenMC.at(0)[i], color.at(i));
		nameRecoSampleColorMap[name] = std::make_pair(MC.at(0)[i], color.at(i));
		i++;
	}

	// Subtract Fakes from Data
	TH1F* h_DataMinFakes = (TH1F*)data->Clone();
	h_DataMinFakes->Add(fakes, -1);

	//clear RecoUnderflow in case you want to treat misses manually
	if (AddMissesbyHand) {
		for (int genBin = 0; genBin <= A.at(0)->GetNbinsY(); genBin++) {
			A.at(0)->SetBinContent(0, genBin, 0);
		}

		for (auto& A_var : A) {
			for (auto& varname : variations) {
				if (TString(A_var->GetName()).Contains(varname)) {
					for (int genBin = 0; genBin <= A_var->GetNbinsY(); genBin++) {
						A_var->SetBinContent(0, genBin, 0);
					}
				}
			}
		}
	}

	int nBins_Gen = GenMC[0].at(0)->GetNbinsX();
	Unfolder.ParseConfig();
	TUnfoldDensity* unfold = Unfolder.SetUp(A.at(0), data);

	// TH1* bias = (TH1*)GenMC[0].at(0)->Clone();
	// bias->Reset();
	// for (auto& h : GenMC[0]) {
	// 	bias->Add(h);
	// }
	// unfold->SetBias(bias);

	TH2* ProbMatrix = (TH2*)A.at(0)->Clone();
	ProbMatrix->Reset();
	unfold->TUnfold::GetProbabilityMatrix(ProbMatrix, TUnfoldDensity::kHistMapOutputVert);
	// Drawer.Draw2D(ProbMatrix, "ProbMatrix" + label, false, varName + "_Reco", varName + "_Gen");

	unfold->SubtractBackground(fakes, "fakes" + label, 1, 0.0); // subtract fakes

// addsys variations of MigrationMatrix
	for (auto& A_var : A) {
		for (auto& varname : variations) {
			if (TString(A_var->GetName()).Contains(varname)) {
				std::cout << "adding " << varname << " as systematic" << std::endl;
				A_var->Print();
				std::cout << A_var->GetRMS() << std::endl;
				unfold->AddSysError(A_var, TString(varname), TUnfoldDensity::kHistMapOutputVert, TUnfoldDensity::kSysErrModeMatrix);
			}
		}
	}

	// Find Best Tau
	Unfolder.FindBestTauLcurve(unfold, label);
	// Unfolder.FindBestTau(unfold, label);
	// unfold->DoUnfold(0.0);
	// unfold->DoUnfold(0.1);




// Get Output
// 0st element=unfolded 1st=folded back
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);
	TH1* unfolded_nominal = std::get<0>(unfold_output);

// add misses
	if (AddMissesbyHand) {
		i = 0;
		for (auto& h : misses) {
			unfolded_nominal->Add(misses.at(i), +1);
			i++;
		}
	}


// STAT SOURCES


// SYST SOURCES
//input data stat error
	TH2* ErrorMatrix_input = unfold->GetEmatrixInput("ErrorMatrix_input" + label);
	Drawer.Draw2D(ErrorMatrix_input, "ErrorMatrix_input" + label, true, "Unfolded " + varName, "Unfolded " + varName);
// subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr = unfold->GetEmatrixSysBackgroundUncorr("fakes" + label, "fakes" + label);
// Drawer.Draw2D(ErrorMatrix_subBKGuncorr, "ErrorMatrix_subBKGuncorr" + label, true, "Unfolded " + varName, "Unfolded " + varName);
	TH2* ErrorMatrix_subBKGscale = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
	ErrorMatrix_subBKGscale->Reset();
	unfold->GetEmatrixSysBackgroundScale(ErrorMatrix_subBKGscale, "fakes" + label);
// Drawer.Draw2D(ErrorMatrix_subBKGscale, "ErrorMatrix_subBKGscale" + label, true, "Unfolded " + varName, "Unfolded " + varName);
	TH2* ErrorMatrix_MCstat = unfold->GetEmatrixSysUncorr("ErrorMatrix_MCstat");
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat" + label, true, "Unfolded " + varName, "Unfolded " + varName);

// Visualize ERRORS
	TH2* ErrorMatrix = unfold->GetEmatrixTotal("ErrorMatrix");
// TH2* ErrorMatrix = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
// ErrorMatrix->Reset();
// ErrorMatrix->SetName("ErrorMatrix");
// unfold->GetEmatrix(ErrorMatrix);
// ErrorMatrix->Add(ErrorMatrix_MCstat);
	Drawer.Draw2D(ErrorMatrix, "ErrorMatrixTotal" + label, true, "Unfolded " + varName, "Unfolded " + varName);
	writer.addToFile(ErrorMatrix);

//create shift Histos from MatrixErrors
	TH1* ShiftInputUp = (TH1*) unfolded_nominal->Clone();
	ShiftInputUp->SetName("unfolded_" + genvar + "_DataStatUp");
	TH1* ShiftInputDown = (TH1*) unfolded_nominal->Clone();
	ShiftInputDown->SetName("unfolded_" + genvar + "_DataStatDown");
	TH1* ShiftsubBKGuncorrUp = (TH1*) unfolded_nominal->Clone();
	ShiftsubBKGuncorrUp->SetName("unfolded_" + genvar + "_fakeStatUp");
	TH1* ShiftsubBKGuncorrDown = (TH1*) unfolded_nominal->Clone();
	ShiftsubBKGuncorrDown->SetName("unfolded_" + genvar + "_fakeStatDown");
	TH1* ShiftsubBKGscaleUp = (TH1*) unfolded_nominal->Clone();
	ShiftsubBKGscaleUp->SetName("unfolded_" + genvar + "_fakeScaleUp");
	TH1* ShiftsubBKGscaleDown = (TH1*) unfolded_nominal->Clone();
	ShiftsubBKGscaleDown->SetName("unfolded_" + genvar + "_fakeScaleDown");
	TH1* ShiftMCstatUp = (TH1*) unfolded_nominal->Clone();
	ShiftMCstatUp->SetName("unfolded_" + genvar + "_MCStatUp");
	TH1* ShiftMCstatDown = (TH1*) unfolded_nominal->Clone();
	ShiftMCstatDown->SetName("unfolded_" + genvar + "_MCStatDown");



// Variations of MigrationMatrix
	std::vector<TH2*> v_ErrorMatrixVariations;
	std::vector<TH1*> v_ShiftVariations;
	for (auto& var : variations) {
		TH2* tmp = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
		tmp->Reset();
		unfold->GetEmatrixSysSource(tmp, TString(var));
		v_ErrorMatrixVariations.push_back(tmp);
		// Drawer.Draw2D(tmp, "ErrorMatrixVariations_" + TString(var) + label, true, "Unfolded " + varName, "Unfolded " + varName);

		TH1* Delta = (TH1*) GenMC[0].at(0)->Clone();
		Delta->Reset();
		Delta->Sumw2();
		Delta->SetName(TString("shift_") + var);
		unfold->TUnfoldSys::GetDeltaSysSource(Delta, TString(var));
		v_ShiftVariations.push_back(Delta);
		Drawer.Draw1D(Delta, "Shift_" + TString(var) + label, false, "unfolded " + varName);
	}

	TH1D* METTotalError = new TH1D("TotalError", +varName + label, nBins_Gen, BinEdgesGen.data());
	std::vector<double> ESystL;
	std::vector<double> ESystH;
	std::vector<double> BinCenters;
	std::vector<double> BinContents;
	std::vector<double> zeros;
	std::vector<double> EDataStat;
	std::vector<double> EMCStat;
	std::vector<double> EfakeStat;
	std::vector<double> EfakeScale;
	double systerrorL;
	double systerrorH;

//calculate errors on unfolded Points -> all systematic
	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		std::cout << "calculating error in Bin " << bin << std::endl;
		// create templates of error from Covariance Matrices
		ShiftInputUp->AddBinContent(bin, sqrt(ErrorMatrix_input->GetBinContent(bin, bin)));
		ShiftInputDown->AddBinContent(bin, -1 * sqrt((ErrorMatrix_input->GetBinContent(bin, bin))));
		ShiftsubBKGuncorrUp->AddBinContent(bin, sqrt(ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin)));
		ShiftsubBKGuncorrDown->AddBinContent(bin, -1 * sqrt((ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin))));
		ShiftsubBKGscaleUp->AddBinContent(bin, sqrt(ErrorMatrix_subBKGscale->GetBinContent(bin, bin)));
		ShiftsubBKGscaleDown->AddBinContent(bin, -1 * sqrt((ErrorMatrix_subBKGscale->GetBinContent(bin, bin))));
		ShiftMCstatUp->AddBinContent(bin, sqrt(ErrorMatrix_MCstat->GetBinContent(bin, bin)));
		ShiftMCstatDown->AddBinContent(bin, -1 * sqrt((ErrorMatrix_MCstat->GetBinContent(bin, bin))));
		//Get errors from covariance matrices (symmetric errors)
		double symerror = ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin) +
		                  ErrorMatrix_subBKGscale->GetBinContent(bin, bin) +
		                  ErrorMatrix_MCstat->GetBinContent(bin, bin) +
		                  ErrorMatrix_input->GetBinContent(bin, bin);
		//get errors from Shifts (-> Variations of migrationmatrix)
		systerrorL = 0;
		systerrorH = 0;
		for (auto shift : v_ShiftVariations) {
			double error = shift->GetBinContent(bin);
			std::cout << "error from shift " << shift->GetName() << ": " << error << std::endl;
			if (error > 0) systerrorH += error * error;
			else systerrorL += error * error;
		}
		systerrorH += symerror;
		systerrorL += symerror;

		ESystH.push_back(sqrt(systerrorH));
		ESystL.push_back(sqrt(systerrorL));
		zeros.push_back(0);
		TAxis *xaxis = std::get<0>(unfold_output)->GetXaxis();
		Double_t binCenter = xaxis->GetBinCenter(bin);
		BinCenters.push_back(binCenter);
		BinContents.push_back(std::get<0>(unfold_output)->GetBinContent(bin));
	}

	writer.addToFile(ShiftInputUp);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftInputUp}, {"nominal+InputUp"}, "UnfoldedNominalvs(UnfoldedNominal+InputUp)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftInputDown);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftInputDown}, {"nominal+InputDown"}, "UnfoldedNominalvs(UnfoldedNominal+InputDown)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftsubBKGuncorrUp);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftsubBKGuncorrUp}, {"nominal+subBKGuncorrUp"}, "UnfoldedNominalvs(UnfoldedNominal+subBKGuncorrUp)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftsubBKGuncorrDown);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftsubBKGuncorrDown}, {"nominal+subBKGuncorrDown"}, "UnfoldedNominalvs(UnfoldedNominal+subBKGuncorrDown)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftsubBKGscaleUp);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftsubBKGscaleUp}, {"nominal+subBKGscaleUp"}, "UnfoldedNominalvs(UnfoldedNominal+subBKGscaleUp)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftsubBKGscaleDown);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftsubBKGscaleDown}, {"nominal+subBKGscaleDown"}, "UnfoldedNominalvs(UnfoldedNominal+subBKGscaleDown)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftMCstatUp);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftMCstatUp}, {"nominal+MCstatUp"}, "UnfoldedNominalvs(UnfoldedNominal+MCstatUp)" + label, log, !normalize, !drawpull);
	writer.addToFile(ShiftMCstatDown);
	Drawer.DrawDataMC(std::get<0>(unfold_output), {ShiftMCstatDown}, {"nominal+MCstatDown"}, "UnfoldedNominalvs(UnfoldedNominal+MCstatDown)" + label, log, !normalize, !drawpull);


	TGraphAsymmErrors* MET_TGraph = new TGraphAsymmErrors(nBins_Gen,
	        BinCenters.data(),
	        BinContents.data(),
	        zeros.data(),
	        zeros.data(),
	        ESystL.data(),
	        ESystH.data());
	TH2* L = unfold->GetL("L");
	TH2* RhoTotal = unfold->GetRhoIJtotal("RhoTotal");
	std::cout << "#####chi**2 Values#####" << std::endl;
	std::cout << "chi**2_A+chi**2_L/Ndf = "
	          << unfold->GetChi2A() << "+" << unfold->GetChi2L() << " / " << unfold->GetNdf() << "\n"
	          << "chi**2_Sys/Ndf = "
	          << unfold->GetChi2Sys() << " / " << unfold->GetNdf() << "\n"
	          << "AvgRho = "
	          << unfold->GetRhoAvg() << "\n";



// Drawer.Draw2D(ErrorMatrix, "ErrorMatrix" + label, log, "Unfolded MET", "Unfolded MET");
	Drawer.Draw2D(L, "L" + label);
	Drawer.Draw2D(RhoTotal, "RhoTotal" + label, !log, "Unfolded MET", "Unfolded MET");

	Drawer.DrawDataMC(data, MC.at(0), nameRecoSampleColorMap, "MET" + label, log, !normalize, !drawpull, "#slash{E}_{T}", "# Events");

	std::vector<std::string> GenBkgNames;
	for (const std::string& name : bkgnames) {
		GenBkgNames.push_back("Gen_" + name);
	}

	Drawer.Draw1D(std::get<0>(unfold_output), varName + "_unfolded" + label, log);
	Drawer.Draw1D(std::get<1>(unfold_output), varName + "_foldedback" + label, log);
	unfolded_nominal->SetName("unfolded_" + genvar);
	writer.addToFile(unfolded_nominal);
	int nVariation = 0;
	std::vector<TH1*> v_NomPlusVar;
	for (auto& var : variations) {
		TH1* NomPlusVar = (TH1*)std::get<0>(unfold_output)->Clone();
		NomPlusVar->Add(v_ShiftVariations.at(nVariation));
		if (TString(var).Contains("CMS_res") || TString(var).Contains("CMS_scale")) NomPlusVar->SetName("unfolded_" + genvar + TString(var));
		else  NomPlusVar->SetName("unfolded_" + genvar + "_" + TString(var));
		v_NomPlusVar.push_back(NomPlusVar);
		nVariation += 1;
	}

	nVariation = 0;
	for (auto& var : variations) {
		Drawer.DrawDataMC(std::get<0>(unfold_output), {v_NomPlusVar.at(nVariation)}, {"nominal+" + var}, "UnfoldedNominalvs(UnfoldedNominal+" + var + ")" + label, log, !normalize, !drawpull);
		writer.addToFile(v_NomPlusVar.at(nVariation));
		nVariation += 1;
	}

// i = 0;
// for (auto& h : GenMC.at(0)) {
// 	h->Add(misses.at(i), -1);
// 	i++;
// }

	for (const auto& sysvar : GenMC) {
		for (const auto& bkg : sysvar) {
			writer.addToFile(bkg);
		}
	}


	Drawer.DrawDataMCerror(MET_TGraph,
	                       GenMC.at(0),
	                       nameGenSampleColorMap,
	                       varName + "UnfoldedvsGenErrors" + label,
	                       log,
	                       !normalize,
	                       drawpull,
	                       "unfolded #slash{E}_{T}", "# Events");

	Drawer.DrawDataMC(h_DataMinFakes,
	{ std::get<1>(unfold_output) },
	{ "FoldedBack" },
	varName + "DatavsFoldedBack" + label,
	log);

}
