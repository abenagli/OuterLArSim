//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes, nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: DetectorConstruction.cc, v 1.18 2010-10-23 19:27:38 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

#include "DetectorConstruction.hh"
#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4PropagatorInField.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"

#include "G4UserLimits.hh"


#include "DetectorConstruction.hh"
#include <G4TransportationManager.hh>
#include <G4MagneticField.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include "CreateTree.hh"
#include <algorithm>
#include <string>
#include <sstream>

using namespace CLHEP;



DetectorConstruction::DetectorConstruction (const string& configFileName)
{
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------
  
  ConfigFile config (configFileName) ;
  
  config.readInto(checkOverlaps,"checkOverlaps");
  
  config.readInto(world_x, "world_x");
  config.readInto(world_y, "world_y");
  config.readInto(world_z, "world_z");
  config.readInto(world_material, "world_material");
  
  config.readInto(container_material, "container_material");
  config.readInto(scint_yield, "scint_yield");

  config.readInto(panel_x, "panel_x");
  config.readInto(panel_y, "panel_y");
  config.readInto(panel_z, "panel_z");
  config.readInto(panel_material, "panel_material");
  config.readInto(lightGuide_N, "lightGuide_N");
  config.readInto(lightGuide_x, "lightGuide_x");
  config.readInto(lightGuide_y, "lightGuide_y");
  config.readInto(lightGuide_z, "lightGuide_z");
  config.readInto(lightGuide_material, "lightGuide_material");
  config.readInto(supportPanel, "supportPanel");
  config.readInto(supportPanel_x, "supportPanel_x");
  config.readInto(supportPanel_y, "supportPanel_y");
  config.readInto(supportPanel_z, "supportPanel_z");
  config.readInto(supportPanel_distance, "supportPanel_distance");
  
  config.readInto(primary_material, "primary_material");
  config.readInto(primary_thickness, "primary_thickness");

  config.readInto(gap_x, "gap_x");
  config.readInto(gap_y, "gap_y");
  config.readInto(gap_z, "gap_z");
  config.readInto(gap_material,"gap_material");
  config.readInto(gap_rIndex,"gap_rIndex");
  
  config.readInto(detector, "detector");
  config.readInto(det_conf, "det_conf");
  config.readInto(det_x, "det_x");
  config.readInto(det_y, "det_y");
  config.readInto(det_z, "det_z");
  config.readInto(det_material,"det_material");
  config.readInto(det_efficiency,"det_efficiency");
  config.readInto(det_reflectivity,"det_reflectivity");
  config.readInto(window_x, "window_x");
  config.readInto(window_y, "window_y");
  config.readInto(window_z, "window_z");
  config.readInto(window_material,"window_material");

  initializeMaterials();
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 



DetectorConstruction::~DetectorConstruction()
{
  delete stepLimit;
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 



G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4cout << ">>>>>> DetectorConstruction::Construct()::begin <<<<<<" << G4endl ;
  
  
  //------------------------------------
  //------------- Geometry -------------
  //------------------------------------
  
  
  // The experimental Hall
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * worldS = new G4Box ("worldS", 0.5 * world_x, 0.5 * world_y, 0.5 * world_z) ;
  G4LogicalVolume * worldLV = new G4LogicalVolume (worldS, WoMaterial, "worldLV", 0, 0, 0) ;
  G4VPhysicalVolume * worldPV = new G4PVPlacement (0, G4ThreeVector(), worldLV, "worldPV", 0, false, 0, checkOverlaps) ;
  
  
  // The container
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4RotationMatrix* rotX90m = new G4RotationMatrix;
  rotX90m -> rotateX(-90.*deg);
  G4RotationMatrix* rotY90m = new G4RotationMatrix;
  rotY90m -> rotateY(-90.*deg);
  G4RotationMatrix* rotY90p = new G4RotationMatrix;
  rotY90p -> rotateY(+90.*deg);
  
  G4double container_x = world_x;
  G4double container_y = world_y;
  G4double container_z = world_z;
  
  G4VSolid * containerS = new G4Box ("containerS", 0.5*container_x, 0.5*container_y, 0.5*container_z) ;
  G4LogicalVolume * containerLV = new G4LogicalVolume (containerS, CoMaterial, "containerLV", 0, 0, 0) ;
  G4VPhysicalVolume * containerPV = new G4PVPlacement (rotX90m, G4ThreeVector(), containerLV, "containerPV", worldLV, false, 0, checkOverlaps) ;
  
  
  
  // the PMMA panel
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid* panelS = new G4Box("panelS", 0.5*panel_x, 0.5*panel_y, 0.5*panel_z);
  G4LogicalVolume* panelLV = new G4LogicalVolume(panelS, PaMaterial, "panelLV");
  G4VPhysicalVolume* panelPV = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), panelLV, "panelPV", containerLV, false, 0, checkOverlaps);
  

  // the light guide
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid* lightGuideS = new G4Box("lightGuideS", 0.5*lightGuide_x, 0.5*lightGuide_y, 0.5*lightGuide_z);
  G4LogicalVolume* lightGuideLV = new G4LogicalVolume(lightGuideS, LgMaterial, "lightGuideLV");

  G4VSolid* supportPanelS = NULL;
  G4LogicalVolume* supportPanelLV = NULL;
  G4VSolid* primaryS = NULL;
  G4LogicalVolume* primaryLV = NULL;
  if( supportPanel )
  {
    supportPanelS = new G4Box("supportPanelS", 0.5*supportPanel_x, 0.5*supportPanel_y, 0.5*supportPanel_z);
    supportPanelLV = new G4LogicalVolume(supportPanelS, PaMaterial, "supportPanelLV");
    
    primaryS = new G4Box("primaryS", 0.5*supportPanel_x, 0.5*primary_thickness, 0.5*supportPanel_z);
    primaryLV = new G4LogicalVolume(primaryS, PrMaterial, "primaryLV");
  }
  else
  {
    primaryS = new G4Box("primaryS", 0.5*lightGuide_x, 0.5*primary_thickness, 0.5*lightGuide_z);
    primaryLV = new G4LogicalVolume(primaryS, PrMaterial, "primaryLV");
  }


  G4double vikuitiThickness = 0.150*mm;
  G4VSolid* vikuitiWrappingBackS = new G4Box("vikuitiWrappingBackS", 0.5*lightGuide_x, 0.5*vikuitiThickness, 0.5*lightGuide_z);
  G4VSolid* vikuitiWrappingSideS = new G4Box("vikuitiWrappingSideS", 0.5*lightGuide_x, 0.5*lightGuide_y, 0.5*vikuitiThickness);
  G4LogicalVolume* vikuitiWrappingBackLV = new G4LogicalVolume(vikuitiWrappingBackS, VikuitiMaterial, "vikuitiWrappingBackLV");
  G4LogicalVolume* vikuitiWrappingSideLV = new G4LogicalVolume(vikuitiWrappingSideS, VikuitiMaterial, "vikuitiWrappingSideLV");

  G4VSolid* detS = new G4Box("detS", 0.5*det_x, 0.5*det_y, 0.5*det_z);
  G4LogicalVolume* detLV = new G4LogicalVolume(detS, DeMaterial, "detLV");

  G4VSolid* windowS = new G4Box("windowS", 0.5*window_x, 0.5*window_y, 0.5*window_z);
  G4LogicalVolume* windowLV = new G4LogicalVolume(windowS, WiMaterial, "windowLV");

  for(int jj = 0; jj < lightGuide_N; ++jj)
  {
    G4double spacing = panel_z / (lightGuide_N+1);
    G4double delta_z = 0.5*panel_z - spacing*(jj+1); 
    
    G4VPhysicalVolume* lightGuidePV = new G4PVPlacement(0, G4ThreeVector(0.,+0.5*(panel_y+lightGuide_y)+vikuitiThickness,delta_z), lightGuideLV, Form("lightGuide%02dPV",jj), containerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* vikuitiWrappingBackPV = new G4PVPlacement(0, G4ThreeVector(0.,+0.5*(panel_y+vikuitiThickness),delta_z), vikuitiWrappingBackLV, Form("vikuitiWrappingBack%02dPV",jj), containerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* vikuitiWrappingSide1PV = new G4PVPlacement(0, G4ThreeVector(0.,+0.5*(panel_y+lightGuide_y)+vikuitiThickness,delta_z-0.5*(lightGuide_z+vikuitiThickness)), vikuitiWrappingSideLV, Form("vikuitiWrappingSide1%02dPV",jj), containerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* vikuitiWrappingSide2PV = new G4PVPlacement(0, G4ThreeVector(0.,+0.5*(panel_y+lightGuide_y)+vikuitiThickness,delta_z+0.5*(lightGuide_z+vikuitiThickness)), vikuitiWrappingSideLV, Form("vikuitiWrappingSide2%02dPV",jj), containerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* vikuitiWrappingBackSurface = new G4LogicalBorderSurface("vikuityWrappingBackSurface", lightGuidePV, vikuitiWrappingBackPV, VikuitiSurface);
    G4LogicalBorderSurface* vikuitiWrappingSide1Surface = new G4LogicalBorderSurface("vikuityWrappingSide1Surface", lightGuidePV, vikuitiWrappingSide1PV, VikuitiSurface);
    G4LogicalBorderSurface* vikuitiWrappingSide2Surface = new G4LogicalBorderSurface("vikuityWrappingSide2Surface", lightGuidePV, vikuitiWrappingSide2PV, VikuitiSurface);

    if( supportPanel )
    {
      G4double supportPanel_yCenter = +0.5*(panel_y+supportPanel_y)+vikuitiThickness+lightGuide_y+supportPanel_distance;
      G4VPhysicalVolume* supportPanelPV = new G4PVPlacement(0, G4ThreeVector(0.,supportPanel_yCenter,delta_z), supportPanelLV, Form("supportPanel%02dPV",jj), containerLV, false, 0, checkOverlaps);
      G4VPhysicalVolume* primaryPV = new G4PVPlacement(0, G4ThreeVector(0.,supportPanel_yCenter+0.5*(supportPanel_y+primary_thickness),delta_z), primaryLV, Form("primary%02dPV",jj), containerLV, false, 0, checkOverlaps);
    }
    else
    {
      G4double primary_yCenter = +0.5*panel_y+vikuitiThickness+lightGuide_y;
      G4VPhysicalVolume* primaryPV = new G4PVPlacement(0, G4ThreeVector(0.,primary_yCenter+0.5*primary_thickness,delta_z), primaryLV, Form("primary%02dPV",jj), containerLV, false, 0, checkOverlaps);
    }

    for(int zz = 0; zz < 8; ++zz)
    {
      G4double spacing2 = lightGuide_z/9.;
      G4double delta_z2 = 0.5*lightGuide_z - spacing2*(zz+1);

      G4VPhysicalVolume* windowRPV = new G4PVPlacement(rotY90m, G4ThreeVector(-0.5*(lightGuide_x+window_z), 0.5*(panel_y+lightGuide_y)+vikuitiThickness, delta_z+delta_z2), windowLV, Form("windowR%02d%02dPV",jj,zz), containerLV, false, 0, checkOverlaps);
      G4VPhysicalVolume* detRPV = new G4PVPlacement(rotY90m, G4ThreeVector(-0.5*(lightGuide_x+det_z)-window_z, 0.5*(panel_y+lightGuide_y)+vikuitiThickness, delta_z+delta_z2), detLV, Form("detR%02d%02dPV",jj,zz), containerLV, false, 0, checkOverlaps);

      G4VPhysicalVolume* windowLPV = new G4PVPlacement(rotY90p, G4ThreeVector(+0.5*(lightGuide_x+window_z), 0.5*(panel_y+lightGuide_y)+vikuitiThickness, delta_z+delta_z2), windowLV, Form("windowL%02d%02dPV",jj,zz), containerLV, false, 0, checkOverlaps);
      G4VPhysicalVolume* detLPV = new G4PVPlacement(rotY90p, G4ThreeVector(+0.5*(lightGuide_x+det_z)+window_z, 0.5*(panel_y+lightGuide_y)+vikuitiThickness, delta_z+delta_z2), detLV, Form("detLPV%02d%02d",jj,zz), containerLV, false, 0, checkOverlaps);
      
      G4LogicalBorderSurface* SiPMWindowRSurface = new G4LogicalBorderSurface(Form("SiPMWindowSurface%02d%02dR",jj,zz), windowRPV, detRPV, SiPMSurface);
      G4LogicalBorderSurface* SiPMWindowLSurface = new G4LogicalBorderSurface(Form("SiPMWindowSurfaceL%02d%02d",jj,zz), windowLPV, detLPV, SiPMSurface);
    }
  }
  
  
  
  //-----------------------------------------------------
  //------------- Visualization attributes --------------
  //-----------------------------------------------------

  G4Colour white  (1.00, 1.00, 1.00);  // white
  G4Colour gray   (0.50, 0.50, 0.50);  // gray
  G4Colour black  (0.00, 0.00, 0.00);  // black
  G4Colour red    (1.00, 0.00, 0.00);  // red
  G4Colour green  (0.00, 1.00, 0.00);  // green
  G4Colour blue   (0.00, 0.00, 1.00);  // blue
  G4Colour cyan   (0.00, 1.00, 1.00);  // cyan
  G4Colour air    (0.90, 0.94, 1.00);  // cyan
  G4Colour magenta(1.00, 0.00, 1.00);  // magenta 
  G4Colour yellow (1.00, 1.00, 0.00);  // yellow
  G4Colour brass  (0.80, 0.60, 0.40);  // brass
  G4Colour brown  (0.70, 0.40, 0.10);  // brown
  
  G4VisAttributes* VisAttWorld = new G4VisAttributes(black);
  VisAttWorld -> SetVisibility(true) ;
  VisAttWorld -> SetForceWireframe(true) ;
  worldLV -> SetVisAttributes(VisAttWorld) ;
  
  G4VisAttributes* VisAttContainer = new G4VisAttributes(yellow);
  VisAttContainer -> SetVisibility(true) ;
  VisAttContainer -> SetForceWireframe(true) ;
  containerLV -> SetVisAttributes(VisAttContainer) ;
  
  G4VisAttributes* VisAttPanel = new G4VisAttributes(cyan);
  VisAttPanel -> SetVisibility(true);
  VisAttPanel -> SetForceWireframe(true);
  panelLV -> SetVisAttributes(VisAttPanel);
  
  G4VisAttributes* VisAttLightGuide = new G4VisAttributes(red);
  VisAttLightGuide -> SetVisibility(true);
  VisAttLightGuide -> SetForceWireframe(true);
  panelLV -> SetVisAttributes(VisAttLightGuide);
  
  G4VisAttributes* VisAttVikuitiWrapping = new G4VisAttributes(yellow);
  VisAttVikuitiWrapping -> SetVisibility(true);
  VisAttVikuitiWrapping -> SetForceWireframe(false);
  vikuitiWrappingBackLV -> SetVisAttributes(VisAttVikuitiWrapping);
  vikuitiWrappingSideLV -> SetVisAttributes(VisAttVikuitiWrapping);

  G4VisAttributes* VisAttPrimary = new G4VisAttributes(green);
  VisAttPrimary -> SetVisibility(true);
  VisAttPrimary -> SetForceWireframe(false);
  primaryLV -> SetVisAttributes(VisAttPrimary);

  //G4VisAttributes* VisAttGap = new G4VisAttributes(blue);
  //VisAttGap -> SetVisibility(true);
  //VisAttGap -> SetForceWireframe(false);
  //gapLV -> SetVisAttributes(VisAttGap);
  
  G4VisAttributes* VisAttDet = new G4VisAttributes(gray);
  VisAttDet -> SetVisibility(true);
  VisAttDet -> SetForceWireframe(true);
  detLV -> SetVisAttributes(VisAttDet);
  
  G4VisAttributes* VisAttWindow = new G4VisAttributes(blue);
  VisAttWindow -> SetVisibility(true);
  VisAttWindow -> SetForceWireframe(true);
  windowLV -> SetVisAttributes(VisAttWindow);
  
  G4cout << ">>>>>> DetectorConstruction::Construct()::end <<< " << G4endl;
  return worldPV;
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 



void DetectorConstruction::initializeMaterials()
{
  //-----------------
  // define materials
  G4NistManager* nist = G4NistManager::Instance();
  
  WoMaterial = NULL ;
  if( world_material == 1 ) WoMaterial = nist->FindOrBuildMaterial("G4_AIR");
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid world material specifier " << world_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "World material: "<< WoMaterial << G4endl ;
  
  
  CoMaterial = NULL ;
  if( container_material == 1 ) CoMaterial = MyMaterials::LAr(scint_yield);
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid container material specifier " << container_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Container material: "<< CoMaterial << G4endl ;


  PaMaterial = NULL;
  if      ( panel_material == 1 )  PaMaterial = MyMaterials::PMMA(-1);
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid panel material specifier " << panel_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Panel material: "<< PaMaterial << G4endl ;
  
  
  LgMaterial = NULL;
  if      ( lightGuide_material == 1 )  LgMaterial = MyMaterials::PMMA(0);
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid lightguide material specifier " << lightGuide_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Light guide material: "<< LgMaterial << G4endl ;
  
  VikuitiMaterial = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  VikuitiSurface = MyMaterials::VikuitiSurface();

  
  PrMaterial = NULL;
  if      ( primary_material == 1 )  PrMaterial = MyMaterials::pTP();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid primary material specifier " << primary_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Primary material: "<< PrMaterial << G4endl ;


  DeMaterial = NULL;
  if( det_material == 1 ) DeMaterial = nist->FindOrBuildMaterial("G4_Si");
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid detector material specifier " << det_material << G4endl;
    exit(-1);
  }
  G4cout << "Detector material: " << DeMaterial << G4endl;

  WiMaterial = NULL;
  if( window_material == 1 ) WiMaterial = MyMaterials::Epoxy();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid window material specifier " << window_material << G4endl;
    exit(-1);
  }
  G4cout << "Window material: " << WiMaterial << G4endl;

  SiPMSurface = MyMaterials::SiPMSurface();

  /*
  GaMaterial = NULL;
  if     ( gap_material == 1 ) GaMaterial = nist->FindOrBuildMaterial("G4_AIR");
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid gap material specifier " << gap_material << G4endl;
    exit(-1);
  }
  G4cout << "Gap material: " << gap_material << G4endl;
  
  */

  
  /*
  //------------------
  // change properties
  
  if( crystal_absLength > 0 )
  {
    const G4int nEntries_ABS = 2;
    G4double PhotonEnergy_ABS[nEntries_ABS] = { 1.*eV, 10.*eV };
    G4double Absorption[nEntries_ABS] = { crystal_absLength*mm, crystal_absLength*mm };
    
    CrMaterial -> GetMaterialPropertiesTable() -> RemoveProperty("ABSLENGTH");
    CrMaterial -> GetMaterialPropertiesTable() -> AddProperty("ABSLENGTH",PhotonEnergy_ABS,Absorption,nEntries_ABS);
  }
  if( crystal_rIndex > 0 )
  {
    const G4int nEntries_RI = 2;
    G4double PhotonEnergy_RI[nEntries_RI] = { 1.*eV, 10.*eV };
    G4double RefractiveIndex[nEntries_RI] = { crystal_rIndex, crystal_rIndex };
    
    CrMaterial -> GetMaterialPropertiesTable() -> RemoveProperty("RINDEX");
    CrMaterial -> GetMaterialPropertiesTable() -> AddProperty("RINDEX",PhotonEnergy_RI,RefractiveIndex,nEntries_RI);
  }
  
   if( gap_absLength > 0 )
   {
     const G4int nEntries_ABS = 2;
     G4double PhotonEnergy_ABS[nEntries_ABS] = { 1.*eV, 10.*eV };
     G4double Absorption[nEntries_ABS] = { gap_absLength*mm, gap_absLength*mm };
     
     GaMaterial -> GetMaterialPropertiesTable() -> RemoveProperty("ABSLENGTH");
     GaMaterial -> GetMaterialPropertiesTable() -> AddProperty("ABSLENGTH",PhotonEnergy_ABS,Absorption,nEntries_ABS);
   }
   if( gap_rIndex > 0 )
   {
     const G4int nEntries_RI = 2;
     G4double PhotonEnergy_RI[nEntries_RI] = { 1.*eV, 10.*eV };
     G4double RefractiveIndex[nEntries_RI] = { gap_rIndex, gap_rIndex };
     std::cout << "rIndex: " << gap_rIndex << std::endl;
     GaMaterial -> GetMaterialPropertiesTable() -> RemoveProperty("RINDEX");
     GaMaterial -> GetMaterialPropertiesTable() -> AddProperty("RINDEX",PhotonEnergy_RI,RefractiveIndex,nEntries_RI);
   }
  */
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((stepLimit)&&(maxStep>0.)) stepLimit->SetMaxAllowedStep(maxStep);
}
