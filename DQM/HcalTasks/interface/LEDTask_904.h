#ifndef LEDTask_904_h
#define LEDTask_904_h

#include "DQM/HcalCommon/interface/DQTask.h"
#include "DQM/HcalCommon/interface/Utilities.h"
#include "DQM/HcalCommon/interface/ElectronicsMap.h"
#include "DQM/HcalCommon/interface/Container1D.h"
#include "DQM/HcalCommon/interface/ContainerSingleProf1D.h"
#include "DQM/HcalCommon/interface/ContainerProf1D.h"
#include "DQM/HcalCommon/interface/ContainerProf2D.h"

using namespace hcaldqm;
using namespace hcaldqm::filter;
class LEDTask_904 : public DQTask
{
 public:
  LEDTask_904(edm::ParameterSet const&);
  virtual ~LEDTask_904()
    {}
  
  virtual void bookHistograms(DQMStore::IBooker&,
			      edm::Run const&, edm::EventSetup const&);
  
 protected:
  //	funcs
  virtual void _process(edm::Event const&, edm::EventSetup const&);
  virtual void _resetMonitors(UpdateFreq);
  
  //	tags and tokens
  edm::InputTag _tagQIE11;
  edm::EDGetTokenT<QIE11DigiCollection> _tokQIE11;
  
  //	emap
  HcalElectronicsMap const* _emap;
  electronicsmap::ElectronicsMap _ehashmap;

  //    filters
  HashFilter _filter_slot[36];
  
  //	Cuts
  double _cut;
  int    _ped;
  
  //	1D
  ContainerProf1D       _cShapeCut_EChannel[36];

  Container1D		_cSignal_EChannel[36];
  Container1D           _cTiming_EChannel[36];
  
  //	2D Maps 
  Container2D _cOccupancy_Crate;
  ContainerProf2D _cSignal_Crate;
  ContainerProf2D _cTiming_Crate;
  
  Container2D _cOccupancy_CrateSlot;
  ContainerProf2D _cSignal_CrateSlot;
  ContainerProf2D _cTiming_CrateSlot;
  
  //containers overall
  ContainerSingleProf1D   _cShapeCut;
  ContainerSingle1D       _cADC;
  ContainerSingle1D       _cTiming;
  ContainerSingle1D       _cSignal;
  
};

#endif

