#ifndef CreateTree_H
#define CreateTree_H 1

#include <iostream>
#include <vector>
#include "TString.h"

#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

class CreateTree
{
private:
  
  TTree*  ftree ;
  TString fname ;
  
public:
  
  CreateTree (TString name);
  ~CreateTree () ;
  
  TTree*             GetTree() const { return ftree; };
  TString            GetName() const { return fname; };
  void               AddEnergyDeposit(int index, float deposit);
  void               AddScintillationPhoton(int index);
  void               AddCerenkovPhoton(int index);
  int                Fill();
  bool               Write(TFile *);
  void               Clear() ;
  
  static CreateTree* Instance() { return fInstance; } ;
  static CreateTree* fInstance;
  
  int Event;
  
  std::vector<float>* inputMomentum ; // Px Py Pz E
  std::vector<float>* outputMomentum ; // Px Py Pz E
  std::vector<float>* inputInitialPosition ; // x, y, z
  
  float depositedEnergyTotal;
  float depositedEnergyCrystal;
  float depositedEnergyWrapping;
  float depositedEnergyWorld;
  
  int n_phot_sci_LAr;
  std::vector<float> time_phot_sci_LAr;
  std::vector<float> lambda_phot_sci_LAr;
  std::vector<float> angle_phot_sci_LAr;

  int n_phot_wls_primary;
  std::vector<float> time_phot_wls_primary;
  std::vector<float> lambda_phot_wls_primary;
  std::vector<float> angle_phot_wls_primary;

  int n_phot_abs_primary;
  std::vector<float> time_phot_abs_primary;
  std::vector<float> lambda_phot_abs_primary;
  std::vector<float> angle_phot_abs_primary;

  int n_phot_sci_primary;
  std::vector<float> time_phot_sci_primary;
  std::vector<float> lambda_phot_sci_primary;
  std::vector<float> angle_phot_sci_primary;

  int n_phot_wls_lightGuide;
  std::vector<float> time_phot_wls_lightGuide;
  std::vector<float> lambda_phot_wls_lightGuide;
  std::vector<float> angle_phot_wls_lightGuide;

  int n_phot_abs_lightGuide;
  std::vector<float> time_phot_abs_lightGuide;
  std::vector<float> lambda_phot_abs_lightGuide;
  std::vector<float> angle_phot_abs_lightGuide;

  int n_phot_selfabs_lightGuide;
  std::vector<float> time_phot_selfabs_lightGuide;
  std::vector<float> lambda_phot_selfabs_lightGuide;
  std::vector<float> angle_phot_selfabs_lightGuide;

  int n_phot_sci_lightGuide;
  std::vector<float> time_phot_sci_lightGuide;
  std::vector<float> lambda_phot_sci_lightGuide;
  std::vector<float> angle_phot_sci_lightGuide;

  int n_phot_det;
  std::vector<float> time_phot_det;
  std::vector<float> lambda_phot_det;
  std::vector<float> angle_phot_det;
};

#endif
