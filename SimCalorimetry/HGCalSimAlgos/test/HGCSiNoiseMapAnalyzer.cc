// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

// Geometry
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/HGCalCommonData/interface/HGCalDDDConstants.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"

#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalSiNoiseMap.h"


#include <TH1F.h>
#include <TH2F.h>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//STL headers
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>

#include "vdt/vdtMath.h"

using namespace std;

//
// class declaration
//
class HGCSiNoiseMapAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {

public:

  explicit HGCSiNoiseMapAnalyzer(const edm::ParameterSet&);
  ~HGCSiNoiseMapAnalyzer() override;
  
private:
  void beginJob() override {}
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override {}
  
  // ----------member data ---------------------------
  edm::Service<TFileService> fs_;
  std::map<DetId::Detector, HGCalSiNoiseMap *> noiseMaps_;
  std::map< std::pair<DetId::Detector,int>, TH1F *> layerN_, layerCCE_, layerNoise_, layerSN_, layerF_;
  std::map< DetId::Detector, TH2F *> detN_, detCCE_, detNoise_, detSN_, detF_;
};

//
HGCSiNoiseMapAnalyzer::HGCSiNoiseMapAnalyzer(const edm::ParameterSet& iConfig)
{
  usesResource("TFileService");
  fs_->file().cd();

  //configure the dose map
  std::string doseMapURL( iConfig.getParameter<std::string>("doseMap") );
  noiseMaps_[DetId::HGCalEE]=new HGCalSiNoiseMap;
  noiseMaps_[DetId::HGCalEE]->setDoseMap( doseMapURL );
  noiseMaps_[DetId::HGCalHSi]=new HGCalSiNoiseMap;
  noiseMaps_[DetId::HGCalHSi]->setDoseMap( doseMapURL );
}

//
HGCSiNoiseMapAnalyzer::~HGCSiNoiseMapAnalyzer()
{
}

//
void HGCSiNoiseMapAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& es)
{
  //get geometry
  edm::ESHandle<CaloGeometry> geom;
  es.get<CaloGeometryRecord>().get(geom);

  std::vector<DetId::Detector> dets={DetId::HGCalEE,DetId::HGCalHSi};
  for(auto &d:dets) {
    noiseMaps_[d]->setGeometry( geom->getSubdetectorGeometry(d, ForwardSubdetector::ForwardEmpty) );

    const std::vector<DetId> &detIdVec = noiseMaps_[d]->geom()->getValidDetIds();
    cout << "Subdetector:" << d << " has " << detIdVec.size() << " valid cells" << endl;

    std::map<int,float> layer_z;
    std::map<int,float> layer_minR,layer_maxR;
    for(const auto &cellId : detIdVec)
      {
        HGCSiliconDetId id(cellId.rawId());
        int layer = std::abs(id.layer());
        GlobalPoint pt = noiseMaps_[d]->geom()->getPosition(id);
        float z(pt.z());
        layer_z[layer]=fabs(z);
        float r(pt.perp());
        if(layer_minR.find(layer)==layer_minR.end()){
          layer_minR[layer]=r;
          layer_maxR[layer]=r;
        }else{
          layer_minR[layer]=min(layer_minR[layer],r);
          layer_maxR[layer]=max(layer_maxR[layer],r);
        }
      }


    //start histos
    TString baseName(Form("d%d_",d));
    TString title(d==DetId::HGCalEE ? "CEE" : "CEH_{Si}");    
    float minR(99999.),maxR(0.);
    for(std::map<int,float>::iterator it=layer_z.begin(); it!=layer_z.end(); it++){
      
      int layer(it->first);
      cout << "\t" << layer << " z=" << it->second << " " << layer_minR[layer] << "<r<" << layer_maxR[layer] << endl;

      //this layer histos
      std::pair<DetId::Detector,int> key(d,layer);
      TString layerBaseName(Form("%slayer%d_",baseName.Data(),layer));
      TString layerTitle(Form("%s %d",title.Data(),layer));
      layerTitle+= ";Radius [cm];";
      Int_t nbins(100);
      Float_t layer_rmin(layer_minR[layer]-10),layer_rmax(layer_maxR[layer]+10);
      layerN_[key]     = fs_->make<TH1F>(layerBaseName+"ncells",  layerTitle+"Cells",               nbins, layer_rmin, layer_rmax);
      layerCCE_[key]   = fs_->make<TH1F>(layerBaseName+"cce",     layerTitle+"<CCE>",               nbins, layer_rmin, layer_rmax);                     
      layerNoise_[key] = fs_->make<TH1F>(layerBaseName+"noise",   layerTitle+"<Noise>",             nbins, layer_rmin, layer_rmax);
      layerSN_[key]    = fs_->make<TH1F>(layerBaseName+"sn",      layerTitle+"<S/N>",               nbins, layer_rmin, layer_rmax);
      layerF_[key]     = fs_->make<TH1F>(layerBaseName+"fluence", layerTitle+"<F> [n_{eq}/cm^{2}]", nbins, layer_rmin, layer_rmax);
      minR=min(layer_rmin,minR);
      maxR=max(layer_rmax,maxR);
    }

    //sub-detector histos
    title+=";Layer;Radius [cm];";
    Int_t nbinsX(layer_z.size()),nbinsY(100);
    detN_[d]     = fs_->make<TH2F>(baseName+"ncells",  title+"Cells",               nbinsX,1,nbinsX+1, nbinsY,minR,maxR);
    detCCE_[d]   = fs_->make<TH2F>(baseName+"cce",     title+"<CCE>",               nbinsX,1,nbinsX+1, nbinsY,minR,maxR);
    detNoise_[d] = fs_->make<TH2F>(baseName+"noise",   title+"<Noise>",             nbinsX,1,nbinsX+1, nbinsY,minR,maxR);
    detSN_[d]    = fs_->make<TH2F>(baseName+"sn",      title+"<S/N>",               nbinsX,1,nbinsX+1, nbinsY,minR,maxR);
    detF_[d]     = fs_->make<TH2F>(baseName+"fluence", title+"<F> [n_{eq}/cm^{2}]", nbinsX,1,nbinsX+1, nbinsY,minR,maxR);
                    
    //fill histos
    for(const auto &cellId : detIdVec)
      {
        HGCSiliconDetId id(cellId.rawId());
        int layer = std::abs(id.layer());
        GlobalPoint pt = noiseMaps_[d]->geom()->getPosition(id);
        double r(pt.perp());
        
        std::pair<double,double> cceNoise( noiseMaps_[d]->scaleByFluence(HGCalSiNoiseMap::q80fC,id,r) );

        detN_[d]->Fill(layer,r,1);
        detCCE_[d]->Fill(layer,r,cceNoise.first);
        detNoise_[d]->Fill(layer,r,cceNoise.second);
        detF_[d]->Fill(layer,r,noiseMaps_[d]->getCurrentFluence());

        std::pair<DetId::Detector,int> key(d,layer);
        layerN_[key]->Fill(r,1);        
        layerCCE_[key]->Fill(r,cceNoise.first);
        layerNoise_[key]->Fill(r,cceNoise.second);
        layerF_[key]->Fill(r,noiseMaps_[d]->getCurrentFluence());
      }    
  

    //normalize histos per cell counts
    detF_[d]->Divide(detN_[d]);
    detCCE_[d]->Divide(detN_[d]);
    detNoise_[d]->Divide(detN_[d]);
    for(std::map<int,float>::iterator it=layer_z.begin(); it!=layer_z.end(); it++){
      std::pair<DetId::Detector,int> key(d,it->first);
      layerF_[key]->Divide(layerN_[key]);
      layerCCE_[key]->Divide(layerN_[key]);
      layerNoise_[key]->Divide(layerN_[key]);
    }
  }
  


}

//define this as a plug-in
DEFINE_FWK_MODULE(HGCSiNoiseMapAnalyzer);
