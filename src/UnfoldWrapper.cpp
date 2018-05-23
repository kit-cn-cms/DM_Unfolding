#include "../interface/UnfoldWrapper.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/FileWriter.hpp"
#include "../interface/PathHelper.hpp"

#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>


UnfoldWrapper::UnfoldWrapper(TString varName_, TString label_, std::vector<TH2*> A_, TH1F* data_, TH1* fakes_, std::vector<std::vector<TH1*>> MC_, std::vector<std::vector<TH1*>> GenMC_, std::vector<std::string> variations_, std::vector<std::string> bkgnames_, std::vector<double> BinEdgesGen_): writer(label_) {
	varName = varName_;
	label = label_;
	A = A_;
	data = data_;
	fakes = fakes_;
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

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(std::string(path.GetConfigPath()), pt);
	TString genvar = pt.get<std::string>("vars.gen");
	TString recovar = pt.get<std::string>("vars.reco");

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

	int nBins_Gen = GenMC[0].at(0)->GetNbinsX();
	Unfolder.ParseConfig();
	TUnfoldDensity* unfold = Unfolder.SetUp(A.at(0), data);
	TH2* ProbMatrix = (TH2*)A.at(0)->Clone();
	ProbMatrix->Reset();
	unfold->TUnfold::GetProbabilityMatrix(ProbMatrix, TUnfoldDensity::kHistMapOutputVert);
	Drawer.Draw2D(ProbMatrix, "ProbMatrix" + label, false, varName + "_Reco", varName + "_Gen");

	unfold->SubtractBackground(fakes, "fakes" + label, 1, 0.0); // subtract fakes

// addsys variations of MigrationMatrix
	int nVariation = 0;
	for (auto& var : variations) {
		unfold->AddSysError(A.at(nVariation),
		                    TString(var),
		                    TUnfoldDensity::kHistMapOutputVert,
		                    TUnfoldDensity::kSysErrModeMatrix);
		nVariation += 1;
	}

// unfold->SetBias(GenMC.at(0));

// Find Best Tau
	Unfolder.FindBestTauLcurve(unfold, label);
// Unfolder.FindBestTau(unfold, label);
// unfold->DoUnfold(0.000316228);

// Get Output
// 0st element=unfolded 1st=folded back
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);

// Visualize ERRORS
	TH2* ErrorMatrix = unfold->GetEmatrixTotal("ErrorMatrix" + label);

// STAT SOURCES
	TH2* ErrorMatrix_input = unfold->GetEmatrixInput("ErrorMatrix_input" + label);
	Drawer.Draw2D(ErrorMatrix_input, "ErrorMatrix_input" + label, true, "Unfolded " + varName, "Unfolded " + varName);

// SYST SOURCES
// subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr =
	    unfold->GetEmatrixSysBackgroundUncorr("fakes" + label, "fakes" + label);
	// Drawer.Draw2D(ErrorMatrix_subBKGuncorr, "ErrorMatrix_subBKGuncorr" + label, true, "Unfolded " + varName, "Unfolded " + varName);
	TH2* ErrorMatrix_subBKGscale = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
	ErrorMatrix_subBKGscale->Reset();
	unfold->GetEmatrixSysBackgroundScale(ErrorMatrix_subBKGscale, "fakes" + label);
	// Drawer.Draw2D(ErrorMatrix_subBKGscale, "ErrorMatrix_subBKGscale" + label, true, "Unfolded " + varName, "Unfolded " + varName);
	TH2* ErrorMatrix_MCstat = unfold->GetEmatrixSysUncorr("ErrorMatrix_MCstat");
	// Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat" + label, true, "Unfolded " + varName, "Unfolded " + varName);

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
		unfold->TUnfoldSys::GetDeltaSysSource(Delta, TString(var));
		v_ShiftVariations.push_back(Delta);
		Drawer.Draw1D(Delta, "Shift_" + TString(var) + label, false, "unfolded " + varName);
	}

	TH1D* METTotalError = new TH1D("TotalError", +varName + label, nBins_Gen, BinEdgesGen.data());
	std::vector<double> EStat;
	std::vector<double> ESystL;
	std::vector<double> ESystH;
	std::vector<double> BinCenters;
	std::vector<double> BinContents;
	std::vector<double> TotalError;
	std::vector<double> zeros;
	double systerrorL;
	double systerrorH;

	for (Int_t bin = 1; bin <= nBins_Gen; bin++) {
		double staterror = ErrorMatrix_input->GetBinContent(bin, bin);
		EStat.push_back(sqrt(staterror));
		double systerror = ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin) +
		                   ErrorMatrix_subBKGscale->GetBinContent(bin, bin) +
		                   ErrorMatrix_MCstat->GetBinContent(bin, bin);

		for (auto shift : v_ShiftVariations) {
			float error = shift->GetBinContent(bin, bin);
			if (error > 0) systerrorH = error * error;
			else systerrorL = error * error ;
		}
		systerrorH += systerror + staterror ;
		systerrorL += systerror + staterror ;

		ESystH.push_back(sqrt(systerrorH));
		ESystL.push_back(sqrt(systerrorL));
		zeros.push_back(0);
		TotalError.push_back(sqrt(staterror + systerror));
		TAxis *xaxis = std::get<0>(unfold_output)->GetXaxis();
		Double_t binCenter = xaxis->GetBinCenter(bin);
		BinCenters.push_back(binCenter);
		BinContents.push_back(std::get<0>(unfold_output)->GetBinContent(bin));
	}

	TGraphErrors* MET_Stat = new TGraphErrors(nBins_Gen,
	        BinCenters.data(),
	        BinContents.data(),
	        zeros.data(),
	        EStat.data());
	TGraphAsymmErrors* MET_Syst = new TGraphAsymmErrors(nBins_Gen,
	        BinCenters.data(),
	        BinContents.data(),
	        zeros.data(),
	        zeros.data(),
	        ESystL.data(),
	        ESystH.data());
	TH2* L = unfold->GetL("L");
	TH2* RhoTotal = unfold->GetRhoIJtotal("RhoTotal");
	std::cout << "#####chi**2 from DummyData#####" << std::endl;
	std::cout << "chi**2_A+chi**2_L/Ndf = "
	          << unfold->GetChi2A() << "+" << unfold->GetChi2L() << " / " << unfold->GetNdf() << "\n"
	          << "chi**2_Sys/Ndf = "
	          << unfold->GetChi2Sys() << " / " << unfold->GetNdf() << "\n";

	bool log = true;
	bool drawpull = true;
	bool normalize = true;
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

	nVariation = 0;
	std::vector<TH1*> v_NomPlusVar;
	for (auto& var : variations) {
		TH1* NomPlusVar = (TH1*)std::get<0>(unfold_output)->Clone();
		NomPlusVar->Add(v_ShiftVariations.at(nVariation));
		NomPlusVar->SetName("unfolded_" + genvar + TString(var));
		v_NomPlusVar.push_back(NomPlusVar);
		nVariation += 1;
	}

	nVariation = 0;
	for (auto& var : variations) {
		Drawer.DrawDataMC(v_NomPlusVar.at(nVariation), {std::get<0>(unfold_output)}, {"nominal+" + var}, var + "vsNominal" + label, log, !normalize, drawpull);
		writer.addToFile(v_NomPlusVar.at(nVariation));
		nVariation += 1;
	}


	for (const auto& sysvar : GenMC) {
		for (const auto& bkg : sysvar) {
			writer.addToFile(bkg);
		}
	}

	Drawer.DrawDataMCerror(MET_Stat,
	                       MET_Syst,
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