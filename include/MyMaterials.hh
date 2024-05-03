#ifndef MyMaterials_hh
#define MyMaterials_hh

#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

#include "TString.h"

using namespace CLHEP;



class MyMaterials
{
public:
  MyMaterials();
  ~MyMaterials();
  
  static G4Material* LAr(const G4double& scint_yield = 100);
  static G4Material* PMMA(const G4int& WLSConc = -1);
  static G4Material* pTP();
  
  static G4Material* LYSO_Ce();
  
  static G4Material* Teflon();

  static G4Material* Epoxy();  
  static G4Material* OpticalGrease();
  
  static G4OpticalSurface* VikuitiSurface();
  static G4OpticalSurface* SiPMSurface();

  static G4double fromNmToEv(G4double wavelength);
  static G4double fromEvToNm(G4double energy);
};

#endif
