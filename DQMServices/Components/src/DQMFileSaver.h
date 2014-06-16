#ifndef DQMSERVICES_COMPONENTS_DQMFILESAVER_H
#define DQMSERVICES_COMPONENTS_DQMFILESAVER_H

#include "FWCore/Framework/interface/global/EDAnalyzer.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include <sys/time.h>
#include <string>

namespace saverDetails {struct NoCache {};}

class DQMStore;
class DQMFileSaver : public edm::global::EDAnalyzer<edm::RunCache<saverDetails::NoCache>, edm::LuminosityBlockCache<saverDetails::NoCache> >
{
public:
  DQMFileSaver(const edm::ParameterSet &ps);
  
protected:
  virtual void beginJob(void);
  virtual std::shared_ptr<saverDetails::NoCache> globalBeginRun(const edm::Run &, const edm::EventSetup &) const;
  virtual std::shared_ptr<saverDetails::NoCache> globalBeginLuminosityBlock(const edm::LuminosityBlock &, const edm::EventSetup &) const;
  virtual void analyze(edm::StreamID, const edm::Event &e, const edm::EventSetup &) const;
  virtual void globalEndLuminosityBlock(const edm::LuminosityBlock &, const edm::EventSetup &) const;
  virtual void globalEndRun(const edm::Run &, const edm::EventSetup &) const;
  virtual void endJob(void);
  virtual void postForkReacquireResources(unsigned int childIndex, unsigned int numberOfChildren);

public:
  enum Convention
  {
    Online,
    Offline,
    FilterUnit
  };

  enum FileFormat
  {
    ROOT,
    PB
  };

private:
  void saveForOnlinePB(int run, const std::string &suffix) const;
  void saveForOnline(int run, const std::string &suffix, const std::string &rewrite) const;
  void saveForFilterUnitPB(int run, int lumi) const;
  void saveForFilterUnit(const std::string& rewrite, int run, int lumi) const;
  void saveJobReport(const std::string &filename) const;
  void saveJson(int run, int lumi, const std::string& fn, const std::string& data_fn) const;

  Convention	convention_;
  FileFormat    fileFormat_;
  std::string	workflow_;
  std::string	producer_;
  std::string	dirName_;
  std::string   child_;
  std::string	filterName_;
  int        	version_;
  bool		runIsComplete_;
  bool          enableMultiThread_;

  int		saveByLumiSection_;
  int		saveByEvent_;
  int		saveByMinute_;
  int		saveByTime_;
  int		saveByRun_;
  bool		saveAtJobEnd_;
  int		saveReference_;
  int		saveReferenceQMin_;
  int		forceRunNumber_;

  std::string	fileBaseName_;
  std::string	fileUpdate_;

  DQMStore	*dbe_;
  
  //mutable std::atomic<int> irun_;
  mutable std::atomic<int> nrun_;
  //mutable std::atomic<int> ievent_;
  //mutable std::atomic<int> nevent_;
  //mutable std::atomic<int> ilumi_;
  mutable std::atomic<int> nlumi_;
  mutable std::atomic<int> ilumiprev_;
  mutable timeval	start_;
  mutable timeval	saved_;

};

#endif // DQMSERVICES_COMPONEntS_DQMFILESAVER_H
