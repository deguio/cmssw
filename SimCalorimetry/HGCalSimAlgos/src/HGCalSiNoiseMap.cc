#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalSiNoiseMap.h"

//
HGCalSiNoiseMap::HGCalSiNoiseMap() :
  encpScale_(840.),
  encScale_(1.60217646E-4)
{
  encsParam_[q80fC]  = {636.,  15.6, 0.0328};
  encsParam_[q160fC] = {1045., 8.74, 0.0685};
  encsParam_[q320fC] = {1915., 2.79, 0.0878};

  ileakParam_ = {0.993,-42.668};

  cellCapacitance_[HGCSiliconDetId::waferType::HGCalFine]=50;
  cellCapacitance_[HGCSiliconDetId::waferType::HGCalCoarseThin]=65;
  cellCapacitance_[HGCSiliconDetId::waferType::HGCalCoarseThick]=45;

  cceParam_[HGCSiliconDetId::waferType::HGCalFine]={22,-0.3546};
  cceParam_[HGCSiliconDetId::waferType::HGCalCoarseThin]={15,-0.2538};
  cceParam_[HGCSiliconDetId::waferType::HGCalCoarseThick]={9,-0.1096};
}

//
std::pair<double,double> HGCalSiNoiseMap::scaleByDose(SignalRange_t srange,const HGCSiliconDetId &cellId,const std::array<double, 8>&radius) {

  std::pair<double,double> cceNoise(1.,0.);
  
  //decode cell properties
  int layer(cellId.layer());
  HGCSiliconDetId::waferType cellThick( HGCSiliconDetId::waferType(cellId.type()) );
  double cellCap(cellCapacitance_[cellThick]);
  double cellThickVal( cellThick==HGCSiliconDetId::waferType::HGCalFine? 120.e-6:
                       cellThick==HGCSiliconDetId::waferType::HGCalCoarseThin ? 200.e-6:
                       300.e-6);
  double cellArea(geom()->getArea(cellId));
  double cellVol(cellArea*cellThickVal );

  //get fluence
  if(getDoseMap().empty()) return cceNoise;
  double cellFluence( getFluenceValue(layer,radius) );


  //leakage current
  double ileak((ileakParam_[0]*log(cellFluence)+ileakParam_[1])*cellVol);
  
  //charge collection efficiency
  cceNoise.first=(cceParam_[cellThick][1]*log(cellFluence)/cceParam_[cellThick][0]-1.);

  //build noise estimate
  double enc_p(encpScale_*sqrt(ileak));
  double enc_s(encsParam_[srange][0]+encsParam_[srange][1]*cellCap+encsParam_[srange][2]*pow(cellCap,2));
  double enc(hypot(enc_p,enc_s)*encScale_);
  cceNoise.second=enc;

  return cceNoise;
}
