
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
  
  //	INITIALIZE
  // create a slot filter and initialize what you need
  unsigned int itr=0;
  for(unsigned int crate=61; crate <= 63; ++crate)
    for(unsigned int slot=SLOT_uTCA_MIN; slot<=SLOT_uTCA_MAX; ++slot)
      {
	std::vector<uint32_t> vhashSlot;
	vhashSlot.push_back(HcalElectronicsId(crate, slot, FIBER_uTCA_MIN1, FIBERCH_MIN, false).rawId());
	_filter_slot[itr].initialize(filter::fPreserver, hashfunctions::fCrateSlot, vhashSlot);

	_cShapeCut_EChannel[itr].initialize(_name,
				       "ShapeCut",hashfunctions::fEChannel,
				       new quantity::ValueQuantity(quantity::fTiming_TS),
				       new quantity::ValueQuantity(quantity::fQIE10fC_300000));

	_cSignal_EChannel[itr].initialize(_name,
				     "Signal",hashfunctions::fEChannel,
				     new quantity::ValueQuantity(quantity::ffC_3000),
				     new quantity::ValueQuantity(quantity::fN, true));
	
	_cTiming_EChannel[itr].initialize(_name,
				     "Timing",hashfunctions::fEChannel,
				     new quantity::ValueQuantity(quantity::fTiming_TS200),
				     new quantity::ValueQuantity(quantity::fN, true));

	++itr;

      }


  _cShapeCut.initialize(_name,
			"ShapeCut", 
			new quantity::ValueQuantity(quantity::fTiming_TS),
			new quantity::ValueQuantity(quantity::fQIE10fC_300000));

  //ADC
  _cADC.initialize(_name, "ADC",
		   new quantity::ValueQuantity(quantity::fQIE10ADC_256),
		   new quantity::ValueQuantity(quantity::fN, true));


  //OCCUPANCY
  _cOccupancy_Crate.initialize(_name, 
	"Occupancy", hashfunctions::fCrate,
	new quantity::ElectronicsQuantity(quantity::fSlotuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberuTCAFiberCh),
	new quantity::ValueQuantity(quantity::fN));
  _cOccupancy_CrateSlot.initialize(_name, 
	"Occupancy", hashfunctions::fCrateSlot,
	new quantity::ElectronicsQuantity(quantity::fFiberuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberCh),
	new quantity::ValueQuantity(quantity::fN));

  //SIGNAL
  _cSignal.initialize(_name,
        "Signal",
        new quantity::ValueQuantity(quantity::ffC_3000),
        new quantity::ValueQuantity(quantity::fN, true));
  _cSignal_Crate.initialize(_name, 
	"Signal", hashfunctions::fCrate,
	new quantity::ElectronicsQuantity(quantity::fSlotuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberuTCAFiberCh),
	new quantity::ValueQuantity(quantity::fQIE10fC_300000));
  _cSignal_CrateSlot.initialize(_name, 
	"Signal", hashfunctions::fCrateSlot,
	new quantity::ElectronicsQuantity(quantity::fFiberuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberCh),
	new quantity::ValueQuantity(quantity::fQIE10fC_300000));




  //TIMING
  _cTiming.initialize(_name,
        "Timing",
	new quantity::ValueQuantity(quantity::fTiming_TS200),
	new quantity::ValueQuantity(quantity::fN, true));
  _cTiming_Crate.initialize(_name, 
	"Timing", hashfunctions::fCrate,
	new quantity::ElectronicsQuantity(quantity::fSlotuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberuTCAFiberCh),
	new quantity::ValueQuantity(quantity::fTiming_TS200));
  _cTiming_CrateSlot.initialize(_name, 
	"Timing", hashfunctions::fCrateSlot,
	new quantity::ElectronicsQuantity(quantity::fFiberuTCA),
	new quantity::ElectronicsQuantity(quantity::fFiberCh),
	new quantity::ValueQuantity(quantity::fTiming_TS200));
  
  
  //	BOOK

  itr = 0;
  for(unsigned int crate=61; crate <= 63; ++crate)
    for(unsigned int slot=SLOT_uTCA_MIN; slot<=SLOT_uTCA_MAX; ++slot)
      {
	char aux[100];
	sprintf(aux, "/Crate%d_Slot%d", crate, slot);

	_cShapeCut_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem ,aux);
	_cTiming_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem, aux);
	_cSignal_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem, aux);

	++itr;
      }
  _cSignal.book(ib, _subsystem);
  _cOccupancy_Crate.book(ib, _emap, _subsystem);
  _cOccupancy_CrateSlot.book(ib, _emap, _subsystem);
  _cSignal_Crate.book(ib, _emap, _subsystem);
  _cTiming_Crate.book(ib, _emap, _subsystem);
  _cSignal_CrateSlot.book(ib, _emap, _subsystem);
  _cTiming_CrateSlot.book(ib, _emap, _subsystem);
  _cShapeCut.book(ib, _subsystem);
  _cTiming.book(ib, _subsystem);
  _cADC.book(ib, _subsystem);
  
  _ehashmap.initialize(_emap, electronicsmap::fD2EHashMap);
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
      _cOccupancy_Crate.fill(eid);
      _cOccupancy_CrateSlot.fill(eid);


      
      //iterate thru all TS and fill      
      double sumQ = 0;
      double onlinePed = (adc2fC[frame[0].adc()]+adc2fC[frame[1].adc()])/2.;
      int index = (eid.crateId() - 61)*12+eid.slot()-1;

      for (int j=0; j<frame.samples(); j++)
	{
	  //shapes are after the cut
	  _cShapeCut_EChannel[index].fill(eid, j, adc2fC[frame[j].adc()]);
	  _cShapeCut.fill(eid, j, adc2fC[frame[j].adc()]);

	  _cADC.fill(eid, frame[j].adc());
	  
	  //pedestal subtracted mean charge
	  sumQ += adc2fC[frame[j].adc()] - onlinePed;
	}
      
      double timing = utilities::aveTS_v10<QIE11DataFrame>(frame, 20, 0, frame.samples()-1);

      _cSignal.fill(eid, sumQ); 
      _cSignal_Crate.fill(eid, sumQ);
      _cSignal_CrateSlot.fill(eid, sumQ);
      _cSignal_EChannel[index].fill(eid, sumQ); 

      _cTiming.fill(eid, timing);
      _cTiming_Crate.fill(eid, timing);
      _cTiming_CrateSlot.fill(eid, timing);
      _cTiming_EChannel[index].fill(eid, timing);
      
      
    }
  
}

DEFINE_FWK_MODULE(LEDTask_904);


