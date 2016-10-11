
#include "DQM/HcalTasks/interface/PedestalTask_904.h"

using namespace hcaldqm;
PedestalTask_904::PedestalTask_904(edm::ParameterSet const& ps):
	DQTask(ps)
{
	
	//	tags
	_tagQIE11 = ps.getUntrackedParameter<edm::InputTag>("tagQIE11",
		edm::InputTag("hcalDigis"));
	_tokQIE11 = consumes<QIE11DigiCollection>(_tagQIE11);

	//	cuts
	_cut = ps.getUntrackedParameter<double>("cut", 50.0);
	_ped = ps.getUntrackedParameter<int>("ped", 4);
}
/* virtual */ void PedestalTask_904::bookHistograms(DQMStore::IBooker &ib,
	edm::Run const& r, edm::EventSetup const& es)
{
		if (r.runAuxiliary().run()==1)
			return;

	DQTask::bookHistograms(ib, r, es);

	//	GET WHAT YOU NEED
	edm::ESHandle<HcalDbService> dbs;
	es.get<HcalDbRecord>().get(dbs);
	_emap = dbs->getHcalMapping();

	// create a slot filter and initialize what you need
	unsigned int itr=0;
	for(unsigned int crate=61; crate <= 63; ++crate)
	  for(unsigned int slot=SLOT_uTCA_MIN; slot<=SLOT_uTCA_MAX; ++slot)
	    {

	      std::vector<uint32_t> vhashSlot;
	      vhashSlot.push_back(HcalElectronicsId(crate, slot, FIBER_uTCA_MIN1, FIBERCH_MIN, false).rawId());
	      _filter_slot[itr].initialize(filter::fPreserver, hashfunctions::fCrateSlot, vhashSlot);
	      
	      _cShapeCut_EChannel[itr].initialize(_name,
						  "ShapeCut", hashfunctions::fEChannel,
						  new quantity::ValueQuantity(quantity::fTiming_TS),
						  new quantity::ValueQuantity(quantity::fQIE10fC_300000));

	      _cADC_EChannel[itr].initialize(_name,
					     "ADC", hashfunctions::fEChannel,
					     new quantity::ValueQuantity(quantity::fQIE10ADC_256),
					     new quantity::ValueQuantity(quantity::fN, true));
	      _cTDC_EChannel[itr].initialize(_name,
					     "TDC", hashfunctions::fEChannel,
					     new quantity::ValueQuantity(quantity::fQIE10TDC_64),
					     new quantity::ValueQuantity(quantity::fN, true));
	      
	      ++itr;
	    }
	

	
	_cShapeCut.initialize(_name,
			      "ShapeCut", 
			      new quantity::ValueQuantity(quantity::fTiming_TS),
			      new quantity::ValueQuantity(quantity::fQIE10fC_300000));
	_cTDCvsADC.initialize(_name, "TDCvsADC",
			      new quantity::ValueQuantity(quantity::fQIE10ADC_256),
			      new quantity::ValueQuantity(quantity::fQIE10TDC_64),
			      new quantity::ValueQuantity(quantity::fN, true));
	_cTDC.initialize(_name, "TDC",
			 new quantity::ValueQuantity(quantity::fQIE10TDC_64),
			 new quantity::ValueQuantity(quantity::fN, true));
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


	itr = 0;
	for(unsigned int crate=61; crate <= 63; ++crate)
	  for(unsigned int slot=SLOT_uTCA_MIN; slot<=SLOT_uTCA_MAX; ++slot)
	    {
	      char aux[100];
	      sprintf(aux, "/Crate%d_Slot%d", crate, slot);
	      _cShapeCut_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem, aux);
	      _cTDC_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem, aux);
	      _cADC_EChannel[itr].book(ib, _emap, _filter_slot[itr], _subsystem, aux);

	      ++itr;
	    }
	
	_cShapeCut.book(ib, _subsystem);

	_cTDCvsADC.book(ib, _subsystem);
	_cTDC.book(ib, _subsystem);

	_cADC.book(ib, _subsystem);

	_cOccupancy_Crate.book(ib, _emap, _subsystem);
	_cOccupancy_CrateSlot.book(ib, _emap, _subsystem);

	_ehashmap.initialize(_emap, electronicsmap::fD2EHashMap);
}

/* virtual */ void PedestalTask_904::endLuminosityBlock(edm::LuminosityBlock const& lb,
	edm::EventSetup const& es)
{
	
	//	finish
	DQTask::endLuminosityBlock(lb, es);
}

/* virtual */ void PedestalTask_904::_process(edm::Event const& e, 
	edm::EventSetup const&)
{

	edm::Handle<QIE11DigiCollection> cqie10;
	if (!e.getByToken(_tokQIE11, cqie10))
		std::cout << "Collection isn't available" << std::endl;

	for (uint32_t i=0; i<cqie10->size(); i++)
	{
		QIE11DataFrame frame = static_cast<QIE11DataFrame>((*cqie10)[i]);
		DetId did = frame.detid();
		HcalElectronicsId eid = HcalElectronicsId(_ehashmap.lookup(did));
		_cOccupancy_Crate.fill(eid);
		_cOccupancy_CrateSlot.fill(eid);

		//	iterate thru all TS and fill
		for (int j=0; j<frame.samples(); j++)
		  {
		        //	shapes are after the cut
		        int index = (eid.crateId() - 61)*12+eid.slot()-1;
			_cShapeCut_EChannel[index].fill(eid, j, adc2fC[frame[j].adc()]);

			_cShapeCut.fill(eid, j, adc2fC[frame[j].adc()]);

			//	w/o a cut
			_cTDCvsADC.fill(eid, frame[j].adc(), frame[j].tdc());

			_cTDC_EChannel[index].fill(eid, frame[j].tdc());
			_cTDC.fill(eid, frame[j].tdc());

			_cADC_EChannel[index].fill(eid, frame[j].adc());
			_cADC.fill(eid, frame[j].adc());

		}
	}
}

/* virtual */ void PedestalTask_904::_resetMonitors(UpdateFreq)
{
}

DEFINE_FWK_MODULE(PedestalTask_904);
