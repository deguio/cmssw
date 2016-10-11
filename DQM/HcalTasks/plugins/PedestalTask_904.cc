
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
	std::vector<uint32_t> vhashC36;
	vhashC36.push_back(HcalElectronicsId(63, 1, FIBER_uTCA_MIN1, FIBERCH_MIN, false).rawId());

	_filter_C36.initialize(filter::fPreserver, hashfunctions::fCrate, vhashC36);

	//	INITIALIZE what you need
	_cShapeCut_EChannel.initialize(_name,
		"ShapeCut", hashfunctions::fEChannel,
		new quantity::ValueQuantity(quantity::fTiming_TS),
		new quantity::ValueQuantity(quantity::fQIE10fC_300000));
	_cShapeCut.initialize(_name,
		"ShapeCut", 
		new quantity::ValueQuantity(quantity::fTiming_TS),
		new quantity::ValueQuantity(quantity::fQIE10fC_300000));
	// _cTDCvsADC.initialize(_name, "TDCvsADC",
	// 	new quantity::ValueQuantity(quantity::fQIE10ADC_256),
	// 	new quantity::ValueQuantity(quantity::fQIE10TDC_64),
	// 	new quantity::ValueQuantity(quantity::fN, true));
	_cTDC.initialize(_name, "TDC",
		new quantity::ValueQuantity(quantity::fQIE10TDC_64),
		new quantity::ValueQuantity(quantity::fN, true));
	_cADC.initialize(_name, "ADC",
		new quantity::ValueQuantity(quantity::fQIE10ADC_256),
		new quantity::ValueQuantity(quantity::fN, true));
	for (unsigned int j=0; j<10; j++)
	{
		// _cTDCvsADC_EChannel[j].initialize(_name,
		// 	"TDCvsADC", hashfunctions::fEChannel,
		// 	new quantity::ValueQuantity(quantity::fQIE10ADC_256),
		// 	new quantity::ValueQuantity(quantity::fQIE10TDC_64),
		// 	new quantity::ValueQuantity(quantity::fN, true));
		_cADC_EChannel[j].initialize(_name,
			"ADC", hashfunctions::fEChannel,
			new quantity::ValueQuantity(quantity::fQIE10ADC_256),
			new quantity::ValueQuantity(quantity::fN, true));
		_cTDC_EChannel[j].initialize(_name,
			"TDC", hashfunctions::fEChannel,
			new quantity::ValueQuantity(quantity::fQIE10TDC_64),
			new quantity::ValueQuantity(quantity::fN, true));
	}

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


	//_cShapeCut_EChannel.book(ib, _emap, _filter_C36, _subsystem);
	_cShapeCut_EChannel.book(ib, _emap, _subsystem);
	_cShapeCut.book(ib, _subsystem);
	//	_cTDCvsADC.book(ib, _subsystem);
	_cTDC.book(ib, _subsystem);
	_cADC.book(ib, _subsystem);
	// _cOccupancy_Crate.book(ib, _emap, _filter_C36, _subsystem);
	// _cOccupancy_CrateSlot.book(ib, _emap, _filter_C36, _subsystem);
	_cOccupancy_Crate.book(ib, _emap, _subsystem);
	_cOccupancy_CrateSlot.book(ib, _emap, _subsystem);

	for (unsigned int i=0; i<10; i++)
	{
		char aux[10];
		sprintf(aux, "TS%d", i);
		// _cTDCvsADC_EChannel[i].book(ib, _emap, _filter_C36, _subsystem, aux);
		// _cTDC_EChannel[i].book(ib, _emap, _filter_C36, _subsystem, aux);
		// _cADC_EChannel[i].book(ib, _emap, _filter_C36, _subsystem, aux);
		//		_cTDCvsADC_EChannel[i].book(ib, _emap, _subsystem, aux);
		_cTDC_EChannel[i].book(ib, _emap, _subsystem, aux);
		_cADC_EChannel[i].book(ib, _emap, _subsystem, aux);
	}


	//_ehashmap.initialize(_emap, electronicsmap::fD2EHashMap, _filter_C36);
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

	// std::cout << "QIE11 Analyzer Collection Size = " << cqie10->size()
	// 	<< std::endl;
	for (uint32_t i=0; i<cqie10->size(); i++)
	{
		QIE11DataFrame frame = static_cast<QIE11DataFrame>((*cqie10)[i]);
		DetId did = frame.detid();
		HcalElectronicsId eid = HcalElectronicsId(_ehashmap.lookup(did));
		_cOccupancy_Crate.fill(eid);
		_cOccupancy_CrateSlot.fill(eid);


		//std::cout << "eid: " << eid.readoutVMECrateId() << " " << eid.htrSlot() << " " << eid.fiberIndex() << " " << eid.fiberChanId() << std::endl;

		//	iterate thru all TS and fill
		for (int j=0; j<frame.samples(); j++)
		{
			//	shapes are after the cut
			_cShapeCut_EChannel.fill(eid, j, adc2fC[frame[j].adc()]);	
			_cShapeCut.fill(eid, j, adc2fC[frame[j].adc()]);

			//			std::cout << frame[j].adc() << "  ";
			

			//	w/o a cut
			//			_cTDCvsADC_EChannel[j].fill(eid, frame[j].adc(), 
			//			frame[j].tdc());
			//			_cTDCvsADC.fill(eid, frame[j].adc(), frame[j].tdc());
			_cTDC_EChannel[j].fill(eid, frame[j].tdc());
			_cTDC.fill(eid, frame[j].tdc());
			_cADC_EChannel[j].fill(eid, frame[j].adc());
			_cADC.fill(eid, frame[j].adc());

		}
		//		std::cout << std::endl;
	}
}

/* virtual */ void PedestalTask_904::_resetMonitors(UpdateFreq)
{
}

DEFINE_FWK_MODULE(PedestalTask_904);
