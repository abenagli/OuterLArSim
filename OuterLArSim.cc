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
// $Id: exampleN06.cc,v 1.18 2010-10-23 19:33:55 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// Description: Test of Continuous Process G4Cerenkov
//              and RestDiscrete Process G4Scintillation
//              -- Generation Cerenkov Photons --
//              -- Generation Scintillation Photons --
//              -- Transport of optical Photons --
// Version:     5.0
// Created:     1996-04-30
// Author:      Juliet Armstrong
// mail:        gum@triumf.ca
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "TString.h"
#include "TTree.h"
#include "TRandom3.h"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"
#include "CreateTree.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

using namespace CLHEP;

long int CreateSeed();

int main(int argc, char **argv)
{
  if (argc != 3 && argc != 2)
  {
    cout << "Syntax for exec: crystal <configuration file> <output file>" << endl;
    cout << "Syntax for viz:  crystal <configuration file>" << endl;
    return 0;
  }

  std::string file;
  std::string filename;
  TFile *outfile = NULL;

  if (argc == 3)
  {
    G4cout << "Starting exec mode..." << G4endl;
    file = argv[2];
    filename = file + ".root";
    G4cout << "Writing data to file '" << filename << "' ..." << G4endl;

    outfile = new TFile((TString)filename, "RECREATE");
    outfile->cd();
  }

  if (argc == 2)
  {
    G4cout << "Starting viz mode..." << G4endl;
  }

  G4cout << "=====>   C O N F I G U R A T I O N   <====\n"
         << G4endl;

  G4cout << "Configuration file: '" << argv[1] << "'" << G4endl;
  ConfigFile config(argv[1]);

  // Seed the random number generator manually
  //
  G4long myseed = config.read<long int>("seed");
  if (myseed == -1)
  {
    G4cout << "Creating random seed..." << G4endl;
    myseed = CreateSeed();
  }
  G4cout << "Random seed : " << myseed << G4endl;
  CLHEP::HepRandom::setTheSeed(myseed);

  // Create output tree
  //
  CreateTree *mytree = new CreateTree("tree");

  // Get runtime options
  //
  G4int printModulo = config.read<int>("printModulo");
  G4int switchOnScintillation = config.read<int>("switchOnScintillation");
  G4int switchOnCerenkov = config.read<int>("switchOnCerenkov");
  G4int propagateScintillation = config.read<int>("propagateScintillation");
  G4int propagateCerenkov = config.read<int>("propagateCerenkov");

  // User Verbose output class
  //
  G4VSteppingVerbose *verbosity = new SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);

  // Run manager
  //auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
  
  // Physics list defined using PhysListFactory
  //
  std::string physName("");

  G4PhysListFactory factory;
  const std::vector<G4String> &names = factory.AvailablePhysLists();
  for (unsigned n = 0; n != names.size(); n++)
    G4cout << "PhysicsList: " << names[n] << G4endl;

  if (physName == "")
  {
    char *path = getenv("PHYSLIST");
    if (path)
      physName = G4String(path);
  }

  if (physName == "" || factory.IsReferencePhysList(physName))
  {
    physName = "FTFP_BERT_EMZ"; // most precise
  }

  std::cout << "Using physics list: " << physName << std::endl;

  // UserInitialization classes - mandatory
  //

  G4cout << ">>> Define physics list::begin <<<" << G4endl;
  G4VModularPhysicsList *physics = factory.GetReferencePhysList(physName);
  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
  G4OpticalParameters *opticalParams = G4OpticalParameters::Instance();
  //opticalParams->SetVerboseLevel(2);
  physics->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physics);
  G4cout << ">>> Define physics list::end <<<" << G4endl;

  G4cout << ">>> Define DetectorConstruction::begin <<<" << G4endl;
  DetectorConstruction *detector = new DetectorConstruction(argv[1]);
  runManager->SetUserInitialization(detector);
  G4cout << ">>> Define DetectorConstruction::end <<<" << G4endl;
  
  G4cout << ">>> Define ActionInitialization::begin <<<" << G4endl;
  G4double source_dist = config.read<double>("source_dist");
  ActionInitialization *actionInitialization = new ActionInitialization(printModulo, source_dist, propagateScintillation, propagateCerenkov);
  runManager->SetUserInitialization(actionInitialization);
  G4cout << ">>> Define ActionInitialization::end <<<" << G4endl;

  string gps_instructions_file = "";

  if (argc == 2) // Define UI session for interactive mode
  {
    // Initialize G4 kernel
    //
    runManager->Initialize();

    G4VisManager *visManager = new G4VisExecutive;
    visManager->Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    G4UIExecutive *ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute macros/vis.mac");
    ui->SessionStart();
    delete ui;
    delete visManager;
  }

  else
  {
    runManager->Initialize();
    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    config.readInto(gps_instructions_file, "gps_instructions_file");
    UImanager->ApplyCommand("/control/execute " + gps_instructions_file);
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not
  // be deleted in the main() program !
  //
  delete runManager;
  delete verbosity;

  if (argc == 3)
  {
    G4cout << "Writing tree to file " << filename << " ..." << G4endl;
    mytree->Write(outfile);
    outfile->Close();
  }

  return 0;
}

long int CreateSeed()
{
  TRandom3 rangen;

  long int sec = time(0);
  G4cout << "Time : " << sec << G4endl;

  //   sec += getpid();
  //   G4cout << "PID  : " << getpid() << G4endl;

  FILE *fp = fopen("/proc/uptime", "r");
  int upsecs = 0;
  if (fp != NULL)
  {
    char buf[BUFSIZ];
    char *b = fgets(buf, BUFSIZ, fp);
    if (b == buf)
    {
      /* The following sscanf must use the C locale.  */
      setlocale(LC_NUMERIC, "C");
      setlocale(LC_NUMERIC, "");
    }
    fclose(fp);
  }
  G4cout << "Upsecs: " << upsecs << G4endl;
  sec += upsecs;

  G4cout << "Seed for srand: " << sec << G4endl;
  srand(sec);
  rangen.SetSeed(rand());
  long int seed = round(1000000 * rangen.Uniform());
  return seed;
}
