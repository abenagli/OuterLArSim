#include "CreateTree.hh"
//#include <algorithm>

using namespace std ;

CreateTree* CreateTree::fInstance = NULL ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::CreateTree (TString name)
{
  if( fInstance )
  {
    return ;
  }
  
  this -> fInstance = this ;
  this -> fname     = name ;
  this -> ftree     = new TTree (name,name) ;
  
  this -> GetTree ()->Branch ("Event", &this->Event, "Event/I") ;
  
  inputInitialPosition = new vector<float>(3,0.); 
  inputMomentum =        new vector<float>(4,0.);
  outputMomentum =       new vector<float>(4,0.);   
  this -> GetTree() -> Branch("inputInitialPosition", "vector<float>", &inputInitialPosition);
  this -> GetTree() -> Branch("inputMomentum",        "vector<float>", &inputMomentum);
  this -> GetTree() -> Branch("outputMomentum",       "vector<float>", &outputMomentum);
  
  this -> GetTree() -> Branch("depositedEnergyTotal",    &this->depositedEnergyTotal,       "depositedEnergyTotal/F");
  this -> GetTree() -> Branch("depositedEnergyCrystal",  &this->depositedEnergyCrystal,   "depositedEnergyCrystal/F");
  this -> GetTree() -> Branch("depositedEnergyWrapping", &this->depositedEnergyWrapping, "depositedEnergyWrapping/F");
  this -> GetTree() -> Branch("depositedEnergyWorld",    &this->depositedEnergyWorld,       "depositedEnergyWorld/F");
  
  this -> GetTree() -> Branch("n_phot_sci_LAr", &this->n_phot_sci_LAr, "n_phot_sci_LAr/I");
  this -> GetTree() -> Branch("time_phot_sci_LAr",   &time_phot_sci_LAr);
  this -> GetTree() -> Branch("lambda_phot_sci_LAr", &lambda_phot_sci_LAr);
  this -> GetTree() -> Branch("angle_phot_sci_LAr", &angle_phot_sci_LAr);
  
  this -> GetTree() -> Branch("n_phot_wls_primary", &this->n_phot_wls_primary, "n_phot_wls_primary/I");
  this -> GetTree() -> Branch("time_phot_wls_primary",   &time_phot_wls_primary);
  this -> GetTree() -> Branch("lambda_phot_wls_primary", &lambda_phot_wls_primary);
  this -> GetTree() -> Branch("angle_phot_wls_primary", &angle_phot_wls_primary);

  this -> GetTree() -> Branch("n_phot_abs_primary", &this->n_phot_abs_primary, "n_phot_abs_primary/I");
  this -> GetTree() -> Branch("time_phot_abs_primary",   &time_phot_abs_primary);
  this -> GetTree() -> Branch("lambda_phot_abs_primary", &lambda_phot_abs_primary);
  this -> GetTree() -> Branch("angle_phot_abs_primary", &angle_phot_abs_primary);

  this -> GetTree() -> Branch("n_phot_sci_primary", &this->n_phot_sci_primary, "n_phot_sci_primary/I");
  this -> GetTree() -> Branch("time_phot_sci_primary",   &time_phot_sci_primary);
  this -> GetTree() -> Branch("lambda_phot_sci_primary", &lambda_phot_sci_primary);
  this -> GetTree() -> Branch("angle_phot_sci_primary", &angle_phot_sci_primary);

  this -> GetTree() -> Branch("n_phot_wls_lightGuide", &this->n_phot_wls_lightGuide, "n_phot_wls_lightGuide/I");
  this -> GetTree() -> Branch("time_phot_wls_lightGuide",   &time_phot_wls_lightGuide);
  this -> GetTree() -> Branch("lambda_phot_wls_lightGuide", &lambda_phot_wls_lightGuide);
  this -> GetTree() -> Branch("angle_phot_wls_lightGuide", &angle_phot_wls_lightGuide);
  
  this -> GetTree() -> Branch("n_phot_abs_lightGuide", &this->n_phot_abs_lightGuide, "n_phot_abs_lightGuide/I");
  this -> GetTree() -> Branch("time_phot_abs_lightGuide",   &time_phot_abs_lightGuide);
  this -> GetTree() -> Branch("lambda_phot_abs_lightGuide", &lambda_phot_abs_lightGuide);
  this -> GetTree() -> Branch("angle_phot_abs_lightGuide", &angle_phot_abs_lightGuide);

  this -> GetTree() -> Branch("n_phot_selfwls_lightGuide", &this->n_phot_selfwls_lightGuide, "n_phot_selfwls_lightGuide/I");
  this -> GetTree() -> Branch("time_phot_selfwls_lightGuide",   &time_phot_selfwls_lightGuide);
  this -> GetTree() -> Branch("lambda_phot_selfwls_lightGuide", &lambda_phot_selfwls_lightGuide);
  this -> GetTree() -> Branch("angle_phot_selfwls_lightGuide", &angle_phot_selfwls_lightGuide);

  this -> GetTree() -> Branch("n_phot_selfabs_lightGuide", &this->n_phot_selfabs_lightGuide, "n_phot_selfabs_lightGuide/I");
  this -> GetTree() -> Branch("time_phot_selfabs_lightGuide",   &time_phot_selfabs_lightGuide);
  this -> GetTree() -> Branch("lambda_phot_selfabs_lightGuide", &lambda_phot_selfabs_lightGuide);
  this -> GetTree() -> Branch("angle_phot_selfabs_lightGuide", &angle_phot_abs_lightGuide);

  this -> GetTree() -> Branch("n_phot_sci_lightGuide", &this->n_phot_sci_lightGuide, "n_phot_sci_lightGuide/I");
  this -> GetTree() -> Branch("time_phot_sci_lightGuide",   &time_phot_sci_lightGuide);
  this -> GetTree() -> Branch("lambda_phot_sci_lightGuide", &lambda_phot_sci_lightGuide);
  this -> GetTree() -> Branch("angle_phot_sci_lightGuide", &angle_phot_sci_lightGuide);
  
  this -> GetTree() -> Branch("n_phot_det", &this->n_phot_det, "n_phot_det/I");
  this -> GetTree() -> Branch("time_phot_det",   &time_phot_det);
  this -> GetTree() -> Branch("lambda_phot_det", &lambda_phot_det);
  this -> GetTree() -> Branch("angle_phot_det", &angle_phot_det);
  this -> GetTree() -> Branch("prodX_phot_det", &prodX_phot_det);
  this -> GetTree() -> Branch("prodY_phot_det", &prodY_phot_det);
  this -> GetTree() -> Branch("prodZ_phot_det", &prodZ_phot_det);
  this -> GetTree() -> Branch("trackLength_phot_det", &trackLength_phot_det);

  this -> Clear() ;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



CreateTree::~CreateTree()
{}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



int CreateTree::Fill() 
{ 
  return this -> GetTree() -> Fill(); 
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



bool CreateTree::Write(TFile * outfile)
{
  outfile -> cd();
  
  int bytes = outfile -> Write();
  std::cout << "============================================"  << std::endl;
  std::cout << "nr of  B written:  " << int(bytes)             << std::endl;
  std::cout << "nr of KB written:  " << int(bytes/1024.)       << std::endl;
  std::cout << "nr of MB written:  " << int(bytes/1024./1024.) << std::endl;
  std::cout << "============================================"  << std::endl;
  
  outfile -> Close();
  
  return true ;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



void CreateTree::Clear()
{
  Event	= 0;
  
  for(int i = 0; i < 3; ++i) 
  {
    inputInitialPosition -> at(i) = 0.;
  }
  for(int i = 0; i < 4; ++i) 
  {
    inputMomentum ->at(i) = 0.;
    outputMomentum ->at(i) = 0.;
  }
  
  depositedEnergyTotal = 0.;
  depositedEnergyCrystal = 0.;
  depositedEnergyWrapping = 0.;
  depositedEnergyWorld = 0.;
  
  prodPositionMap_phot.clear();
  
  n_phot_sci_LAr = 0;
  time_phot_sci_LAr.clear();
  lambda_phot_sci_LAr.clear();
  angle_phot_sci_LAr.clear();
  
  n_phot_wls_primary = 0;
  time_phot_wls_primary.clear();
  lambda_phot_wls_primary.clear();
  angle_phot_wls_primary.clear();
  
  n_phot_abs_primary = 0;
  time_phot_abs_primary.clear();
  lambda_phot_abs_primary.clear();
  angle_phot_abs_primary.clear();
  
  n_phot_sci_primary = 0;
  time_phot_sci_primary.clear();
  lambda_phot_sci_primary.clear();
  angle_phot_sci_primary.clear();

  n_phot_wls_lightGuide = 0;
  time_phot_wls_lightGuide.clear();
  lambda_phot_wls_lightGuide.clear();
  angle_phot_wls_lightGuide.clear();
  
  n_phot_abs_lightGuide = 0;
  time_phot_abs_lightGuide.clear();
  lambda_phot_abs_lightGuide.clear();
  angle_phot_abs_lightGuide.clear();

  n_phot_selfwls_lightGuide = 0;
  time_phot_selfwls_lightGuide.clear();
  lambda_phot_selfwls_lightGuide.clear();
  angle_phot_selfwls_lightGuide.clear();

  n_phot_selfabs_lightGuide = 0;
  time_phot_selfabs_lightGuide.clear();
  lambda_phot_selfabs_lightGuide.clear();
  angle_phot_selfabs_lightGuide.clear();

  n_phot_sci_lightGuide = 0;
  time_phot_sci_lightGuide.clear();
  lambda_phot_sci_lightGuide.clear();
  angle_phot_sci_lightGuide.clear();

  n_phot_det = 0;
  time_phot_det.clear();
  lambda_phot_det.clear();
  angle_phot_det.clear();
  trackID_phot_det.clear();
  prodX_phot_det.clear();
  prodY_phot_det.clear();
  prodZ_phot_det.clear(); 
  trackLength_phot_det.clear();
  trackLengthMap_phot_det.clear();
}
