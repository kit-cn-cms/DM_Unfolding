#include "../interface/UnfoldWrapper.hpp"
#include "../interface/Unfolder.hpp"
#include "../interface/HistDrawer.hpp"
#include "../interface/FileWriter.hpp"
#include "../interface/PathHelper.hpp"

#include "boost/lexical_cast.hpp"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>


UnfoldWrapper::UnfoldWrapper(TString varName_, TString label_, std::vector<TH2*> A_, TH1F* data_, std::vector<TH1F*> fakes_, std::vector<TH1*> misses_, std::vector<std::vector<TH1*>> MC_, std::vector<std::vector<TH1*>> GenMC_, std::vector<std::string> variations_, std::vector<std::string> bkgnames_, std::vector<double> BinEdgesGen_): writer(label_) {
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
	// HistDrawer Drawer;
	PathHelper path;

	bool log = true;
	bool moveUF = true;
	bool drawpull = true;
	bool normalize = true;

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(std::string(path.GetConfigPath()), pt);
	TString genvar = pt.get<std::string>("vars.gen");
	TString recovar = pt.get<std::string>("vars.reco");
	TString RecoVariableNameLateX = pt.get<std::string>("vars.recoLateX");

	bool AddMissesbyHand = pt.get<bool>("Unfolding.AddMissesbyHand");
	bool FillFakesinUF = pt.get<bool>("Unfolding.FillFakesinUF");
	bool manualErrors = pt.get<bool>("Unfolding.manualErrors");
	bool doLCurveScan = pt.get<bool>("Unfolding.doLCurveScan");
	bool doRhoScan = pt.get<bool>("Unfolding.doRhoScan");

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
	if (!FillFakesinUF) h_DataMinFakes->Add(fakes.at(0), -1);

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

	//setBias Manually
	// TH1* bias = (TH1*)GenMC[0].at(0)->Clone();
	// bias->Reset();
	// for (auto& h : GenMC[0]) {
	// 	bias->Add(h);
	// }
	// unfold->SetBias(bias);

	TH2* ProbMatrix = (TH2*)A.at(0)->Clone();
	ProbMatrix->Reset();
	unfold->TUnfold::GetProbabilityMatrix(ProbMatrix, TUnfoldDensity::kHistMapOutputVert);
	Drawer.Draw2D(ProbMatrix, "ProbMatrix" + label, !log, !moveUF, "reconstructed " + RecoVariableNameLateX , "generated " + RecoVariableNameLateX);

	if (!FillFakesinUF) unfold->SubtractBackground(fakes.at(0), "fakes" + label, 1., 0.00); // subtract fakes

// addsys variations of MigrationMatrix
	if (!manualErrors) {
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
	}

	// Find Best Tau
	// do plots of scans
	if (doLCurveScan) Unfolder.FindBestTauLcurve(unfold, label);
	if (doRhoScan) Unfolder.FindBestTau(unfold, label);

	//drop regularization
	unfold->DoUnfold(0.0);

// Get Output
// 0st element=unfolded 1st=folded back
	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);
	TH1* unfolded_nominal = std::get<0>(unfold_output);
	unfolded_nominal->Sumw2();
	std::cout << "Unfolded Underflow: " << unfolded_nominal->GetBinContent(0) << std::endl;

// add misses
	if (AddMissesbyHand) {
		i = 0;
		for (auto& h : misses) {
			unfolded_nominal->Add(misses.at(i), +1);
			i++;
		}
	}


// SYST SOURCES
//input data stat error
	TH2* ErrorMatrix_input = unfold->GetEmatrixInput("ErrorMatrix_input");
	Drawer.Draw2D(ErrorMatrix_input, "ErrorMatrix_input" + label, log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);
	writer.addToFile(ErrorMatrix_input);

// subtracted bkgs
	TH2* ErrorMatrix_subBKGuncorr = unfold->GetEmatrixSysBackgroundUncorr("fakes" + label, "fakes" + label);
	Drawer.Draw2D(ErrorMatrix_subBKGuncorr, "ErrorMatrix_subBKGuncorr" + label,  log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);
	TH2* ErrorMatrix_subBKGscale = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
	ErrorMatrix_subBKGscale->Reset();
	unfold->GetEmatrixSysBackgroundScale(ErrorMatrix_subBKGscale, "fakes" + label);
	Drawer.Draw2D(ErrorMatrix_subBKGscale, "ErrorMatrix_subBKGscale" + label,  log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);

	TH2* ErrorMatrix_MCstat = unfold->GetEmatrixSysUncorr("ErrorMatrix_MCstat");
	Drawer.Draw2D(ErrorMatrix_MCstat, "ErrorMatrix_MCstat" + label,  log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);
	writer.addToFile(ErrorMatrix_MCstat);

	TH2* DataMCStat = (TH2*) ErrorMatrix_MCstat->Clone();
	DataMCStat -> Add(ErrorMatrix_input);
	DataMCStat->SetName("ErrorMatrix_DataMCStat");
	writer.addToFile(DataMCStat);

	TH2* DataMCFakeStat = (TH2*) DataMCStat->Clone();
	DataMCFakeStat->Add(ErrorMatrix_subBKGuncorr);
	DataMCFakeStat->SetName("ErrorMatrix_DataMCFakeStat");
	writer.addToFile(DataMCFakeStat);

// Visualize ERRORS
	TH2* ErrorMatrixTotal = unfold->GetEmatrixTotal("ErrorMatrixTotal");
	Drawer.Draw2D(ErrorMatrixTotal, "ErrorMatrixTotal" + label,  log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);
	writer.addToFile(ErrorMatrixTotal);

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
	TH1* ShiftsubBKGscale = nullptr;

	std::vector<TH1*> v_InputStat;

	if (!FillFakesinUF) {
		ShiftsubBKGscale = unfold->GetDeltaSysBackgroundScale("fakes" + label, "shift_bkgScale");
	}
// Variations of MigrationMatrix
	std::vector<TH2*> v_ErrorMatrixVariations;
	std::vector<TH1*> v_ShiftVariations;


	for (auto& var : variations) {
		TH2* tmp = (TH2*)ErrorMatrix_subBKGuncorr->Clone();
		tmp->Reset();
		unfold->GetEmatrixSysSource(tmp, TString(var));
		v_ErrorMatrixVariations.push_back(tmp);
		Drawer.Draw2D(tmp, "ErrorMatrixVariations_" + TString(var) + label,  log, !moveUF, "unfolded " + RecoVariableNameLateX, "unfolded " + RecoVariableNameLateX);

		if (!manualErrors) {
			TH1* Delta = (TH1*) GenMC[0].at(0)->Clone();
			Delta->Reset();
			Delta->Sumw2();
			Delta->SetName(TString("shift_") + var);
			unfold->TUnfoldSys::GetDeltaSysSource(Delta, TString(var));
			v_ShiftVariations.push_back(Delta);
			Drawer.Draw1D(Delta, "Shift_" + TString(var) + label, false, "unfolded " + varName);

			TH1* NomPlusVar = (TH1*) unfolded_nominal->Clone();
			NomPlusVar->Add(Delta);
			NomPlusVar->SetName("unfolded_" + genvar + "_" + TString(var));
			writer.addToFile(NomPlusVar);
			// Drawer.DrawDataMC(unfolded_nominal, {NomPlusVar}, {var}, "UnfoldedNominalvs(UnfoldedNominal+" + var + ")" + label, log, !normalize, !drawpull, "unfolded " + RecoVariableLateX);

		}
	}

	for (auto& A_var : A) {
		for (auto& varname : variations) {
			if (TString(A_var->GetName()).Contains(varname)) {
				std::cout << "getting shift from " << varname << " as systematic" << std::endl;
				A_var->Print();
				std::cout << "Mean gen " << A_var->GetMean(1) << " vs nominal: " <<  A.at(0)->GetMean(1) << std::endl;
				std::cout << "Mean reco " << A_var->GetMean(2) << " vs nominal: " <<  A.at(0)->GetMean(2) << std::endl;

				if (manualErrors) {
					TH1* Delta = GetSysShift(A_var, varname, genvar, unfolded_nominal);
					Delta->Sumw2();
					Delta->SetName(TString("shift_") + varname);
					v_ShiftVariations.push_back(Delta);
					Drawer.Draw1D(Delta, "Shift_" + TString(varname) + label, false, "unfolded " + varName);
				}
			}
		}
	}


	TH1D* TotalError = new TH1D("TotalError", +varName + label, nBins_Gen, BinEdgesGen.data());
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

		TH1* tmphistUp = (TH1*) ShiftInputUp->Clone();
		tmphistUp->Reset();
		tmphistUp->SetBinContent(bin, ShiftInputUp->GetBinContent(bin));
		tmphistUp->SetName( TString::Format("unfolded_" + genvar + "_Input_Bin%iUp", bin));
		writer.addToFile(tmphistUp);

		TH1* tmphistDown = (TH1*) ShiftInputDown->Clone();
		tmphistDown->Reset();
		tmphistDown->SetBinContent(bin, ShiftInputDown->GetBinContent(bin));
		tmphistDown->SetName( TString::Format("unfolded_" + genvar + "_Input_Bin%iDown", bin));
		writer.addToFile(tmphistDown);

		ShiftsubBKGuncorrUp->AddBinContent(bin, sqrt(ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin)));
		ShiftsubBKGuncorrDown->AddBinContent(bin, -1 * sqrt(ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin)));

		if (!FillFakesinUF) {
			ShiftsubBKGscaleUp->AddBinContent(bin, ShiftsubBKGscale->GetBinContent(bin, bin));
			ShiftsubBKGscaleDown->AddBinContent(bin, -1 * (ShiftsubBKGscale->GetBinContent(bin, bin)));
		}
		ShiftMCstatUp->AddBinContent(bin, sqrt(ErrorMatrix_MCstat->GetBinContent(bin, bin)));
		ShiftMCstatDown->AddBinContent(bin, -1 * sqrt((ErrorMatrix_MCstat->GetBinContent(bin, bin))));
		//Get errors from covariance matrices (symmetric errors)
		double symerror = ErrorMatrix_subBKGuncorr->GetBinContent(bin, bin) +
		                  ErrorMatrix_subBKGscale->GetBinContent(bin, bin) +
		                  ErrorMatrix_MCstat->GetBinContent(bin, bin);// +
		// ErrorMatrix_input->GetBinContent(bin, bin);
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


	TGraphAsymmErrors* TGraph = new TGraphAsymmErrors(nBins_Gen,
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
	          << unfold->GetRhoAvg() << "\n"
	          << "RhoMax = "
	          << unfold->GetRhoMax() << "\n";


// Drawer.Draw2D(ErrorMatrix, "ErrorMatrix" + label, log, "unfolded "+ varName, "unfolded "+ varName);
	Drawer.Draw2D(L, "L" + label);
	Drawer.Draw2D(RhoTotal, "RhoTotal" + label, !log, !moveUF, "unfolded " + varName, "unfolded " + varName);

	//calculate correlationmatrix for input+MCStat
	TMatrixDSym cov(DataMCFakeStat->GetNbinsX());
	TMatrixDSym covdiag(DataMCFakeStat->GetNbinsX());
	TMatrixD corr(DataMCFakeStat->GetNbinsX(), DataMCFakeStat->GetNbinsX());

	for (int i = 0; i < DataMCFakeStat->GetNbinsX(); i++) {
		for (int j = i; j < DataMCFakeStat->GetNbinsX(); j++) {
			if (j != i) {
				cov(i, j) = DataMCFakeStat->GetBinContent(i + 1 , j + 1) ;
				cov(j, i) = DataMCFakeStat->GetBinContent(i + 1 , j + 1) ;
				covdiag(i, j) = 0;
				covdiag(j, i) = 0;
			}
			else {
				cov(i, i) = DataMCFakeStat->GetBinContent(i + 1  , j + 1);
				covdiag(i, i) = 1. / sqrt(cov(i, i));
			}
		}
	}

	// cov.Print();
	// covdiag.Print();

	corr = covdiag * cov * covdiag;
	std::cout << "Correlation matric originating from Data and MC statistical fluctuations:" << std::endl;
	corr.Print();
	TH2* h_corrDataMCstat = new TH2D(corr);
	h_corrDataMCstat->SetName("h_corrDataMCstat");
	Drawer.Draw2D(h_corrDataMCstat, "RhoDataMCstat" + label, !log, !moveUF, "unfolded Bin Number", "unfolded Bin Number");


	Drawer.DrawDataMC(data, MC.at(0), nameRecoSampleColorMap, recovar + label, log, !normalize, !drawpull, varName, "# Events");

	std::vector<std::string> GenBkgNames;
	for (const std::string& name : bkgnames) {
		GenBkgNames.push_back("Gen_" + name);
	}

	Drawer.Draw1D(std::get<0>(unfold_output), recovar + "_unfolded" + label, varName, log);
	Drawer.Draw1D(std::get<1>(unfold_output), recovar + "_foldedback" + label, varName, log);
	unfolded_nominal->SetName("unfolded_" + genvar);
	writer.addToFile(unfolded_nominal);

	for (const auto& sysvar : GenMC) {
		for (const auto& bkg : sysvar) {
			writer.addToFile(bkg);
			TH1* bkgcopy = (TH1*) bkg->Clone();
			TString tmpName = bkg->GetName();
			// rename systematic histos from BosonReweighting to account for correct correlations
			if (tmpName.Contains("z_nunu_jets_" + genvar + "_BosonWeight_EW2") or tmpName.Contains("z_nunu_jets_" + genvar + "_BosonWeight_EW3") or tmpName.Contains("z_nunu_jets_" + genvar + "_BosonWeight_Mixed")) {
				bkgcopy->SetName(tmpName.ReplaceAll("BosonWeight", "ZvvBosonWeight"));
			}
			if (tmpName.Contains("z_ll_jets_" + genvar + "_BosonWeight_EW2") or tmpName.Contains("z_ll_jets_" + genvar + "_BosonWeight_EW3") or tmpName.Contains("z_ll_jets_" + genvar + "_BosonWeight_Mixed")) {
				bkgcopy->SetName(tmpName.ReplaceAll("BosonWeight", "ZllBosonWeight"));
			}
			if (tmpName.Contains("w_lnu_jets_" + genvar + "_BosonWeight_EW2") or tmpName.Contains("w_lnu_jets_" + genvar + "_BosonWeight_EW3") or tmpName.Contains("w_lnu_jets_" + genvar + "_BosonWeight_Mixed")) {
				bkgcopy->SetName(tmpName.ReplaceAll("BosonWeight", "WlnuBosonWeight"));
			}

			// rename systematic histos from BosonReweighting to account for correct correlations
			if (tmpName.Contains("z_nunu_jets_" + genvar + "_Weight_scale_variation")) {
				bkgcopy->SetName(tmpName.ReplaceAll("Weight", "amc"));
			}
			if (tmpName.Contains("z_ll_jets_" + genvar + "_Weight_scale_variation")) {
				bkgcopy->SetName(tmpName.ReplaceAll("Weight", "amc"));
			}
			if (tmpName.Contains("w_lnu_jets_" + genvar + "_Weight_scale_variation")) {
				bkgcopy->SetName(tmpName.ReplaceAll("Weight", "amc"));
			}
			writer.addToFile(bkgcopy);
		}
	}


	Drawer.DrawDataMCerror(TGraph,
	                       GenMC.at(0),
	                       nameGenSampleColorMap,
	                       recovar + "UnfoldedvsGenErrors" + label,
	                       log,
	                       !normalize,
	                       drawpull,
	                       "unfolded " + varName, "# Events");

	Drawer.DrawDataMC(h_DataMinFakes, { std::get<1>(unfold_output) }, { "folded back" }, "foldedBack" + label,	log, !normalize, !drawpull, RecoVariableNameLateX);

}


TH1* UnfoldWrapper::GetSysShift(TH2* A_variated, TString variationName, TString variableName, TH1* nominalUnfolded) {
	Unfolder Unfolder;
	Unfolder.ParseConfig();

	TUnfoldDensity* unfold = Unfolder.SetUp(A_variated, data);
	unfold->DoUnfold(0);

	std::tuple<TH1*, TH1*> unfold_output;
	unfold_output = Unfolder.GetOutput(unfold);

	TH1* NomPlusVar = (TH1*) std::get<0>(unfold_output)->Clone();
	NomPlusVar->SetName("unfolded_" + variableName + "_" + TString(variationName));
	// bool log = true;
	// bool normalize = true;
	// bool drawpull = true;
	// this->Drawer.DrawDataMC(nominalUnfolded, {NomPlusVar}, {variationName}, "UnfoldedNominalvs(UnfoldedNominal+" + variationName + ")" + label, log, !normalize, !drawpull, "unfolded "+RecoVariableNameLateX);



	// for (int iBin = 1; iBin < NomPlusVar->GetNbinsX(); iBin++) {
	// 	double diff = NomPlusVar->GetBinContent(iBin) - nominalUnfolded->GetBinContent(iBin);
	// 	if (abs(diff) < 2) NomPlusVar->SetBinContent(iBin, NomPlusVar->GetBinContent(iBin) + 100 * diff);
	// }

	TH1* shift = (TH1*) nominalUnfolded->Clone();
	shift->Add(NomPlusVar, -1);
	writer.addToFile(NomPlusVar);

	return shift;
}