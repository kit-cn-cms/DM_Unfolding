import ROOT as r
from array import array
import numpy as np


class Unfolder:
    def __init__(self, mat, data, label = "" ,regMode=r.TUnfoldDensity.kRegModeCurvature,
                 constrainArea=True, densityMode=r.TUnfoldDensity.kDensityModeBinWidth,
                 biasScale=0, nScan=100, tauMin=0.00001, tauMax=0.9, outputFileName = "outputUnfolding.root"):
        self.mat = mat
        self.input = data
        self.regMode = regMode
        if label != "":
            label = "_"+label
        self.label = label
        self.constrainArea = constrainArea
        if self.constrainArea:
            constrain = r.TUnfold.kEConstraintArea
        else:
            constrain = r.TUnfold.kEConstraintNone
        self.densityMode = densityMode
        self.unf = r.TUnfoldDensity(self.mat, self.regMode, r.TUnfoldDensity.kRegModeCurvature, constrain,
                                    self.densityMode)
        self.tau = 0
        self.biasScale = biasScale
        self.nScan = nScan
        self.tauMin = tauMin
        self.tauMax = tauMax
        self.file = r.TFile(outputFileName,"RECREATE")

        self.didUnfolding = False
        n = self.unf.SetInput(self.input)
        if n >= 1:
            print "Unfolding result may be wrong\n"
                
    def setTau(self, tau):
        self.tau = tau

    def doUnfolding(self, tau=0):
        self.tau = tau
        self.unf.DoUnfold(self.tau, self.input, self.biasScale)
        self.didUnfolding = True
        self.printChiVals()

    def subBkg(self, h_bkg, name):
        self.unf.SubtractBackground(h_bkg, name)

    def getResult(self):
        if self.didUnfolding:
            h = self.unf.GetOutput("unfolded")
            h.Write()
            return h
        else:
            raise RuntimeError("Unfolding needs to be performed before accessing the unfolded result")

    def getFoldedBack(self):
        if self.didUnfolding:
            h =  self.unf.GetFoldedOutput("unfoldedBack")
            h.Write()
            return h
        else:
            raise RuntimeError("Unfolding needs to be performed before accessing the BackFolded result")

    def getRegMatrix(self):
        histL = self.unf.GetL("L")
        for j in range(1, histL.GetNbinsY() + 1):
            print "L[", self.unf.GetLBinning().GetBinName(j), "]"
            for i in range(1, histL.GetNbinsY() + 1):
                c = histL.GetBinContent(i, j)
                if c != 0:
                    print " [", i, "]=", c

    def getNormVector(self):
        h = self.unf.GetNormalisationVector(normvector)
        h.Write()
        return h

    def printChiVals(self):
        print "#" * 50
        print "Tau          = ", self.tau
        print "chi**2_A     = ", self.unf.GetChi2A()
        print "chi**2_L     = ", self.unf.GetChi2L()
        print "chi**2_Sys   = ", self.unf.GetChi2Sys()
        print "Ndf          = ", self.unf.GetNdf()
        print "AvgRho       = ", self.unf.GetRhoAvg()
        print "RhoMax       = ", self.unf.GetRhoMax()
        print "#" * 50

    def findBestTau(self, mode="glCorr", ScanMode=r.TUnfoldDensity.kEScanTauRhoAvg):
        if mode == "glCorr":
            print "#" * 50
            print "Finding best tau via Scan of global correlations"
            print "#" * 50
            scanResult = r.MakeNullPointer(r.TSpline)
            # lCurve = r.MakeNullPointer(r.TGraph)
            iBest = self.unf.ScanTau(self.nScan, self.tauMin, self.tauMax, scanResult, ScanMode)
            self.tau = self.unf.GetTau()
            self.didUnfolding = True
            self.printChiVals()
            t = r.Double()
            rho = r.Double()

            scanResult.GetKnot(iBest, t, rho)
            bestRhoLogTau = r.TGraph(1, array('d', [t]), array('d', [rho]))

            tAll = [] * self.nScan
            rhoAll = [] * self.nScan
            for i in range(self.nScan):
                tmpX = r.Double()
                tmpY = r.Double()
                scanResult.GetKnot(i, tmpX, tmpY)
                tAll.append(tmpX)
                rhoAll.append(tmpY)
            knots = r.TGraph(self.nScan, array('d', tAll), array('d', rhoAll))

            # Draw Tau Scan
            tau = r.TCanvas("tau_RhoScan", "tau_RhoScan")
            tau.cd()
            tau.SetBottomMargin(0.15)
            tau.SetLeftMargin(0.15)
            knots.SetTitle("")
            knots.GetXaxis().SetTitle("log #tau")
            knots.GetXaxis().SetTitleSize(0.06)
            knots.GetXaxis().SetLabelSize(0.05)

            knots.GetYaxis().SetTitle("#bar{#rho}")
            knots.GetYaxis().SetTitleSize(0.06)
            knots.GetYaxis().SetLabelSize(0.05)

            knots.Draw("A*")
            bestRhoLogTau.SetMarkerColor(r.kRed)
            bestRhoLogTau.Draw("*")
            scanResult.Draw("same")

            pt = r.TPaveText(0.2, .95, 0.9, 0.99, "blNDC")
            pt.AddText("CMS #it{private work}")
            pt.SetFillColor(r.kWhite)
            pt.SetTextSize(0.055)
            pt.Draw("SAME")

            r.gPad.Update()

            tau.SaveAs("pdfs/tau_RhoScan"+self.label+".pdf")
            # tau.SaveAs("pngs/tau_RhoScan"+self.label+".png")
            tau.Write()

        elif mode == "LCurve":
            print "#" * 50
            print "Finding best tau via LCurve Scan"
            print "#" * 50
            logTauX = r.MakeNullPointer(r.TSpline)
            logTauY = r.MakeNullPointer(r.TSpline)
            logTauCurvature = r.MakeNullPointer(r.TSpline)
            lCurve = r.MakeNullPointer(r.TGraph)

            iBest = self.unf.ScanLcurve(self.nScan, self.tauMin, self.tauMax, lCurve, logTauX, logTauY, logTauCurvature)
            self.tau = self.unf.GetTau()
            self.didUnfolding = True
            self.printChiVals()

            # save point corresponding to the kink in the L curve as TGraph
            t = r.Double()
            x = r.Double()
            y = r.Double()
            c = r.Double()
            logTauX.GetKnot(iBest, t, x)
            logTauY.GetKnot(iBest, t, y)
            logTauCurvature.GetKnot(iBest, t, c)
            bestLcurve = r.TGraph(1, array('d', [x]), array('d', [y]))
            bestLogTauX = r.TGraph(1, array('d', [t]), array('d', [x]))
            bestCLogTau = r.TGraph(1, array('d', [t]), array('d', [c]))

            tAll = [] * self.nScan
            cAll = [] * self.nScan
            for i in range(self.nScan):
                tmpX = r.Double()
                tmpY = r.Double()
                logTauCurvature.GetKnot(i, tmpX, tmpY)
                tAll.append(tmpX)
                cAll.append(tmpY)
            curvknots = r.TGraph(self.nScan, array('d', tAll), array('d', cAll))

            tau = r.TCanvas("logtauvsChi2_LCurveScan", "logtauvsChi2_LCurveScan")
            tau.cd()
            logTauX.Draw()

            bestLogTauX.SetMarkerColor(r.kRed)
            bestLogTauX.Draw("*")
            tau.SaveAs("pdfs/logtauvsChi2" + self.label + ".pdf");
            tau.SaveAs("pngs/logtauvsChi2" + self.label + ".png");

            curv = r.TCanvas("curvature_LCurveScan", "curvature_LCurveScan")
            curv = r.TCanvas("curvature_LCurveScan", "curvature_LCurveScan")
            curv.cd()
            curv.SetBottomMargin(0.15)
            curv.SetLeftMargin(0.15)
            logTauCurvature.Draw()
            curvknots.Draw("*")
            bestCLogTau.SetMarkerColor(r.kRed)
            bestCLogTau.Draw("*")
            curv.SaveAs("pdfs/curvature_LCurveScan"+self.label+".pdf")
            curv.SaveAs("pngs/curvature_LCurveScan"+self.label+".png")
            curv.Write()

            clCurve = r.TCanvas("LCurve", "LCurve")
            clCurve.cd()
            clCurve.SetBottomMargin(0.15)
            clCurve.SetLeftMargin(0.15)
            lCurve.SetTitle("")
            lCurve.Draw("AL")
            lCurve.GetXaxis().SetTitle("log #chi_{A}")
            lCurve.GetXaxis().SetTitleSize(0.06)
            lCurve.GetXaxis().SetLabelSize(0.05)

            lCurve.GetYaxis().SetTitle("log #chi_{L}")
            lCurve.GetYaxis().SetTitleSize(0.05)
            lCurve.GetYaxis().SetLabelSize(0.05)
            bestLcurve.SetMarkerColor(r.kRed)
            bestLcurve.Draw("*")

            pt = r.TPaveText(0.2, .95, 0.9, 0.99, "blNDC")
            pt.AddText("CMS #it{private work}")
            pt.SetFillColor(r.kWhite)
            pt.SetTextSize(0.055)
            pt.Draw("SAME")

            clCurve.SaveAs("pdfs/LCurve"+self.label+".pdf")
            clCurve.Print("pngs/LCurve"+self.label+".png")
            clCurve.Print()
        else:
            raise RuntimeError("FindBestTau: Invalid Option '", mode, "' Only glCorr and LCurve possible")

    def addSystematic(self, h_sys, sysname):
        return self.unf.AddSysError(h_sys, sysname, r.TUnfoldDensity.kHistMapOutputVert, r.TUnfoldDensity.kSysErrModeMatrix)

    def getProbMatrix(self):
        if self.didUnfolding:
            h = self.unf.GetProbabilityMatrix("ProbMatrix")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getErrorMatrixInput(self):
        if self.didUnfolding:
            h = self.unf.GetEmatrixInput("ErrorMatrixInput")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getErrorMatrixSubBKGuncorr(self, label):
        if self.didUnfolding:
            h = self.unf.GetEmatrixSysBackgroundUncorr(label, label)
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")
        
    def getErrorMatrixSubBKGscale(self, label):
        if self.didUnfolding:
            EMatrix = self.mat.Clone()
            EMatrix.Reset()
            h = self.unf.GetEmatrixSysBackgroundScale(EMatrix, label)
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getErrorMatrixMCstat(self):
        if self.didUnfolding:
            h = self.unf.GetEmatrixSysUncorr("ErrorMatrixMCstat")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getErrorMatrixDataMCstat(self):
        if self.didUnfolding:
            DataMCstat = self.getErrorMatrixMCstat().Clone()
            DataMCstat.Add(self.getErrorMatrixInput())
            DataMCstat.SetName("ErrorMatrixDataMCStat")
            DataMCstat.Write()
            return DataMCstat
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getErrorMatrixTotal(self):
        if self.didUnfolding:
            h = self.unf.GetEmatrixTotal("ErrorMatrixTotal")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getCorrMatrixTotal(self):
        if self.didUnfolding:
            h = self.unf.GetRhoIJtotal("CorrTotal")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getCorrMatrixDataMCStat(self):
        # calculate correlationmatrix for input+MCStat
        if self.didUnfolding:
            # ErrMat = self.getErrorMatrixDataMCstat().Clone()
            ErrMat = self.getErrorMatrixDataMCstat().Clone()
            nBins = ErrMat.GetNbinsX()
            cov = np.matrix(np.zeros(shape=(nBins,nBins)))
            covdiag = np.matrix(np.zeros(shape=(nBins,nBins)))
            corr = np.matrix(np.zeros(shape=(nBins,nBins)))

            for i in range(nBins):
                for j in range(nBins):
                    if j != i:
                        cov[i,j] = ErrMat.GetBinContent(i + 1 , j + 1)
                        cov[j,i] = ErrMat.GetBinContent(i + 1 , j + 1)
                        covdiag[i,j] = 0
                        covdiag[j,i] = 0
                    else:
                        cov[i,i] = ErrMat.GetBinContent(i + 1  , j + 1)
                        covdiag[i,i] = 1. / np.sqrt(cov[i,i])
            # print "covdiag: \n", covdiag
            # print "cov: \n", cov

            corr = covdiag * cov * covdiag;
            print "#" * 50
            print "Correlation matric originating from Data and MC statistical fluctuations: "
            print corr
            print "#" * 50
            print "CAREFUL SOMETHING SEEMD BUGGED->Problem for future Wieland"
            print "#" * 50

            h_corrDataMCstat = r.TH2D()

            for i in range(nBins):
                for j in range(nBins):
                    h_corrDataMCstat.SetBinContent(i,j,corr[i,j])

            h_corrDataMCstat.SetName("corrMatrixDataMCstat")
            h_corrDataMCstat.Write()
            return h_corrDataMCstat
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getGlobalCorrInputStatBkg(self):
        if self.didUnfolding:
            h = self.unf.GetRhoIstatbgr("CorrInputStatBkg")
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getShiftFromSubtractedBackground(self, label):
        if self.didUnfolding:
            h = self.unf.GetDeltaSysBackgroundScale(label, "shift_"+label)
            h.Write()
            return h
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getShiftFromSystematic(self, sysname, absolute = False):
        if self.didUnfolding:
            if sysname not in self.getListofSysts():
                raise RuntimeError("Sytematic", sysname, "not found")
            Delta = self.unf.GetDeltaSysSource(sysname, "unfolded_shiftAbsolute_" + sysname)
            if absolute:
                Delta.Write()
                return Delta
            else:
                NomPlusVar = self.getResult().Clone()
                NomPlusVar.Add(Delta)
                NomPlusVar.SetName("shift_" + sysname)
                NomPlusVar.Write()
                return NomPlusVar
        else:
            raise RuntimeError("Do Unfolding before accessing results")

    def getListofSysts(self):
        systs = []
        for sys in self.unf.GetSysSources():
            systs.append(sys)
        return systs


if __name__ == "__main__":
    r.gROOT.SetBatch(True)
    file = r.TFile("rootfiles/histos.root")
    mat = file.Get("z_nunu_jets_A")
    mat.Print()
    inp = file.Get("z_nunu_jets_Hadr_Recoil_Pt")
    inp.Print()
    sys = file.Get("z_nunu_jets_A_Weight_PDFUp")
    sys.Print()
    # mat = r.TH2D("test", "test", 20, 0, 1, 20, 0, 1)
    # inp = r.TH1D("test2", "test2", 10, 0, 1)
    unf = Unfolder(mat=mat, data=inp, label="test")
    unf.addSystematic(sys, "PDFUp")
    unf.doUnfolding(tau = 0.2)
    # unf.getProbMatrix()
    h = unf.getShiftFromSystematic(sysname = "PDFUp", absolute=True)
    h.Print()
    # h.Draw()
    # raw_input()
    # unf.getProbMatrix()
    # h = unf.getResult()
    # h.Print()
    # h.Draw()
    # unf.getRegMatrix()
    # h = unf.getNormVector()
    # h.Print()
    unf.findBestTau(mode="glCorr")
    # unf.fillMissesinUF(inp)
