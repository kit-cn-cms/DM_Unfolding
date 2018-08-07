using namespace std;

TH1* processSignal(TString SignalName) {
	cout << "processing " << SignalName << endl;
	// TString ntuples_path = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_tagging";
	TString ntuples_path = "/nfs/dust/cms/user/swieland/Darkmatter/ntuples_signal_madgraph_tagging";
	TChain* chain = new TChain("MVATree");
	chain->Add(TString(ntuples_path + "/" + SignalName + "/" + SignalName + "*_nominal_Tree.root"));
	cout << "Processing " << chain->GetEntries() << " entries" << endl;
	// double bins_met[13] = {200., 240., 300., 380., 470., 570., 670., 790., 910., 1040., 1180., 1330., 2000. };
	// double bins_met[13] = {200., 250., 300., 350., 450., 550., 650., 750., 850., 1000., 1150., 1300., 1400 };
	double bins_met[11] = {200., 300., 400., 500., 600., 700., 800., 900., 1000., 1200., 1400. };


	TH1F* h_sig(nullptr);
	h_sig = new TH1F("h_sig", "Signal", sizeof(bins_met) / sizeof(bins_met[0]) - 1 , bins_met );

	TH1F* h_sig_MuRup(nullptr);
	h_sig_MuRup = new TH1F("h_sig_MuRup", "SignalMuRup", sizeof(bins_met) / sizeof(bins_met[0]) - 1 , bins_met );

	TH1F* h_sig_MuRdown(nullptr);
	h_sig_MuRdown = new TH1F("h_sig_MuRdown", "SignalMuRdown", sizeof(bins_met) / sizeof(bins_met[0]) - 1 , bins_met );

	TH1F* h_sig_MuFup(nullptr);
	h_sig_MuFup = new TH1F("h_sig_MuFup", "SignalMuFup", sizeof(bins_met) / sizeof(bins_met[0]) - 1 , bins_met );

	TH1F* h_sig_MuFdown(nullptr);
	h_sig_MuFdown = new TH1F("h_sig_MuFdown", "SignalMuFdown", sizeof(bins_met) / sizeof(bins_met[0]) - 1 , bins_met );

	TTreeReader theReader(chain);
	TTreeReaderValue<Float_t> GenMET(theReader, "Evt_Pt_GenMET");
	TTreeReaderValue<Float_t> RecoMET(theReader, "Evt_Pt_MET");

	//triggers
	TTreeReaderValue<Long64_t> Triggered_HLT_PFMET170_X (theReader, "Triggered_HLT_PFMET170_X");
	TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X (theReader,  "Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X" );
	TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X (theReader,  "Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X" );
	TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X (theReader,  "Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X" );
	TTreeReaderValue<Long64_t> Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X (theReader,  "Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X" );

	//bools
	TTreeReaderValue<Long64_t> Miss (theReader,  "Miss" );
	TTreeReaderValue<Long64_t> Fake (theReader,  "Fake" );
	TTreeReaderValue<Long64_t> recoSelected (theReader,  "recoSelected" );
	TTreeReaderValue<Long64_t> genSelected (theReader,  "genSelected" );

	//weights
	TTreeReaderValue<Float_t> Weight_XS (theReader, "Weight_XS" );
	TTreeReaderValue<Float_t> Weight_GEN_nom (theReader, "Weight_GEN_nom" );
	TTreeReaderValue<Float_t> Weight_PU (theReader, "Weight_pu69p2" );
	TTreeReaderValue<Float_t> Weight_CSV (theReader, "Weight_CSV" );
	TTreeReaderValue<Float_t> Weight_MuRup(theReader, "Weight_scale_variation_muR_2p0_muF_1p0" );
	TTreeReaderValue<Float_t> Weight_MuRdown(theReader, "Weight_scale_variation_muR_0p5_muF_1p0" );
	TTreeReaderValue<Float_t> Weight_MuFup(theReader, "Weight_scale_variation_muR_1p0_muF_2p0" );
	TTreeReaderValue<Float_t> Weight_MuFdown(theReader, "Weight_scale_variation_muR_1p0_muF_0p5" );

	Float_t varDeltaPhi_Jet_MET[100];
	chain->SetBranchAddress("DeltaPhi_Jet_MET", &varDeltaPhi_Jet_MET);


	chain->LoadTree(-1);
	long iEntry = 0;
	long misscounter = 0;
	while (theReader.Next()) {
		if (iEntry % 50000 == 0) cout << "Processing Event Nr.: " << iEntry << endl;

		// theReader.SetEntry(iEntry);
		chain->GetEntry(iEntry);
		bool dPhiCut = varDeltaPhi_Jet_MET[0] > 1.0;
		// cout << *varDeltaPhi_Jet_MET << endl;
		double weight_ = (*Weight_XS) * (*Weight_CSV) * (*Weight_PU) * (*Weight_GEN_nom) * 35.91823;
		bool triggered = *Triggered_HLT_PFMET170_X || *Triggered_HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_X || *Triggered_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_X || *Triggered_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_X || *Triggered_HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_X;
		// bool miss = *genSelected && (!*recoSelected || !dPhiCut || !triggered);
		bool gensel = *genSelected;
		// bool gensel = *var_gen > 200 && *GenMonoJetSelection && *GenLeptonVetoSelection && *GenBTagVetoSelection && *GenPhotonVetoSelection && *GenMETSelection && *GenmonoVselection;

		bool miss = *genSelected && (!*recoSelected || !dPhiCut || !triggered || *RecoMET < 250);
		// bool miss = gensel && (!*recoSelected || !dPhiCut || !triggered);
		// if (triggered && *recoSelected && dPhiCut && *genSelected && *RecoMET > 250) {
		if (gensel) {
			h_sig->Fill(*GenMET, weight_);
			h_sig_MuRup->Fill(*GenMET, weight_ * fabs(*Weight_MuRup));
			h_sig_MuRdown->Fill(*GenMET, weight_ * fabs(*Weight_MuRdown));
			h_sig_MuFup->Fill(*GenMET, weight_ * fabs(*Weight_MuFup));
			h_sig_MuFdown->Fill(*GenMET, weight_ * fabs(*Weight_MuFdown));
		}
		// if (miss) {
		// 	misscounter += weight_;
		// 	h_sig->Fill(*GenMET, weight_);
		// 	h_sig_MuRup->Fill(*GenMET, weight_ * fabs(*Weight_MuRup));
		// 	h_sig_MuRdown->Fill(*GenMET, weight_ * fabs(*Weight_MuRdown));
		// 	h_sig_MuFup->Fill(*GenMET, weight_ * fabs(*Weight_MuFup));
		// 	h_sig_MuFdown->Fill(*GenMET, weight_ * fabs(*Weight_MuFdown));
		// }
		iEntry++;
	}
	// h_sig->Draw();
	cout << misscounter << " misses in " << SignalName << endl;
	h_sig->SetName(SignalName);
	h_sig_MuRup->SetName(SignalName + "_Weight_scale_variation_muRUp");
	h_sig_MuRdown->SetName(SignalName + "_Weight_scale_variation_muRDown");
	h_sig_MuFup->SetName(SignalName + "_Weight_scale_variation_muFUp");
	h_sig_MuFdown->SetName(SignalName + "_Weight_scale_variation_muFDown");

	h_sig->Write();
	h_sig_MuRup->Write();
	h_sig_MuRdown->Write();
	h_sig_MuFup->Write();
	h_sig_MuFdown->Write();
	return h_sig;

}



void processSignalSamples(const char* signalname = "Axial_MonoJ_NLO_Mphi-1000_Mchi-300_gSM-0p25_gDM-1p0_13TeV-madgraph", const char* filename = "/nfs/dust/cms/user/swieland/Darkmatter/DM_Unfolding/rootfiles/signals.root") {

	// TFile* outputfile = new TFile(filename, "UPDATE");
	TFile* outputfile = new TFile("rootfiles/signals/" + TString(signalname) + ".root", "UPDATE");
	// for (auto& name : Names_madgraph) processSignal(name);
	processSignal(signalname);
	// for (auto& name : Names) processSignal(name);

}
