#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "TString.h"
#include "TRandom3.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include <time.h>

#include <iostream>
#include <fstream>
#include <vector>
#include "TTree.h"

using namespace std;
using namespace CLHEP;



G4double fromEvToNm (G4double energy)
{
  return 1239.84187 / energy;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


SteppingAction::SteppingAction(const G4int& scint, const G4int& cher):
  propagateScintillation(scint),
  propagateCerenkov(cher)
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


SteppingAction::~SteppingAction ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void SteppingAction::UserSteppingAction (const G4Step * theStep)
{
  G4Track* theTrack = theStep->GetTrack () ;

  const G4ThreeVector& theTrackDirection = theTrack->GetMomentumDirection();
  const G4ThreeVector& theTrackVertexDirection = theTrack->GetVertexMomentumDirection();

  G4int trackID = theTrack->GetTrackID();
  G4TrackStatus theTrackStatus = theTrack -> GetTrackStatus ();
  G4ParticleDefinition* particleType = theTrack->GetDefinition () ;

  G4StepPoint * thePrePoint  = theStep->GetPreStepPoint () ;
  G4StepPoint * thePostPoint = theStep->GetPostStepPoint () ;
  const G4ThreeVector & thePrePosition  = thePrePoint->GetPosition () ;
  G4VPhysicalVolume * thePrePV  = thePrePoint->GetPhysicalVolume () ;
  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume () ;
  G4String thePrePVName  = "" ; if ( thePrePV )  thePrePVName  = thePrePV  -> GetName () ;
  G4String thePostPVName = "" ; if ( thePostPV ) thePostPVName = thePostPV -> GetName () ;
  G4VSolid* thePreS = thePrePV->GetLogicalVolume()->GetSolid();

  G4int nStep = theTrack -> GetCurrentStepNumber();

  //-------------
  // get position
  // G4double global_x = thePrePosition.x()/mm;
  // G4double global_y = thePrePosition.y()/mm;
  // G4double global_z = thePrePosition.z()/mm;


  // input track
  if( trackID == 1 )
  {
    if( G4StrUtil::contains(thePrePVName,"containerPV") )
    {
      G4ThreeVector momentum = theTrack->GetMomentum();
      CreateTree::Instance() -> outputMomentum->at(0) = momentum.x()/GeV;
      CreateTree::Instance() -> outputMomentum->at(1) = momentum.y()/GeV;
      CreateTree::Instance() -> outputMomentum->at(2) = momentum.z()/GeV;
      CreateTree::Instance() -> outputMomentum->at(3) = theTrack->GetTotalEnergy()/GeV;
    }
  }

  
  // optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    G4String processName = theTrack->GetCreatorProcess()->GetProcessName();
    G4String interName = thePostPoint->GetProcessDefinedStep()->GetProcessName();

    // Retrieve the status of the photon                                                                                                                                                                                
    G4OpBoundaryProcessStatus theStatus = Undefined;

    static G4ThreadLocal G4ProcessManager* OpManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    if(OpManager)
    {
      G4int nproc = OpManager->GetPostStepProcessVector()->entries();
      G4ProcessVector* fPostStepDoItVector = OpManager->GetPostStepProcessVector(typeDoIt);

      for(G4int i = 0; i < nproc; ++i)
      {
        G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
        fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
        if(fOpProcess)
        {
          theStatus = fOpProcess->GetStatus();
          break;
        }
      }
    }

    //G4cout << "nStep: " << nStep << "   thePrePoint: " << thePrePoint->GetPosition() << " thePrePV: " << thePrePVName << "   thePostPoint: " << thePostPoint->GetPosition() << "   thePostPV: " << thePostPVName << "   processName: " << processName << "   LVAtVertex: "<< theTrack->GetLogicalVolumeAtVertex()->GetName() << "   theTrackStatus: " << theTrackStatus << "   angle with (0,0,1): " << (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) << std::endl;
    //G4cout << "nStep: " << nStep << " thePrePV: " << thePrePVName << "   thePostPV: " << thePostPVName << "   processName: " << processName << "   proc: " << interName << "   status: " << theStatus << "   LVAtVertex: "<< theTrack->GetLogicalVolumeAtVertex()->GetName() << "   theTrackStatus: " << theTrackStatus << "   angle with (0,0,1): " << (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) << std::endl;
    //if( theStatus == Detection ) G4cout << "DETECTION!!!" << G4endl;
    
    //----------------------------
    // count photons at production
    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"containerLV") ) && 
        ( processName == "Scintillation" ) && 
        ( nStep == 1) ) 
    {
      CreateTree::Instance()->n_phot_sci += 1;
      CreateTree::Instance()->time_phot_sci.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->lambda_phot_sci.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->angle_phot_sci.push_back( cos(G4ThreeVector(0.,0.,-1.).angle(theTrackVertexDirection)) );
    }

    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"containerLV") ) && 
    ( G4StrUtil::contains(thePostPVName,"primary") ) &&
    ( interName == "OpWLS" ) && 
    ( theTrackStatus == 2) ) 
    {
      CreateTree::Instance()->n_phot_abs_primary += 1;
      CreateTree::Instance()->lambda_phot_abs_primary.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->time_phot_abs_primary.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->angle_phot_abs_primary.push_back( cos(G4ThreeVector(0.,0.,-1.).angle(theTrackVertexDirection)) );
    }

    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"primary") ) && 
    ( processName == "OpWLS" ) && 
    ( theTrackStatus == 0 ) &&
    ( nStep == 1) )
    {
      CreateTree::Instance()->n_phot_wls_primary += 1;
      CreateTree::Instance()->lambda_phot_wls_primary.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->time_phot_wls_primary.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->angle_phot_wls_primary.push_back( cos(G4ThreeVector(0.,0.,-1.).angle(theTrackVertexDirection)) );
    }

    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"primary") ) && 
    ( G4StrUtil::contains(thePostPVName,"lightGuide") ) &&
    ( interName == "OpWLS" ) && 
    ( theTrackStatus == 2) ) 
    {
      CreateTree::Instance()->n_phot_abs_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_abs_lightGuide.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->time_phot_abs_lightGuide.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->angle_phot_abs_lightGuide.push_back( cos(G4ThreeVector(0.,0.,-1.).angle(theTrackVertexDirection)) );
    }

    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"lightGuide") ) && 
    ( processName == "OpWLS" ) && 
    ( theTrackStatus == 0 ) &&
    ( nStep == 1) )
    {
      CreateTree::Instance()->n_phot_wls_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_wls_lightGuide.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->time_phot_wls_lightGuide.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->angle_phot_wls_lightGuide.push_back( cos(G4ThreeVector(0.,0.,-1.).angle(theTrackVertexDirection)) );
    }

    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"lightGuide") ) &&
      (theStatus == Detection) )
    {
      CreateTree::Instance()->n_phot_det += 1;
      CreateTree::Instance()->lambda_phot_det.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->time_phot_det.push_back( thePrePoint->GetGlobalTime()/picosecond );
      CreateTree::Instance()->angle_phot_det.push_back( cos(G4ThreeVector(1.,0.,0.).angle(theTrackVertexDirection)) );
    }
  }
//    
//      if( processName == "Cerenkov" )
//      {
//        CreateTree::Instance()->tot_phot_cer += 1;
//        
//        CreateTree::Instance()->h_phot_cer_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_cer_E -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_cer_time -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_cer_angleAtProduction -> Fill( G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958 );
//        CreateTree::Instance()->h_phot_cer_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        
//        CreateTree::Instance()->time_phot_cer.push_back( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->lambda_phot_cer.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        
//        if( !propagateCerenkov ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//      }
//      
//      //if( fromEvToNm(theTrack->GetTotalEnergy()/eV) > 1200. ) theTrack->SetTrackStatus(fKillTrackAndSecondaries); 
//    }
//    
//    
//    //-------------------------------------------
//    // count photons exiting from lateral surface
//    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"crystal") ) &&
//        (thePrePVName == "latLayerPV") && (thePostPVName == "worldPV") )
//    {
//      if( processName == "Scintillation" )
//      {
//        CreateTree::Instance()->tot_lat_phot_sci += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_sci_lat_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_sci_lat_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_sci_lat_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_sci_lat_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_lat_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_lat_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_sci_lat_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//      
//      if( processName == "Cerenkov" )
//      {
//        CreateTree::Instance()->tot_lat_phot_cer += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_cer_lat_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_cer_lat_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_cer_lat_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_cer_lat_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_lat_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_lat_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_cer_lat_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//    }
//    
//    
//    //-----------------------------------------
//    // count photons exiting from front surface
//    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"crystal") ) &&
//        (thePrePVName == "frontLayerPV") && (thePostPVName == "worldPV") )
//    {
//      if( processName == "Scintillation" )
//      {
//        CreateTree::Instance()->tot_fro_phot_sci += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_sci_fro_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_sci_fro_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_sci_fro_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_sci_fro_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_fro_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_fro_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_sci_fro_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//      
//      if( processName == "Cerenkov" )
//      {
//        CreateTree::Instance()->tot_fro_phot_cer += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_cer_fro_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_cer_fro_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_cer_fro_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_cer_fro_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_fro_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_fro_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_cer_fro_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//    }
//    
//    
//    //----------------------------------------
//    // count photons exiting from back surface
//    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"crystal") ) &&
//        (thePrePVName == "backLayerPV") && (thePostPVName == "worldPV" || thePostPVName == "containerPV") )
//    {
//      if( processName == "Scintillation" )
//      {
//        CreateTree::Instance()->tot_bac_phot_sci += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_sci_bac_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_sci_bac_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_sci_bac_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_sci_bac_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_bac_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_bac_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_sci_bac_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//      
//      if( processName == "Cerenkov" )
//      {
//        CreateTree::Instance()->tot_bac_phot_cer += 1;
//        // if you do not want to kill a photon once it exits the crystal, comment here below
//        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
//        
//        CreateTree::Instance()->h_phot_cer_bac_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_cer_bac_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_cer_bac_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_cer_bac_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_bac_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_bac_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_cer_bac_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//      }
//    }
//    
//    
//    //-----------------------
//    // count detected photons
//    if( ( G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(),"crystal") ) &&
//        ( G4StrUtil::contains(thePostPVName,"detPV")) && (theTrackStatus == fStopAndKill) )
//    {
//      if( processName == "Scintillation" )
//      {
//        CreateTree::Instance()->tot_det_phot_sci += 1;
//        if( G4StrUtil::contains(thePostPVName,"detPVL") ) CreateTree::Instance()->tot_detL_phot_sci += 1;
//        if( G4StrUtil::contains(thePostPVName,"detPVR") ) CreateTree::Instance()->tot_detR_phot_sci += 1;
//        
//        CreateTree::Instance()->h_phot_sci_det_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_sci_det_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_sci_det_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_sci_det_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_det_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_sci_det_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_sci_det_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//        
//        CreateTree::Instance()->time_det_phot_sci.push_back( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->lambda_det_phot_sci.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->trackLength_det_phot_sci.push_back( fromEvToNm(theTrack->GetTrackLength()/mm) );
//      }
//      
//      if( processName == "Cerenkov" )
//      {
//        CreateTree::Instance()->tot_det_phot_sci += 1;
//        if( G4StrUtil::contains(thePostPVName,"detPVL") ) CreateTree::Instance()->tot_detL_phot_sci += 1;
//        if( G4StrUtil::contains(thePostPVName,"detPVR") ) CreateTree::Instance()->tot_detR_phot_sci += 1;
//        
//        CreateTree::Instance()->h_phot_cer_det_lambda -> Fill( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->h_phot_cer_det_E      -> Fill( theTrack->GetTotalEnergy()/eV );
//        CreateTree::Instance()->h_phot_cer_det_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->h_phot_cer_det_angleAtProduction -> Fill( (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_det_angleWithSurfNormal -> Fill( (thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)*57.2958) );
//        CreateTree::Instance()->h_phot_cer_det_cosAngleAtProduction -> Fill( cos(G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)) );
//        CreateTree::Instance()->h_phot_cer_det_cosAngleWithSurfNormal -> Fill( cos(thePreS->SurfaceNormal(thePrePosition).angle(theTrackDirection)) );
//        
//        CreateTree::Instance()->time_det_phot_cer.push_back( thePrePoint->GetGlobalTime()/picosecond );
//        CreateTree::Instance()->lambda_det_phot_cer.push_back( fromEvToNm(theTrack->GetTotalEnergy()/eV) );
//        CreateTree::Instance()->trackLength_det_phot_cer.push_back( fromEvToNm(theTrack->GetTrackLength()/mm) );
//      }
//    }
//  } // optical photon
//
//  
//  // non optical photon
//  else
//  {
//    //G4cout << ">>> begin non optical photon" << G4endl;
//    
//    G4double energy = theStep->GetTotalEnergyDeposit() - theStep->GetNonIonizingEnergyDeposit();
//    if ( energy == 0. ) return;
//    
//    CreateTree::Instance() -> depositedEnergyTotal += energy/GeV;
//    
//    if( thePrePVName == "crystalPV" )
//    {
//      CreateTree::Instance() -> depositedEnergyCrystal += energy/GeV;
//      
//      CreateTree::Instance()->E_dep.push_back( energy/GeV );
//      CreateTree::Instance()->E_dep_time.push_back( thePrePoint->GetGlobalTime()/picosecond );
//    }
//    
//    if( G4StrUtil::contains(thePrePVName,"wrapping") )
//    {
//      CreateTree::Instance()->depositedEnergyWrapping += energy/GeV;
//    }
//    
//    if( G4StrUtil::contains(thePrePVName,"world") )
//    {
//      CreateTree::Instance() -> depositedEnergyWorld += energy/GeV;
//    }
//    
//    //G4cout << ">>> end non optical photon" << G4endl;
//  } // non optical photon
  
  
  return ;
}
