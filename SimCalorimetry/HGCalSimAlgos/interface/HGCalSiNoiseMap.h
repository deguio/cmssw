#ifndef simcalorimetry_hgcalsimalgos_hgcalsinoisemap
#define simcalorimetry_hgcalsimalgos_hgcalsinoisemap

#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalRadiationMap.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include <string>


/**
   @class HGCalSiNoiseMap
   @short derives from HGCalRadiation map to parse fluence parameters, provides Si-specific functions
*/
class HGCalSiNoiseMap : public HGCalRadiationMap {

 public:
  enum SignalRange_t {q80fC, q160fC, q320fC};
  HGCalSiNoiseMap(); 
  ~HGCalSiNoiseMap() {};
  
  /**
     @short returns the charge collection efficiency and noise
  */
  std::pair<double,double> scaleByFluence(SignalRange_t srange,const HGCSiliconDetId &did,double &radius);

  inline double &getCurrentFluence() { return curFluence_; }
  
 private:

  //
  std::map<HGCSiliconDetId::waferType,double> cellCapacitance_;
  std::map<HGCSiliconDetId::waferType,std::vector<double> > cceParam_;

  //leakage current/volume vs fluence 
  std::vector<double> ileakParam_;
  
  //shaper noise param
  const double encpScale_;
  
  //electron charge in fC
  const double encScale_;
  
  //last fluence computed
  double curFluence_;

  //electronics noise (series+parallel) polynomial coeffs;
  std::map<SignalRange_t,std::vector<double> > encsParam_;
};

#endif
