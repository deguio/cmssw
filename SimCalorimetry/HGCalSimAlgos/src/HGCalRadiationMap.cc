#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalRadiationMap.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include <fstream>

//
HGCalRadiationMap::HGCalRadiationMap() :
  grayToKrad_(0.1), 
  refEdge_(3),
  verbose_(false)
{
}

//
void HGCalRadiationMap::setDoseMap(const std::string& fullpath)
{
  doseMap_ = readDosePars(fullpath);
}

//
void HGCalRadiationMap::setGeometry(const CaloSubdetectorGeometry* geom)
{
  hgcalGeom_ = static_cast<const HGCalGeometry*>(geom);
}

//
std::map<int, HGCalRadiationMap::DoseParameters> HGCalRadiationMap::readDosePars(const std::string& fullpath)
{
  std::map<int, DoseParameters> result;

  //no dose file means no aging
  if(fullpath.empty())
    return result;

  edm::FileInPath fp(fullpath);
  std::ifstream infile(fp.fullPath());
  if(!infile.is_open())
  {
    throw cms::Exception("FileNotFound") << "Unable to open '" << fullpath << "'" << std::endl;
  }
  std::string line;
  while(getline(infile,line))
  {
    int layer;
    DoseParameters dosePars;

    //space-separated
    std::stringstream linestream(line);
    linestream >> layer >> dosePars.a_ >>  dosePars.b_ >> dosePars.c_ >> dosePars.d_ >> dosePars.e_ >> dosePars.f_ >> dosePars.g_ >> dosePars.h_ >> dosePars.i_ >> dosePars.j_;

    result[layer] = dosePars;
  }
  return result;
}

//
double HGCalRadiationMap::getDoseValue(const int layer, const std::array<double, 8>& radius)
{
  double cellDose = std::pow(10, doseMap_[layer].a_ + doseMap_[layer].b_*radius[4] + doseMap_[layer].c_*radius[5] + doseMap_[layer].d_*radius[6] + doseMap_[layer].e_*radius[7]); //dose in grey
  return cellDose * grayToKrad_; //convert to kRad
}

//
double HGCalRadiationMap::getFluenceValue(const int layer, const std::array<double, 8>& radius)
{
  double cellFluence = std::pow(10, doseMap_[layer].f_ + doseMap_[layer].g_*radius[0] + doseMap_[layer].h_*radius[1] + doseMap_[layer].i_*radius[2] + doseMap_[layer].j_*radius[3]); //dose in grey
  return cellFluence;
}
//
std::pair<double, double> HGCalRadiationMap::scaleByDose(const HGCScintillatorDetId& cellId,  const std::array<double, 8>& radius)
{
  if(doseMap_.empty())
    return std::make_pair(1., 0.);

  int layer = cellId.layer();
  double cellDose = getDoseValue(layer, radius); //in kRad
  constexpr double expofactor = 1./199.6;
  double scaleFactor = std::exp( -std::pow(cellDose, 0.65) * expofactor );

  double cellFluence = getFluenceValue(layer, radius); //in 1-Mev-equivalent neutrons per cm2

  constexpr double factor = 2. / (2*1e13); //SiPM area = 2mm^2
  double noise = 2.18 * sqrt(cellFluence * factor);

  if(verbose_)
  {
    LogDebug("HGCalRadiationMap") << "HGCalRadiationMap::scaleByDose - Dose, scaleFactor, fluence, noise: "
                                      << cellDose << " " << scaleFactor << " "
                                      << cellFluence << " " << noise;

    LogDebug("HGCalRadiationMap") << "HGCalRadiationMap::setDoseMap - layer, a, b, c, d, e, f: "
                                      << layer << " "
                                      << doseMap_[layer].a_ << " "
                                      << doseMap_[layer].b_ << " "
                                      << doseMap_[layer].c_ << " "
                                      << doseMap_[layer].d_ << " "
                                      << doseMap_[layer].e_ << " "
                                      << doseMap_[layer].f_;
  }

  return std::make_pair(scaleFactor, noise);
}

double HGCalRadiationMap::scaleByArea(const HGCScintillatorDetId& cellId, const std::array<double, 8>& radius)
{
  double circ = 2 * M_PI * radius[0];

  double edge(refEdge_);
  if(cellId.type() == 0)
  {
    constexpr double factor = 1./360.;
    edge = circ * factor; //1 degree
  }
  else
  {
    constexpr double factor = 1./288.;
    edge = circ * factor; //1.25 degrees
  }

  double scaleFactor = refEdge_ / edge;  //assume reference 3cm of edge

  if(verbose_)
  {
    LogDebug("HGCalRadiationMap") << "HGCalRadiationMap::scaleByArea - Type, layer, edge, radius, SF: "
                                      << cellId.type() << " "
                                      << cellId.layer() << " "
                                      << edge << " "
                                      << radius[0] << " "
                                      << scaleFactor << std::endl;
  }

  return scaleFactor;
}

std::array<double, 8> HGCalRadiationMap::computeRadius(const HGCScintillatorDetId& cellId)
{
  GlobalPoint global = hgcalGeom_->getPosition(cellId);

  double radius2 = std::pow(global.x(), 2) + std::pow(global.y(), 2); //in cm
  double radius4 = std::pow(radius2, 2);
  double radius = sqrt(radius2);
  double radius3 = std::pow(radius, 3);

  double radius_m100 = radius-100;
  double radius_m100_2 = std::pow(radius_m100, 2);
  double radius_m100_3 = std::pow(radius_m100, 3);
  double radius_m100_4 = std::pow(radius_m100_2, 2);

  std::array<double, 8> radii { {radius, radius2, radius3, radius4, radius_m100, radius_m100_2, radius_m100_3, radius_m100_4} };
  return radii;
}
