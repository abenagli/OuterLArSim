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
// $Id: PrimaryGeneratorAction.cc,v 1.6 2006-06-29 17:54:27 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "CreateTree.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4double& z_0)
{
  G4GeneralParticleSource* gps = new G4GeneralParticleSource();
  
  z_0_ = z_0;
  gun_ = gps;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete gun_;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //G4ThreeVector posCentre(0.*CLHEP::mm,300.*CLHEP::mm,z_0_*CLHEP::mm);
  //gun_ -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(posCentre);

  if( gun_->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    G4double angle = G4UniformRand() * 360.0 * deg;
    SetOptPhotonPolar(angle);
  }
  
  gun_ -> GeneratePrimaryVertex(anEvent);
}


void PrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if( gun_->GetParticleDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    G4ExceptionDescription ed;
    ed << "The particleGun is not an opticalphoton.";
    G4Exception("PrimaryGeneratorAction::SetOptPhotonPolar", "neutronModerator",
                JustWarning, ed);
    return;
  }

  G4ThreeVector normal(1., 0., 0.);
  G4ThreeVector kphoton = gun_->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  G4double modul2       = product * product;

  G4ThreeVector e_perpend(0., 0., 1.);
  if(modul2 > 0.)
    e_perpend = (1. / std::sqrt(modul2)) * product;
  G4ThreeVector e_paralle = e_perpend.cross(kphoton);

  G4ThreeVector polar =
    std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
  gun_->SetParticlePolarization(polar);
}
