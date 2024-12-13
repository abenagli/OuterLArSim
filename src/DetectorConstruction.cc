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
  config.readInto(vikuiti_side, "vikuiti_side");
  config.readInto(vikuiti_thickness, "vikuiti_thickness");
  config.readInto(lightGuide_geometry, "lightGuide_geometry");
  config.readInto(lightGuide_N, "lightGuide_N");
  config.readInto(lightGuide_x, "lightGuide_x");
  config.readInto(lightGuide_y, "lightGuide_y");
  config.readInto(lightGuide_z, "lightGuide_z");
  config.readInto(lightGuide_material, "lightGuide_material");
  config.readInto(lightGuide_WLSConc, "lightGuide_WLSConc"); 
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
  config.readInto(det_N, "det_N");
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
  
  
  // the light guide container
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4double lightGuideContainer_x = lightGuide_x + 2. * ( window_z + det_z );
  G4double lightGuideContainer_y = lightGuide_y + vikuiti_thickness;
  G4double lightGuideContainer_z = lightGuide_z + 2. * ( window_z + det_z );
  if( supportPanel ) lightGuideContainer_y += supportPanel_distance + supportPanel_y + primary_thickness;
  else               lightGuideContainer_y += primary_thickness;
  G4VSolid* lightGuideContainerS = new G4Box("lightGuideContainerS", 0.5*lightGuideContainer_x, 0.5*lightGuideContainer_y, 0.5*lightGuideContainer_z);
  G4LogicalVolume* lightGuideContainerLV = new G4LogicalVolume(lightGuideContainerS, CoMaterial, "lightGuideContainerLV");


  // the light guide
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid* lightGuideS = NULL;
  G4LogicalVolume* lightGuideLV = NULL;
  G4VPhysicalVolume* lightGuidePV = NULL;
  G4LogicalVolume* vikuitiWrappingMidLeftLV = NULL;
  G4LogicalVolume* vikuitiWrappingMidRightLV = NULL;
  if( 1 )
  {
    lightGuideS = new G4Box("lightGuideS", 0.5*lightGuide_x, 0.5*lightGuide_y, 0.5*lightGuide_z);
    lightGuideLV = new G4LogicalVolume(lightGuideS, LgMaterial, "lightGuideLV");
    lightGuidePV = new G4PVPlacement(0, G4ThreeVector(0.,-0.5*(lightGuideContainer_y-lightGuide_y)+vikuiti_thickness,0.), lightGuideLV, "lightGuidePV", lightGuideContainerLV, false, 0, checkOverlaps);
  }
  else
  {
    lightGuideS = new G4Box("lightGuideS", 0.25*lightGuide_x-0.5*vikuiti_thickness, 0.5*lightGuide_y, 0.5*lightGuide_z);
    lightGuideLV = new G4LogicalVolume(lightGuideS, LgMaterial, "lightGuideLV");
    lightGuidePV = new G4PVPlacement(0, G4ThreeVector(-0.25*lightGuide_x-0.5*vikuiti_thickness,-0.5*(lightGuideContainer_y-lightGuide_y)+vikuiti_thickness,0.), lightGuideLV, "lightGuidePV", lightGuideContainerLV, false, 0, checkOverlaps);
    lightGuidePV = new G4PVPlacement(0, G4ThreeVector(+0.25*lightGuide_x+0.5*vikuiti_thickness,-0.5*(lightGuideContainer_y-lightGuide_y)+vikuiti_thickness,0.), lightGuideLV, "lightGuidePV", lightGuideContainerLV, false, 0, checkOverlaps);
    
    G4VSolid* vikuitiWrappingMidLeftS = new G4Box("vikuitiWrappingMidLeftS", 0.5*vikuiti_thickness, 0.5*lightGuide_y, 0.5*lightGuide_z);
    vikuitiWrappingMidLeftLV = new G4LogicalVolume(vikuitiWrappingMidLeftS, VikuitiMaterial, "vikuitiWrappingMidLeftLV");
    G4VPhysicalVolume* vikuitiWrappingMidLeftPV = new G4PVPlacement(0, G4ThreeVector(-0.5*vikuiti_thickness,-0.5*(lightGuideContainer_y-lightGuide_y)+vikuiti_thickness,0.), vikuitiWrappingMidLeftLV, "vikuitiWrappingMidLeftPV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* vikuitiWrappingMidLeftSurface = new G4LogicalBorderSurface("vikuityWrappingMidLeftSurface", lightGuidePV, vikuitiWrappingMidLeftPV, VikuitiSurface);
    
    G4VSolid* vikuitiWrappingMidRightS = new G4Box("vikuitiWrappingMidRightS", 0.5*vikuiti_thickness, 0.5*lightGuide_y, 0.5*lightGuide_z);
    vikuitiWrappingMidRightLV = new G4LogicalVolume(vikuitiWrappingMidRightS, VikuitiMaterial, "vikuitiWrappingMidRightLV");
    G4VPhysicalVolume* vikuitiWrappingMidRightPV = new G4PVPlacement(0, G4ThreeVector(+0.5*vikuiti_thickness,-0.5*(lightGuideContainer_y-lightGuide_y)+vikuiti_thickness,0.), vikuitiWrappingMidRightLV, "vikuitiWrappingMidRightPV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* vikuitiWrappingMidRightSurface = new G4LogicalBorderSurface("vikuityWrappingMidRightSurface", lightGuidePV, vikuitiWrappingMidRightPV, VikuitiSurface);
  }

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
    
    G4double supportPanel_yCenter = -0.5*(lightGuideContainer_y-supportPanel_y)+vikuiti_thickness+lightGuide_y+supportPanel_distance;
    G4double primary_yCenter = -0.5*(lightGuideContainer_y-primary_thickness)+vikuiti_thickness+lightGuide_y+supportPanel_distance+supportPanel_y;
    G4VPhysicalVolume* supportPanelPV = new G4PVPlacement(0, G4ThreeVector(0.,supportPanel_yCenter,0.), supportPanelLV, "supportPanelPV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* primaryPV = new G4PVPlacement(0, G4ThreeVector(0.,primary_yCenter,0.), primaryLV, "primaryPV", lightGuideContainerLV, false, 0, checkOverlaps);
  }
  else
  {
    primaryS = new G4Box("primaryS", 0.5*lightGuide_x, 0.5*primary_thickness, 0.5*lightGuide_z);
    primaryLV = new G4LogicalVolume(primaryS, PrMaterial, "primaryLV");
    G4double primary_yCenter = -0.5*(lightGuideContainer_y-primary_thickness)+vikuiti_thickness+lightGuide_y;
    G4VPhysicalVolume* primaryPV = new G4PVPlacement(0, G4ThreeVector(0.,primary_yCenter,0.), primaryLV, "primaryPV", lightGuideContainerLV, false, 0, checkOverlaps);
  }
  
  G4VSolid* vikuitiWrappingBackS = new G4Box("vikuitiWrappingBackS", 0.5*lightGuide_x, 0.5*vikuiti_thickness, 0.5*lightGuide_z);
  G4LogicalVolume* vikuitiWrappingBackLV = new G4LogicalVolume(vikuitiWrappingBackS, VikuitiMaterial, "vikuitiWrappingBackLV");
  G4VPhysicalVolume* vikuitiWrappingBackPV = new G4PVPlacement(0, G4ThreeVector(0.,-0.5*(lightGuideContainer_y-vikuiti_thickness),0.), vikuitiWrappingBackLV, "vikuitiWrappingBackPV", lightGuideContainerLV, false, 0, checkOverlaps);
  G4LogicalBorderSurface* vikuitiWrappingBackSurface = new G4LogicalBorderSurface("vikuityWrappingBackSurface", lightGuidePV, vikuitiWrappingBackPV, VikuitiSurface);
  
  
  G4LogicalVolume* vikuitiWrappingSideXLV = NULL;
  G4LogicalVolume* vikuitiWrappingSideZLV = NULL;
  
  
  
  // the SiPMs
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid* detS = new G4Box("detS", 0.5*det_x, 0.5*det_y, 0.5*det_z);
  G4LogicalVolume* detLV = new G4LogicalVolume(detS, DeMaterial, "detLV");
  
  G4VSolid* windowS = new G4Box("windowS", 0.5*window_x, 0.5*window_y, 0.5*window_z);
  G4LogicalVolume* windowLV = new G4LogicalVolume(windowS, WiMaterial, "windowLV");
  
  std::map<int,G4double> coord_x;
  std::map<int,G4double> coord_y;
  std::map<int,G4double> coord_z;  
  for(G4int iDet = 0; iDet < det_N; ++iDet)
  {
    G4RotationMatrix* rot = 0;
    G4double y_center = -0.5*lightGuideContainer_y+vikuiti_thickness+0.5*lightGuide_y;
    G4double det_x_center = 0.;
    G4double det_z_center = 0.;
    G4double window_x_center = 0.;
    G4double window_z_center = 0.;
    G4double spacingX = 0.;
    G4double spacingZ = 0.;
    G4int det_N_face = 0;

    if( det_conf == "shortLR" )
    {
      det_N_face = int(det_N/2.);

      spacingZ = ( lightGuide_z - det_x*det_N_face ) / (det_N_face+1.);
      
      rot = rotY90m;
      window_x_center = - ( 0.5*lightGuide_x + 0.5*window_z );
      det_x_center = - ( 0.5*lightGuide_x  + window_z + 0.5*det_z );
      det_z_center = -0.5*lightGuide_z + spacingZ*((iDet%det_N_face)+1.) + det_x*(iDet%det_N_face) + 0.5*det_x;
      window_z_center = det_z_center;
      if( int(iDet/det_N_face) == 1 )
      {
        window_x_center *= -1;
        det_x_center *= -1;
      }
    }
    
    else if( det_conf == "longUD" )
    {
      det_N_face = int(det_N/2.);

      spacingX = ( lightGuide_x - det_x*det_N_face ) / (det_N_face+1.);
      
      window_z_center = - ( 0.5*lightGuide_z + 0.5*window_z );
      det_z_center = - ( 0.5*lightGuide_z  + window_z + 0.5*det_z );
      det_x_center = -0.5*lightGuide_x + spacingX*((iDet%det_N_face)+1.) + det_x*(iDet%det_N_face) + 0.5*det_x;
      window_x_center = det_x_center;
      if( int(iDet/det_N_face) == 1 )
      {
        window_z_center *= -1;
        det_z_center *= -1;
      }
    }

    else if( det_conf == "all" )
    {
      det_N_face = int(det_N/4.);

      spacingX = ( lightGuide_x - det_x*det_N_face ) / (det_N_face+1.);
      spacingZ = ( lightGuide_z - det_x*det_N_face ) / (det_N_face+1.);
      if( int(iDet/det_N_face)%2 == 0 )
      {
        rot = rotY90m;
        window_x_center = - ( 0.5*lightGuide_x + 0.5*window_z );
        det_x_center = - ( 0.5*lightGuide_x  + window_z + 0.5*det_z );
        det_z_center = -0.5*lightGuide_z + spacingZ*((iDet%det_N_face)+1.) + det_x*(iDet%det_N_face) + 0.5*det_x;
        window_z_center = det_z_center;
        if( int(int(iDet/det_N_face)/2) == 1 )
        {
          window_x_center *= -1;
          det_x_center *= -1;
        }
      }
      else
      {
        rot = 0;
        window_z_center = - ( 0.5*lightGuide_z + 0.5*window_z );
        det_z_center = - ( 0.5*lightGuide_z  + window_z + 0.5*det_z );
        det_x_center = -0.5*lightGuide_x + spacingX*((iDet%det_N_face)+1.) + det_x*(iDet%det_N_face) + 0.5*det_x;
        window_x_center = det_x_center;        
        if( int(int(iDet/det_N_face)/2) == 1 )
        {
          window_z_center *= -1;
          det_z_center *= -1;
        }
      }
    }
    
    G4VPhysicalVolume* windowPV = new G4PVPlacement(rot, G4ThreeVector(window_x_center,y_center,window_z_center), windowLV, "windowPV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* detPV = new G4PVPlacement(rot, G4ThreeVector(det_x_center,y_center,det_z_center), detLV, "detPV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* SiPMWindowSurface = new G4LogicalBorderSurface("SiPMWindowSurface", windowPV, detPV, SiPMSurface);
    
    coord_x[iDet] = det_x_center;
    coord_y[iDet] = y_center;
    coord_z[iDet] = det_z_center;    
  }
  
  
  if( vikuiti_side )
  {
    G4VSolid* rodX = new G4Box("rodX",5.*m,0.75*det_x,0.75*det_y);
    G4VSolid* rodZ = new G4Box("rodZ",0.75*det_x,0.75*det_y,5.*m);
    
    G4VSolid* tempXS = new G4Box("vikuitiWrappingSideXS", 0.5*lightGuide_x, 0.5*lightGuide_y, 0.5*vikuiti_thickness);
    G4VSolid* vikuitiWrappingSideXS = tempXS;

    G4VSolid* tempZS = new G4Box("vikuitiWrappingSideZS", 0.5*vikuiti_thickness, 0.5*lightGuide_y, 0.5*lightGuide_z);
    G4VSolid* vikuitiWrappingSideZS = tempZS;

    for(G4int iDet = 0; iDet < int(det_N/2); ++iDet)
    {
      if( lightGuide_geometry == "slats" and det_conf == "shortLR" )
      {
        vikuitiWrappingSideZS = new G4SubtractionSolid("vikuitiWrappingSideZS", vikuitiWrappingSideZS, rodX, 0, G4ThreeVector(0.,0.,coord_z[iDet]));
      }
      
      else if( lightGuide_geometry == "slats" and det_conf == "longUD" )
      {
        vikuitiWrappingSideXS = new G4SubtractionSolid("vikuitiWrappingSideXS", vikuitiWrappingSideXS, rodZ, 0, G4ThreeVector(coord_x[iDet],0.,0.));
      }

      else if( lightGuide_geometry == "tiles" )
      {
        G4int det_N_face = int(det_N/4.);
        
        if( int(iDet/det_N_face)%2 == 0 )
        {
          vikuitiWrappingSideXS = new G4SubtractionSolid("vikuitiWrappingSideXS", vikuitiWrappingSideXS, rodZ, 0, G4ThreeVector(coord_z[iDet],0.,0.));
        }
        else
        {
          vikuitiWrappingSideZS = new G4SubtractionSolid("vikuitiWrappingSideZS", vikuitiWrappingSideZS, rodX, 0, G4ThreeVector(0.,0.,coord_x[iDet]));
        }
      }
    }
    
    vikuitiWrappingSideXLV = new G4LogicalVolume(vikuitiWrappingSideXS, VikuitiMaterial, "vikuitiWrappingSideXLV");
    vikuitiWrappingSideZLV = new G4LogicalVolume(vikuitiWrappingSideZS, VikuitiMaterial, "vikuitiWrappingSideZLV");

    G4VPhysicalVolume* vikuitiWrappingSideX1PV = new G4PVPlacement(0, G4ThreeVector(0.,-0.5*lightGuideContainer_y+vikuiti_thickness+0.5*lightGuide_y,0.5*(lightGuide_z+vikuiti_thickness)), vikuitiWrappingSideXLV, "vikuitiWrappingSideX1PV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* vikuitiWrappingSideX2PV = new G4PVPlacement(0, G4ThreeVector(0.,-0.5*lightGuideContainer_y+vikuiti_thickness+0.5*lightGuide_y,-0.5*(lightGuide_z+vikuiti_thickness)), vikuitiWrappingSideXLV, "vikuitiWrappingSideX2PV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* vikuitiWrappingSideX1Surface = new G4LogicalBorderSurface("vikuityWrappingSideX1Surface", lightGuidePV, vikuitiWrappingSideX1PV, VikuitiSurface);
    G4LogicalBorderSurface* vikuitiWrappingSideX2Surface = new G4LogicalBorderSurface("vikuityWrappingSideX2Surface", lightGuidePV, vikuitiWrappingSideX2PV, VikuitiSurface);
    
    G4VPhysicalVolume* vikuitiWrappingSideZ1PV = new G4PVPlacement(0, G4ThreeVector(0.5*(lightGuide_x+vikuiti_thickness),-0.5*lightGuideContainer_y+vikuiti_thickness+0.5*lightGuide_y,0.), vikuitiWrappingSideZLV, "vikuitiWrappingSideZ1PV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4VPhysicalVolume* vikuitiWrappingSideZ2PV = new G4PVPlacement(0, G4ThreeVector(-0.5*(lightGuide_x+vikuiti_thickness),-0.5*lightGuideContainer_y+vikuiti_thickness+0.5*lightGuide_y,0.), vikuitiWrappingSideZLV, "vikuitiWrappingSideZ2PV", lightGuideContainerLV, false, 0, checkOverlaps);
    G4LogicalBorderSurface* vikuitiWrappingSideZ1Surface = new G4LogicalBorderSurface("vikuityWrappingSideZ1Surface", lightGuidePV, vikuitiWrappingSideZ1PV, VikuitiSurface);
    G4LogicalBorderSurface* vikuitiWrappingSideZ2Surface = new G4LogicalBorderSurface("vikuityWrappingSideZ2Surface", lightGuidePV, vikuitiWrappingSideZ2PV, VikuitiSurface);
  }
  
  
  // placement of the light guides
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  for(int jj = 0; jj < lightGuide_N; ++jj)
  {
    G4double spacing_x = 0.;
    G4double spacing_z = 0.;
    G4double delta_x = 0.;
    G4double delta_z = 0.;
    
    if( lightGuide_geometry == "slats" || lightGuide_geometry == "slatsLongFace")
    {
      spacing_z = (panel_z - lightGuide_N*lightGuide_z) / (lightGuide_N+1);
      delta_z = 0.5*panel_z - spacing_z*(jj+1.) - lightGuide_z*(jj*1.) - 0.5*lightGuide_z;      
    }
    else if( lightGuide_geometry == "tiles" )
    {
      spacing_x = (panel_x - 2.*lightGuide_x) / 3.;
      delta_x = 0.5*panel_x - spacing_x*(int(jj%2)+1.) - lightGuide_x*(int(jj%2)*1.) - 0.5*lightGuide_x;
      spacing_z = (panel_z - 0.5*lightGuide_N*lightGuide_z) / (0.5*lightGuide_N+1);
      delta_z = 0.5*panel_z - spacing_z*(int(jj/2)+1.) - lightGuide_z*(int(jj/2)*1.) - 0.5*lightGuide_z;
    }
    
    G4VPhysicalVolume* lightGuideContainerPV = new G4PVPlacement(0, G4ThreeVector(delta_x,+0.5*(panel_y+lightGuideContainer_y),delta_z), lightGuideContainerLV, Form("lightGuideContainer%02dPV",jj), containerLV, false, 0, checkOverlaps);
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
  VisAttWorld -> SetVisibility(false) ;
  VisAttWorld -> SetForceWireframe(true) ;
  worldLV -> SetVisAttributes(VisAttWorld) ;
  
  G4VisAttributes* VisAttContainer = new G4VisAttributes(yellow);
  VisAttContainer -> SetVisibility(false) ;
  VisAttContainer -> SetForceWireframe(true) ;
  containerLV -> SetVisAttributes(VisAttContainer) ;
  
  G4VisAttributes* VisAttPanel = new G4VisAttributes(gray);
  VisAttPanel -> SetVisibility(true);
  VisAttPanel -> SetForceSolid(true);
  panelLV -> SetVisAttributes(VisAttPanel);
  if( supportPanel ) supportPanelLV -> SetVisAttributes(VisAttPanel);

  G4VisAttributes* VisAttLightGuideContainer = new G4VisAttributes(magenta);
  VisAttLightGuideContainer -> SetVisibility(false);
  VisAttLightGuideContainer -> SetForceWireframe(true);
  lightGuideContainerLV -> SetVisAttributes(VisAttLightGuideContainer);

  G4VisAttributes* VisAttLightGuide = new G4VisAttributes(green);
  VisAttLightGuide -> SetVisibility(true);
  VisAttLightGuide -> SetForceSolid(true);
  lightGuideLV -> SetVisAttributes(VisAttLightGuide);
  
  G4VisAttributes* VisAttVikuitiWrapping = new G4VisAttributes(yellow);
  VisAttVikuitiWrapping -> SetVisibility(true);
  VisAttVikuitiWrapping -> SetForceWireframe(false);
  vikuitiWrappingBackLV -> SetVisAttributes(VisAttVikuitiWrapping);
  if( vikuitiWrappingMidLeftLV ) vikuitiWrappingMidLeftLV -> SetVisAttributes(VisAttVikuitiWrapping);
  if( vikuitiWrappingMidRightLV ) vikuitiWrappingMidRightLV -> SetVisAttributes(VisAttVikuitiWrapping);
  if( vikuiti_side ) vikuitiWrappingSideXLV -> SetVisAttributes(VisAttVikuitiWrapping);
  if( vikuiti_side ) vikuitiWrappingSideZLV -> SetVisAttributes(VisAttVikuitiWrapping);
  
  G4VisAttributes* VisAttPrimary = new G4VisAttributes(cyan);
  VisAttPrimary -> SetVisibility(true);
  VisAttPrimary -> SetForceSolid(true);
  primaryLV -> SetVisAttributes(VisAttPrimary);
  
  G4VisAttributes* VisAttDet = new G4VisAttributes(gray);
  VisAttDet -> SetVisibility(true);
  VisAttDet -> SetForceSolid(true);
  detLV -> SetVisAttributes(VisAttDet);
  
  G4VisAttributes* VisAttWindow = new G4VisAttributes(blue);
  VisAttWindow -> SetVisibility(true);
  VisAttWindow -> SetForceSolid(true);
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
  if      ( lightGuide_material == 1 )  LgMaterial = MyMaterials::PMMA(lightGuide_WLSConc);
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
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((stepLimit)&&(maxStep>0.)) stepLimit->SetMaxAllowedStep(maxStep);
}
