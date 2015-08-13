#include "DQMServices/Components/src/DQMPrintStats.h"

using namespace edm;

DQMPrintStats::DQMPrintStats( const edm::ParameterSet& ps )
{}


void DQMPrintStats::beginJob() 
{
  dbe_ = edm::Service<DQMStore>().operator->();
}

void DQMPrintStats::endLuminosityBlock(const edm::LuminosityBlock& lumiSeg,
			const edm::EventSetup& c)
{
  std::cout << "DQMPrintStats::endLumi" << std::endl;
  dbe_->printFULLContent();
}


void DQMPrintStats::endRun(const Run& r, const EventSetup& context) 
{
  std::cout << "DQMPrintStats::endRun" << std::endl;
  dbe_->printFULLContent();
}

void DQMPrintStats::endJob() 
{
  std::cout << "DQMPrintStats::endJob" << std::endl;
  dbe_->printFULLContent();
}
