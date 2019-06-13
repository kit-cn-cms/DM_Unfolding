import ROOT as r
import json

class HistoHelper():

    def __init__(self, filename):
        try:
            self.file = r.TFile(filename)
            # self.file.ls()
        except ValueError:
            print "Didn't find file ", filename

    def getHisto(self, name):
        h = self.file.Get(str(name))
        h.SetDirectory(0)
        if h == None:
            raise Exception("Didn't find Histogram " + name + " in file " + self.file.GetName())
        return h

    def getHistosForSample(self, sample = "z_nunu_jets", variable = "Hadr_Recoil_Pt", systs=[]):
        output = dict()
        output["nominal"] = self.getHisto(sample+"_"+variable)
        for sys in systs:
            output[sys+"Up"] = self.getHisto(sample+"_"+variable+"_"+sys+"Up")
            output[sys+"Down"] = self.getHisto(sample+"_"+variable+"_"+sys+"Down")
        return output

    def fillFakesinUF(self, mat, h_fakes):
        print "filling ", h_fakes.GetName(), " in Underflow of ", mat.GetName()
        for xBin in range(mat.GetNbinsX() + 1):
            mat.SetBinContent(xBin, 0, 0) # make sure UF is empty in the beginning
            mat.SetBinContent(xBin, 0, h_fakes.GetBinContent(xBin))
            # print "setting recobin ", xBin," from ", mat.GetBinContent(xBin), " to ", h_fakes.GetBinContent(xBin)

    def fillMissesinUF(self, mat, h_misses):
        print "filling ", h_misses.GetName(), " in Underflow of ", mat.GetName()
        for yBin in range(mat.GetNbinsY() + 1):
            mat.SetBinContent(0, yBin, 0) # make sure UF is empty in the beginning
            mat.SetBinContent(0, yBin, h_misses.GetBinContent(yBin))
            # print "setting yBin ", yBin," from ", mat.GetBinContent(yBin), " to ", h_misses.GetBinContent(yBin)


if __name__ == "__main__":
    helper = HistoHelper("../rootfiles/histos.root")
    # helper.getHisto("h")
    dic = helper.getHistosForSample(variable = "A", systs=["CMS_res_j","CMS_scale_j"])