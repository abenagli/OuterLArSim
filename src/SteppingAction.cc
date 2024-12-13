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

G4double fromEvToNm(G4double energy)
{
  return 1239.84187 / energy;
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::SteppingAction(const G4int &scint, const G4int &cher) :
  propagateScintillation(scint),
  propagateCerenkov(cher)
{
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::~SteppingAction()
{
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void SteppingAction::UserSteppingAction(const G4Step *theStep)
{
  G4Track *theTrack = theStep->GetTrack();
  
  const G4ThreeVector &theTrackDirection = theTrack->GetMomentumDirection();
  const G4ThreeVector &theTrackVertexDirection = theTrack->GetVertexMomentumDirection();
  
  G4int trackID = theTrack->GetTrackID();
  G4TrackStatus theTrackStatus = theTrack->GetTrackStatus();
  G4ParticleDefinition *particleType = theTrack->GetDefinition();
  
  G4StepPoint *thePrePoint = theStep->GetPreStepPoint();
  G4StepPoint *thePostPoint = theStep->GetPostStepPoint();
  const G4ThreeVector &thePrePosition = thePrePoint->GetPosition();
  G4VPhysicalVolume *thePrePV = thePrePoint->GetPhysicalVolume();
  G4VPhysicalVolume *thePostPV = thePostPoint->GetPhysicalVolume();
  G4String thePrePVName = "";
  if (thePrePV)
    thePrePVName = thePrePV->GetName();
  G4String thePostPVName = "";
  if (thePostPV)
    thePostPVName = thePostPV->GetName();
  G4VSolid *thePreS = thePrePV->GetLogicalVolume()->GetSolid();

  G4int nStep = theTrack->GetCurrentStepNumber();

  //-------------
  // get position
  // G4double global_x = thePrePosition.x()/mm;
  // G4double global_y = thePrePosition.y()/mm;
  // G4double global_z = thePrePosition.z()/mm;

  // input track
  if (trackID == 1)
  {
    if (G4StrUtil::contains(thePrePVName, "containerPV"))
    {
      G4ThreeVector momentum = theTrack->GetMomentum();
      CreateTree::Instance()->outputMomentum->at(0) = momentum.x() / GeV;
      CreateTree::Instance()->outputMomentum->at(1) = momentum.y() / GeV;
      CreateTree::Instance()->outputMomentum->at(2) = momentum.z() / GeV;
      CreateTree::Instance()->outputMomentum->at(3) = theTrack->GetTotalEnergy() / GeV;
    }
  }
  
  // optical photon
  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    
    G4String processName = "NONE";
    if( theTrack->GetCreatorProcess() ) processName = theTrack->GetCreatorProcess()->GetProcessName();
    G4String interName = thePostPoint->GetProcessDefinedStep()->GetProcessName();
    
    // Retrieve the status of the photon
    G4OpBoundaryProcessStatus theStatus = Undefined;
    
    static G4ThreadLocal G4ProcessManager *OpManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    if (OpManager)
    {
      G4int nproc = OpManager->GetPostStepProcessVector()->entries();
      G4ProcessVector *fPostStepDoItVector = OpManager->GetPostStepProcessVector(typeDoIt);

      for (G4int i = 0; i < nproc; ++i)
      {
        G4VProcess *fCurrentProcess = (*fPostStepDoItVector)[i];
        fOpProcess = dynamic_cast<G4OpBoundaryProcess *>(fCurrentProcess);
        if (fOpProcess)
        {
          theStatus = fOpProcess->GetStatus();
          break;
        }
      }
    }
    
    // G4cout << "nStep: " << nStep << "   thePrePoint: " << thePrePoint->GetPosition() << " thePrePV: " << thePrePVName << "   thePostPoint: " << thePostPoint->GetPosition() << "   thePostPV: " << thePostPVName << "   processName: " << processName << "   LVAtVertex: "<< theTrack->GetLogicalVolumeAtVertex()->GetName() << "   theTrackStatus: " << theTrackStatus << "   angle with (0,0,1): " << (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) << std::endl;
    // G4cout << "nStep: " << nStep << " thePrePV: " << thePrePVName << "   thePostPV: " << thePostPVName << "   processName: " << processName << "   proc: " << interName << "   status: " << theStatus << "   LVAtVertex: "<< theTrack->GetLogicalVolumeAtVertex()->GetName() << "   theTrackStatus: " << theTrackStatus << "   angle with (0,0,1): " << (G4ThreeVector(0.,0.,1.).angle(theTrackVertexDirection)*57.2958) << std::endl;
    // if( theStatus == Detection ) G4cout << "DETECTION!!!" << G4endl;

    //----------------------------
    // count photons at production
    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "containerLV")) &&
        (processName == "Scintillation") &&
        (nStep == 1))
    {
      CreateTree::Instance()->n_phot_sci_LAr += 1;
      CreateTree::Instance()->time_phot_sci_LAr.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->lambda_phot_sci_LAr.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->angle_phot_sci_LAr.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    //-----------------------------
    // count photons in primary wls
    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "containerLV")) &&
        (G4StrUtil::contains(thePostPVName, "primary")) &&
        (interName == "OpWLS") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_wls_primary += 1;
      CreateTree::Instance()->lambda_phot_wls_primary.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_wls_primary.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_wls_primary.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "containerLV")) &&
        (G4StrUtil::contains(thePostPVName, "primary")) &&
        (interName == "OpAbsorption") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_abs_primary += 1;
      CreateTree::Instance()->lambda_phot_abs_primary.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_abs_primary.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_abs_primary.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "primary")) &&
        (processName == "OpWLS") &&
        (nStep == 1))
    {
      CreateTree::Instance()->n_phot_sci_primary += 1;
      CreateTree::Instance()->lambda_phot_sci_primary.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_sci_primary.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_sci_primary.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    //--------------------------------
    // count photons in the lightguide
    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "primary")) &&
        (G4StrUtil::contains(thePostPVName, "lightGuide")) &&
        (interName == "OpWLS") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_wls_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_wls_lightGuide.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_wls_lightGuide.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_wls_lightGuide.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "primary")) &&
        (G4StrUtil::contains(thePostPVName, "lightGuide")) &&
        (interName == "OpAbsorption") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_abs_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_abs_lightGuide.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_abs_lightGuide.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_abs_lightGuide.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }
    
    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "lightGuide")) &&
        (G4StrUtil::contains(thePostPVName, "lightGuide")) &&
        (interName == "OpWLS") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_selfwls_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_selfwls_lightGuide.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_selfwls_lightGuide.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_selfwls_lightGuide.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "lightGuide")) &&
        (G4StrUtil::contains(thePostPVName, "lightGuide")) &&
        (interName == "OpAbsorption") &&
        (theTrackStatus == 2))
    {
      CreateTree::Instance()->n_phot_selfabs_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_selfabs_lightGuide.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_selfabs_lightGuide.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_selfabs_lightGuide.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }

    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "lightGuide")) &&
        (processName == "OpWLS") &&
        (theTrackStatus == 0) &&
        (nStep == 1))
    {
      CreateTree::Instance()->n_phot_sci_lightGuide += 1;
      CreateTree::Instance()->lambda_phot_sci_lightGuide.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_sci_lightGuide.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_sci_lightGuide.push_back(cos(G4ThreeVector(0., 0., -1.).angle(theTrackVertexDirection)));
    }
    
    if( (G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "lightGuide")) )
    {
      CreateTree::Instance()->trackLengthMap_phot_det[int(trackID)] += float(theStep->GetStepLength()*mm);
      CreateTree::Instance()->prodPositionMap_phot[int(trackID)] = theTrack->GetVertexPosition();
    }
    
    //----------------------
    // count photons at SiPM
    if ((G4StrUtil::contains(theTrack->GetLogicalVolumeAtVertex()->GetName(), "lightGuide")) &&
        (theStatus == Detection))
    {
      CreateTree::Instance()->n_phot_det += 1;
      CreateTree::Instance()->lambda_phot_det.push_back(fromEvToNm(theTrack->GetTotalEnergy() / eV));
      CreateTree::Instance()->time_phot_det.push_back(thePrePoint->GetGlobalTime() / picosecond);
      CreateTree::Instance()->angle_phot_det.push_back(cos(G4ThreeVector(1., 0., 0.).angle(theTrackVertexDirection)));
      CreateTree::Instance()->trackID_phot_det.push_back(int(trackID));
    }

  } // optical photon
  
  return;
}
