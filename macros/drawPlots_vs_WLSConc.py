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

inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc0.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',0) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc1.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',1) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc2.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',2) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc3.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',3) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc4.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',4) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc5.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',5) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc6.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',6) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc7.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',7) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc8.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',8) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc9.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',9) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc10.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',10) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc11.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',11) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc12.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',12) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc13.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',13) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc14.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',14) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc15.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',15) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc16.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',16) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc17.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',17) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc18.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',18) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_shortLR_detN16_vs_WLSConc/out_vs_WLSConc19.root','slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR',19) )

inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc0.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',0) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc1.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',1) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc2.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',2) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc3.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',3) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc4.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',4) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc5.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',5) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc6.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',6) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc7.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',7) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc8.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',8) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc9.root', 'slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',9) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc10.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',10) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc11.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',11) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc12.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',12) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc13.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',13) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc14.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',14) )
inputFiles.append( ('/home/cmsdaq/LEGEND/OuterLArSim/scripts/jobs/slats_longUD_detN16_vs_WLSConc/out_vs_WLSConc15.root','slats 500#times100#times10 mm^{3}, 16 SiPMs longUD',15) )


colors = {}
markers = {}
areas = {}
colors['slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR'] = ROOT.kOrange+1
markers['slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR'] = 1
areas['slats 500#times100#times10 mm^{3}, 16 SiPMs shortLR'] = 6.*6. / (5000.*100.) * 100.
colors['slats 500#times100#times10 mm^{3}, 16 SiPMs longUD'] = ROOT.kRed+1
markers['slats 500#times100#times10 mm^{3}, 16 SiPMs longUD'] = 1
areas['slats 500#times100#times10 mm^{3}, 16 SiPMs longUD'] = 6.*6. / (5000.*100.) * 100.

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

g_eff_vs_WLSConc = {}
for label in labels:
    g_eff_vs_WLSConc[label] = ROOT.TGraphErrors()
    for N in range(20):
        if( (label,N) in rootfiles.keys() ):
            rootfiles[(label,N)].cd()
            g_eff_vs_WLSConc[label].SetPoint(g_eff_vs_WLSConc[label].GetN(),N,profs[(label,N)].GetMean(2))


c = ROOT.TCanvas('c','c',1050,900)
c.cd()
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(-0.5,0.,19.5,5.)
hPad1.SetTitle(";WLS concentration;efficiency (N_{#gamma}^{SiPM} / N_{#gamma}^{pTP}) [%]")
hPad1.Draw()
leg = ROOT.TLegend(0.18, 0.94-len(labels)*0.07, 0.55, 0.94)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.04)
for label in labels:
    g_eff_vs_WLSConc[label].Draw('PL,same')
    g_eff_vs_WLSConc[label].SetLineColor(colors[label])
    g_eff_vs_WLSConc[label].SetMarkerColor(colors[label])
    g_eff_vs_WLSConc[label].SetMarkerSize(markers[label])
    leg.AddEntry(g_eff_vs_WLSConc[label],label,'PL')
leg.Draw('same')
c.Print('c_eff_vs_WLSConc.png')
