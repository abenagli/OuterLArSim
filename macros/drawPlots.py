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
ROOT.gStyle.SetTitleOffset(1,'Y')
ROOT.gStyle.SetLabelSize(0.045,'XYZ')
ROOT.gErrorIgnoreLevel = ROOT.kWarning
ROOT.gROOT.SetBatch(True)
#ROOT.gROOT.SetBatch(False)



inputFiles = []
inputFiles.append( ('../out_new_slats_supportPanel_vikuiti_WLSConc5_detN12.root','slats',0) )
inputFiles.append( ('../out_new_tiles_supportPanel_vikuiti_WLSConc5_detN12.root','tiles',0) )

colors = {}
colors['slats'] = ROOT.kRed
colors['tiles'] = ROOT.kBlue

rootfiles = {}
trees = {}
profs_x = {}
profs_y = {}
for inputFile in inputFiles:
    label = inputFile[1]
    rootfiles[label] = ROOT.TFile.Open(inputFile[0],'READ')
    trees[label] = rootfiles[label].Get('tree')
    profs_x[label] = ROOT.TProfile('p_x_%s'%label,'',25,0.,500.)
    profs_y[label] = ROOT.TProfile('p_y_%s'%label,'',25,0.,1500.)
    print(label,profs_x[label])
    print(label,profs_y[label])
    trees[label].Draw('n_phot_det/n_phot_sci_LAr*100:inputInitialPosition[0]>>p_x_%s'%label,'','goff')
    trees[label].Draw('n_phot_det/n_phot_sci_LAr*100:inputInitialPosition[1]>>p_y_%s'%label,'','goff')    
    print(profs_x[label].GetEntries())
    print(profs_y[label].GetEntries())

c = ROOT.TCanvas('c','c',2100,900)
c.Divide(2,1)
c.cd(1)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(0.,0.,500.,0.4)
hPad1.SetTitle(";source position x [mm];efficiency (vs. LAr) [%]")
hPad1.Draw()
for label in ['slats','tiles']:
    rootfiles[label].cd()
    profs_x[label].SetMarkerColor(colors[label])
    profs_x[label].SetLineColor(colors[label])
    profs_x[label].Draw('same')
    profs_x[label].Draw('hist,same')
c.cd(2)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad2 = ROOT.gPad.DrawFrame(0.,0.,1500.,0.4)
hPad2.SetTitle(";source position y [mm];efficiency (vs. LAr) [%]")
for label in ['slats','tiles']:
    rootfiles[label].cd()
    profs_y[label].SetMarkerColor(colors[label])
    profs_y[label].SetLineColor(colors[label])
    profs_y[label].Draw('same')
    profs_y[label].Draw('hist,same')    
leg = ROOT.TLegend(0.25, 0.94-2*0.07, 0.55, 0.94)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.05)
for label in ['slats','tiles']:
    leg.AddEntry(profs_x[label],label,'PL')
c.cd(1)
leg.Draw('same')
c.cd(2)
leg.Draw('same')
c.Print('c_comparison_eff_LAr.png')



for inputFile in inputFiles:
    label = inputFile[1]
    rootfiles[label] = ROOT.TFile.Open(inputFile[0],'READ')
    trees[label] = rootfiles[label].Get('tree')
    profs_x[label] = ROOT.TProfile('p_x_%s'%label,'',50,0.,500.)
    profs_y[label] = ROOT.TProfile('p_y_%s'%label,'',50,0.,1500.)
    print(label,profs_x[label])
    print(label,profs_y[label])
    trees[label].Draw('n_phot_det/n_phot_wls_primary*100/2.:inputInitialPosition[0]>>p_x_%s'%label,'','goff')
    trees[label].Draw('n_phot_det/n_phot_wls_primary*100/2.:inputInitialPosition[1]>>p_y_%s'%label,'','goff')    
    print(profs_x[label].GetEntries())
    print(profs_y[label].GetEntries())

c = ROOT.TCanvas('c','c',2100,900)
c.Divide(2,1)
c.cd(1)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(0.,0.4,500.,1.2)
hPad1.SetTitle(";source position x [mm];efficiency (N_{#gamma}^{SiPM} / N_{#gamma}^{pTP}) [%]")
hPad1.Draw()
for label in ['slats','tiles']:
    rootfiles[label].cd()
    profs_x[label].SetMarkerColor(colors[label])
    profs_x[label].SetLineColor(colors[label])
    profs_x[label].Draw('same')
    profs_x[label].Draw('hist,same')
c.cd(2)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad2 = ROOT.gPad.DrawFrame(0.,0.4,1500.,1.2)
hPad2.SetTitle(";source position y [mm];efficiency (N_{#gamma}^{SiPM} / N_{#gamma}^{pTP}) [%]")
for label in ['slats','tiles']:
    rootfiles[label].cd()
    profs_y[label].SetMarkerColor(colors[label])
    profs_y[label].SetLineColor(colors[label])
    profs_y[label].Draw('same')
    profs_y[label].Draw('hist,same')
leg = ROOT.TLegend(0.25, 0.94-2*0.07, 0.55, 0.94)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.05)
for label in ['slats','tiles']:
    leg.AddEntry(profs_x[label],label+(' 10mm'),'PL')
c.cd(1)
leg.Draw('same')
c.cd(2)
leg.Draw('same')
c.Print('c_comparison_eff_primary.png')




WLSConcs = [0,5,10,15,19]
inputFiles = {}
trees = {}

g_eff_LAr = ROOT.TGraph()
g_eff_primary = ROOT.TGraph()
g_relEff_LAr = ROOT.TGraph()
g_relEff_primary = ROOT.TGraph()
g_eff_LAr.SetMarkerColor(ROOT.kRed)
g_eff_LAr.SetLineColor(ROOT.kRed)
g_relEff_LAr.SetMarkerColor(ROOT.kRed)
g_relEff_LAr.SetLineColor(ROOT.kRed)
g_eff_primary.SetMarkerColor(ROOT.kBlue)
g_eff_primary.SetLineColor(ROOT.kBlue)
g_relEff_primary.SetMarkerColor(ROOT.kBlue)
g_relEff_primary.SetLineColor(ROOT.kBlue)

g_n_phot_abs_lightGuide = ROOT.TGraph()
g_n_phot_wls_lightGuide = ROOT.TGraph()
g_n_phot_selfabs_lightGuide = ROOT.TGraph()
g_n_phot_selfwls_lightGuide = ROOT.TGraph()

for WLSConc in WLSConcs:
    
    inputFileName = '../out_new_slats_supportPanel_vikuiti_WLSConc%d.root'%WLSConc
    inputFiles[WLSConc] = ROOT.TFile(inputFileName,'READ') 
    trees[WLSConc] = inputFiles[WLSConc].Get('tree')
    tree = trees[WLSConc]
    
    eff_LAr = 0.
    eff_primary = 0.
    n_phot_abs_lightGuide = 0
    n_phot_wls_lightGuide = 0
    n_phot_selfabs_lightGuide = 0
    n_phot_selfwls_lightGuide = 0
    N = tree.GetEntries()
    for i in range(N):
        tree.GetEntry(i)
        eff_LAr += tree.n_phot_det/tree.n_phot_sci_LAr
        eff_primary += tree.n_phot_det/tree.n_phot_wls_primary
        n_phot_abs_lightGuide += tree.n_phot_abs_lightGuide
        n_phot_wls_lightGuide += tree.n_phot_wls_lightGuide
        n_phot_selfabs_lightGuide += tree.n_phot_selfabs_lightGuide
        n_phot_selfwls_lightGuide += tree.n_phot_selfwls_lightGuide
    eff_LAr /= N
    eff_LAr *= 100.
    eff_primary /= N
    eff_primary *= 100.
    n_phot_abs_lightGuide /= N
    n_phot_wls_lightGuide /= N
    n_phot_selfabs_lightGuide /= N
    n_phot_selfwls_lightGuide /= N
    print(n_phot_selfwls_lightGuide)
    g_eff_LAr.SetPoint(g_eff_LAr.GetN(),WLSConc,eff_LAr)
    g_eff_primary.SetPoint(g_eff_primary.GetN(),WLSConc,eff_primary)
    g_n_phot_abs_lightGuide.SetPoint(g_n_phot_abs_lightGuide.GetN(),WLSConc,n_phot_abs_lightGuide)
    g_n_phot_wls_lightGuide.SetPoint(g_n_phot_wls_lightGuide.GetN(),WLSConc,n_phot_wls_lightGuide)
    g_n_phot_selfabs_lightGuide.SetPoint(g_n_phot_selfabs_lightGuide.GetN(),WLSConc,n_phot_selfabs_lightGuide)
    g_n_phot_selfwls_lightGuide.SetPoint(g_n_phot_selfwls_lightGuide.GetN(),WLSConc,n_phot_selfwls_lightGuide)
    for point in range(g_eff_LAr.GetN()):
        g_relEff_LAr.SetPoint(point,g_eff_LAr.GetPointX(point),g_eff_LAr.GetPointY(point)/g_eff_LAr.GetPointY(0))
        g_relEff_primary.SetPoint(point,g_eff_primary.GetPointX(point),g_eff_primary.GetPointY(point)/g_eff_primary.GetPointY(0))
    
c = ROOT.TCanvas('c','c',2100,900)
c.Divide(2,1)
c.cd(1)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(-1.,0.,20.,3.)
hPad1.SetTitle(";WLS concentration;efficiency [%]")
hPad1.Draw()
g_eff_LAr.Draw('PL,same')
g_eff_primary.Draw('PL,same')
c.cd(2)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad2 = ROOT.gPad.DrawFrame(-1.,0.7,20.,1.1)
hPad2.SetTitle(";WLS concentration;relative efficiency")
hPad2.Draw()
g_relEff_LAr.Draw('PL,same')
g_relEff_primary.Draw('PL,same')
c.Print('c_eff.png')

c2 = ROOT.TCanvas('c2','c2',1050,900)
ROOT.gPad.SetGridx()
ROOT.gPad.SetGridy()
hPad1 = ROOT.gPad.DrawFrame(-1.,0.,20.,3000.)
hPad1.SetTitle(";WLS concentration;N_{op. photon}")
hPad1.Draw()
g_n_phot_abs_lightGuide.SetLineColor(ROOT.kRed)
g_n_phot_abs_lightGuide.SetMarkerColor(ROOT.kRed)
g_n_phot_wls_lightGuide.SetLineColor(ROOT.kBlue)
g_n_phot_wls_lightGuide.SetMarkerColor(ROOT.kBlue)
g_n_phot_selfabs_lightGuide.SetLineColor(ROOT.kGreen+1)
g_n_phot_selfabs_lightGuide.SetMarkerColor(ROOT.kGreen+1)
g_n_phot_selfwls_lightGuide.SetLineColor(ROOT.kOrange+1)
g_n_phot_selfwls_lightGuide.SetMarkerColor(ROOT.kOrange+1)
g_n_phot_abs_lightGuide.Draw('PL,same')
g_n_phot_wls_lightGuide.Draw('PL,same')
g_n_phot_selfabs_lightGuide.Draw('PL,same')
g_n_phot_selfwls_lightGuide.Draw('PL,same')
leg = ROOT.TLegend(0.45, 0.94-4*0.07, 0.90, 0.94)
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.05)
leg.AddEntry(g_n_phot_abs_lightGuide,'op. abs.','PL')
leg.AddEntry(g_n_phot_wls_lightGuide,'op. wls','PL')
leg.AddEntry(g_n_phot_selfabs_lightGuide,'self op. abs.','PL')
leg.AddEntry(g_n_phot_selfwls_lightGuide,'self wls abs.','PL')
leg.Draw('same')
c2.Print('c_n_phot_lightGuide.png')
