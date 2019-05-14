import ROOT

def drawHeader(title):
    txt=ROOT.TLatex()
    txt.SetNDC(True)
    txt.SetTextFont(42)
    txt.SetTextSize(0.04)
    txt.SetTextAlign(ROOT.kHAlignLeft+ROOT.kVAlignCenter)
    txt.DrawLatex(0.12,0.97,'#bf{CMS} #it{preliminary}')
    txt.SetTextAlign(ROOT.kHAlignCenter+ROOT.kVAlignCenter)
    txt.SetTextSize(0.035)
    txt.DrawLatex(0.5,0.97,title)

fIn=ROOT.TFile.Open('dosemap_output.root')

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gROOT.SetBatch(True)

c=ROOT.TCanvas('c','c',500,500)
c.SetLeftMargin(0.12)
c.SetTopMargin(0.05)
c.SetBottomMargin(0.11)
c.SetRightMargin(0.12)

plots=['sn','cce','noise','ileak','fluence']
for p in plots:
    for d in [8,9]:
        c.Clear()
        h=fIn.Get('plotter/d%d_%s'%(d,p))
        h.Draw('colz')
        h.GetZaxis().SetTitleOffset(-0.3)
        drawHeader(h.GetTitle())
        c.Modified()
        c.Update()
        c.SaveAs('%s_%s.png'%(p,d))
