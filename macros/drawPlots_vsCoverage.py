#! /usr/bin/python3

import os
import shutil
import glob
import math
import array
import sys
import time

import ROOT
import tdrstyle

#set the tdr style
tdrstyle.setTDRStyle()
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(0)
ROOT.gStyle.SetTitleOffset(1.,'Y')
ROOT.gStyle.SetLabelSize(0.045,'XYZ')
ROOT.gErrorIgnoreLevel = ROOT.kWarning
ROOT.gROOT.SetBatch(True)
#ROOT.gROOT.SetBatch(False)



inputFiles = []

inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN4.root','tiles 10mm',4) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN12.root','tiles 10mm',12) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN16.root','tiles 10mm',16) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN20.root','tiles 10mm',20) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN32.root','tiles 10mm',32) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN48.root','tiles 10mm',48) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN60.root','tiles 10mm',60) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN80.root','tiles 10mm',80) )

inputFiles.append( ('../out_new_slats_supportPanel_vikuiti_WLSConc5_detN4.root','slats 1000mm x 10mm',4) )
inputFiles.append( ('../out_new_slats_supportPanel_vikuiti_WLSConc5_detN12.root','slats 1000mm x 10mm',12) )
inputFiles.append( ('../out_new_slats_supportPanel_vikuiti_WLSConc5_detN16.root','slats 1000mm x 10mm',16) )
inputFiles.append( ('../out_new_slats_supportPanel_vikuiti_WLSConc5_detN20.root','slats 1000mm x 10mm',20) )

inputFiles.append( ('../out_new_tiles_6mm_supportPanel_vikuiti_WLSConc5_detN4.root','tiles 6mm',4) )
inputFiles.append( ('../out_new_tiles_6mm_supportPanel_vikuiti_WLSConc5_detN12.root','tiles 6mm',12) )
inputFiles.append( ('../out_new_tiles_6mm_supportPanel_vikuiti_WLSConc5_detN20.root','tiles 6mm',20) )
inputFiles.append( ('../out_new_tiles_6mm_supportPanel_vikuiti_WLSConc5_detN48.root','tiles 6mm',48) )
inputFiles.append( ('../out_new_tiles_6mm_supportPanel_vikuiti_WLSConc5_detN80.root','tiles 6mm',80) )

inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detN4.root','slats 500mm x 10mm',4) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detN12.root','slats 500mm x 10mm',12) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detN16.root','slats 500mm x 10mm',16) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detN20.root','slats 500mm x 10mm',20) )

inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detLongUD_detN4.root', 'slats 500mm x 10mm, long face',4) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detLongUD_detN12.root','slats 500mm x 10mm, long face',12) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detLongUD_detN16.root','slats 500mm x 10mm, long face',16) )
inputFiles.append( ('../out_new_slats500mm_supportPanel_vikuiti_WLSConc5_detLongUD_detN20.root','slats 500mm x 10mm, long face',20) )

inputFiles.append( ('../out_new_slats500mm90degVikuiti_supportPanel_vikuiti_WLSConc5_detLongUD_detN20.root','slats 500mm x 10mm 2, long face',20) )

colors = {}
colors['slats 1000mm x 10mm'] = ROOT.kRed
colors['tiles 10mm'] = ROOT.kBlue
colors['tiles 6mm'] = ROOT.kGreen+1
colors['slats 500mm x 10mm'] = ROOT.kOrange+1
colors['slats 500mm x 10mm, long face'] = ROOT.kTeal+1
colors['slats 500mm x 10mm 2, long face'] = ROOT.kMagenta

markers = {}
markers['slats 1000mm x 10mm'] = 1
markers['tiles 10mm'] = 1.5
markers['tiles 6mm'] = 1.5
markers['slats 500mm x 10mm'] = 1
markers['slats 500mm x 10mm, long face'] = 1
markers['slats 500mm x 10mm 2, long face'] = 1

areas = {}
areas['slats 1000mm x 10mm'] = 6.*6. / (1000.*100.) * 100.
areas['tiles 10mm'] = 6.*6. / (1000.*100.) * 100.
areas['tiles 6mm'] = 6.*6. / (1000.*100.) * 100.
areas['slats 500mm x 10mm'] = 6.*6. / (500.*100.) * 100.
areas['slats 500mm x 10mm, long face'] = 6.*6. / (500.*100.) * 100.
areas['slats 500mm x 10mm 2, long face'] = 6.*6. / (500.*100.) * 100.
#areas['slats 1000mm x 10mm'] = 6.*6. / (10.*100.) * 100.
#areas['tiles 10mm'] = 6.*6. / (1000.*100.) * 100.
#areas['tiles 6mm'] = 6.*6. / (1000.*100.) * 100.
#areas['slats 500mm x 10mm'] = 6.*6. / (10.*100.) * 100.

labels = colors.keys()

rootfiles = {}
trees = {}
profs = {}
for inputFile in inputFiles:
    label = inputFile[1]
    N = inputFile[2]
    rootfiles[(label,N)] = ROOT.TFile.Open(inputFile[0],'READ')
    trees[(label,N)] = rootfiles[(label,N)].Get('tree')
    profs[(label,N)] = ROOT.TProfile('p_x_%s_%d'%(label,N),'',50,0.,500.)
    trees[(label,N)].Draw('n_phot_det/n_phot_wls_primary*100/2.:inputInitialPosition[0]>>p_x_%s_%d'%(label,N),'','goff')
    print((label,N))

g_eff_vs_coverage = {}
for label in labels:
    area = areas[label]
    g_eff_vs_coverage[label] = ROOT.TGraphErrors()
    for N in [4,6,12,16,20,32,48,60,80]:
        if( (label,N) in rootfiles.keys() ):
            rootfiles[(label,N)].cd()
            g_eff_vs_coverage[label].SetPoint(g_eff_vs_coverage[label].GetN(),N*area,profs[(label,N)].GetMean(2))


c = ROOT.TCanvas('c','c',1050,900)
c.cd()
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(0.,0.,3.,5.)
hPad1.SetTitle(";coverage_{SiPM} [%];efficiency (N_{#gamma}^{SiPM} / N_{#gamma}^{pTP}) [%]")
hPad1.Draw()
leg = ROOT.TLegend(0.18, 0.94-4*0.07, 0.55, 0.94)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.04)
for label in labels:
    g_eff_vs_coverage[label].Draw('PL,same')
    g_eff_vs_coverage[label].SetLineColor(colors[label])
    g_eff_vs_coverage[label].SetMarkerColor(colors[label])
    g_eff_vs_coverage[label].SetMarkerSize(markers[label])
    leg.AddEntry(g_eff_vs_coverage[label],label,'PL')
leg.Draw('same')
c.Print('c_eff_vs_coverage.png')
