
#include "DQM/HcalTasks/interface/LEDTask_904.h"

using namespace hcaldqm;
LEDTask_904::LEDTask_904(edm::ParameterSet const& ps):
  DQTask(ps)
{
  //tags
  _tagQIE11 = ps.getUntrackedParameter<edm::InputTag>("tagQIE11",
						      edm::InputTag("hcalDigis"));
  _tokQIE11 = consumes<QIE11DigiCollection>(_tagQIE11);
}

/* virtual */ void LEDTask_904::bookHistograms(DQMStore::IBooker &ib,
					   edm::Run const& r, edm::EventSetup const& es)
{
  if (_ptype==fLocal)
    if (r.runAuxiliary().run()==1)
      return;
  
  DQTask::bookHistograms(ib, r, es);
  
  edm::ESHandle<HcalDbService> dbService;
  es.get<HcalDbRecord>().get(dbService);
  _emap = dbService->getHcalMapping();
  
  std::vector<uint32_t> vhashC36;
  vhashC36.push_back(HcalElectronicsId(63, 1,
				       FIBER_uTCA_MIN1, FIBERCH_MIN, false).rawId());
  _filter_C36.initialize(filter::fPreserver, hashfunctions::fCrate,
			 vhashC36);
  
  //	INITIALIZE
  _cShapeCut_EChannel.initialize(_name,
				 "ShapeCut",hashfunctions::fEChannel,
				 new quantity::ValueQuantity(quantity::fTiming_TS),
				 new quantity::ValueQuantity(quantity::fQIE10fC_300000));
  _cShapeCut.initialize(_name,
			"ShapeCut", 
			new quantity::ValueQuantity(quantity::fTiming_TS),
			new quantity::ValueQuantity(quantity::fQIE10fC_300000));
  
  
  
  //	BOOK
  _cShapeCut_EChannel.book(ib, _emap, _filter_C36, _subsystem);
  _cShapeCut.book(ib, _subsystem);
  
  
  _ehashmap.initialize(_emap, electronicsmap::fD2EHashMap, _filter_C36);
}

/* virtual */ void LEDTask_904::_resetMonitors(UpdateFreq uf)
{
  DQTask::_resetMonitors(uf);
}



/* virtual */ void LEDTask_904::_process(edm::Event const& e,
				     edm::EventSetup const& es)
{
  edm::Handle<QIE11DigiCollection> cqie11;
  if (!e.getByToken(_tokQIE11, cqie11))
    return;
  
  for (uint32_t i=0; i<cqie11->size(); i++)
    {
      QIE11DataFrame frame = static_cast<QIE11DataFrame>((*cqie11)[i]);
      DetId did = frame.detid();
      HcalElectronicsId eid = HcalElectronicsId(_ehashmap.lookup(did));
      
      //compute the signal, ped subracted
      //double q = utilities::aveTS_v10<QIE11DataFrame>(frame,
      //constants::adc2fC[_ped], 0, frame.samples()-1);
      
      //iterate thru all TS and fill
      for (int j=0; j<frame.samples(); j++)
	{
	  //shapes are after the cut
	  _cShapeCut_EChannel.fill(eid, j, adc2fC[frame[j].adc()]);
	  _cShapeCut.fill(eid, j, adc2fC[frame[j].adc()]);
	  
	}
    }
  
}

DEFINE_FWK_MODULE(LEDTask_904);


