#ifndef LEDTask_904_h
#define LEDTask_904_h

/*
 *	file:			LEDTask_904.h
 *	Author:			Viktor Khristenko
 *	Date:			16.10.2015
 */

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
  HashFilter _filter_C36;
  
  //	Cuts
  double _cut;
  int _ped;
  
  
  //	1D
  ContainerProf1D         _cShapeCut_EChannel;
  
  Container1D		_cSignalMean_EChannel;
  Container1D		_cTimingMean_EChannel;
  
  
  
  //containers overall
  ContainerSingleProf1D   _cShapeCut;
  
  ContainerSingle1D       _cSignalMean;
  ContainerSingle1D       _cTimingMean;
  
};

#endif

