#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <utility>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TTree.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <FWCore/Framework/interface/EDAnalyzer.h>
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

using namespace edm;

///
/// DQMPrintStats itself
///
class DQMPrintStats : public edm::EDAnalyzer {

public:
  DQMPrintStats( const edm::ParameterSet& ps);
  ~DQMPrintStats() {}

protected:
   
  // BeginJob
  void beginJob();

  // BeginRun
  void beginRun(const edm::Run& r, const edm::EventSetup& c) {}

  // Fake Analyze
  void analyze(const edm::Event& e, const edm::EventSetup& c) {}

  void beginLuminosityBlock(const edm::LuminosityBlock& lumiSeg, 
                            const edm::EventSetup& context) {}

  // DQM Client Diagnostic
  void endLuminosityBlock(const edm::LuminosityBlock& lumiSeg, 
                          const edm::EventSetup& c);

  // EndRun
  void endRun(const edm::Run& r, const edm::EventSetup& c);


  // Endjob
  void endJob();

private:
  DQMStore* dbe_;

};

