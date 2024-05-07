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
// * institutes,nor the agencies providing financial support for this *
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
// $Id: DetectorConstruction.hh,v 1.5 2006-06-29 17:53:55 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <iostream>
#include <string>
#include <fstream>
#include <utility>

#include "ConfigFile.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"

#include "MyMaterials.hh"

#include "TString.h"


class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  //! ctor
  DetectorConstruction();
  DetectorConstruction(const string& configFileName);
  
  //! dtor
  ~DetectorConstruction();
  
  //! construct method
  G4VPhysicalVolume* Construct();
  
  //! other methods
  void initializeMaterials();
  void ConstructField();
  void SetMaxStep(G4double);
  
  
private:
  G4bool checkOverlaps;
  
  G4double world_x;
  G4double world_y;
  G4double world_z;
  G4int    world_material;
  
  G4int    container_material;
  G4double scint_yield;

  G4double panel_x;
  G4double panel_y;
  G4double panel_z;
  G4int    panel_material;
  
  G4int    vikuiti_side;
  G4double vikuiti_thickness;

  G4String lightGuide_geometry;
  G4int    lightGuide_N;
  G4int    lightGuide_material;
  G4int    lightGuide_WLSConc; 
  G4double lightGuide_x;
  G4double lightGuide_y;
  G4double lightGuide_z;

  G4int    supportPanel;
  G4double supportPanel_x;
  G4double supportPanel_y;
  G4double supportPanel_z;
  G4double supportPanel_distance;

  G4int primary_material;
  G4double primary_thickness;

  G4double gap_x;
  G4double gap_y;
  G4double gap_z;
  G4int    gap_material;
  G4double gap_rIndex;
  G4double gap_absLength;
  
  G4int  detector;
  G4String det_conf;
  G4double det_x;
  G4double det_y;
  G4double det_z;
  G4int    det_material;
  G4double det_efficiency;
  G4double det_reflectivity;
  
  G4double window_x;
  G4double window_y;
  G4double window_z;
  G4int    window_material;

  //Materials
  G4Material* WoMaterial;
  G4Material* CoMaterial;
  G4Material* PaMaterial;
  G4Material* LgMaterial;
  G4Material* PrMaterial;
  G4Material* DeMaterial;
  G4Material* WiMaterial;

  G4Material* VikuitiMaterial;
  G4OpticalSurface* VikuitiSurface;
  G4OpticalSurface* SiPMSurface;

  G4UserLimits* stepLimit;             // pointer to user step limits
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*DetectorConstruction_h*/
