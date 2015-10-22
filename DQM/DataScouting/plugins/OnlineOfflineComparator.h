#ifndef OnlineOfflineComparator_h
#define OnlineOfflineComparator_h

#include "DQM/DataScouting/interface/ScoutingAnalyzerBase.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"

#include "DataFormats/Scouting/interface/ScoutingCaloJet.h"
#include "DataFormats/Scouting/interface/ScoutingPFJet.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "TLorentzVector.h"
#include <vector>
#include <cmath>

class OnlineOfflineComparator : public ScoutingAnalyzerBase {
 public:
  explicit OnlineOfflineComparator( const edm::ParameterSet &  ) ;
  virtual ~OnlineOfflineComparator() ;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  virtual void analyze( const edm::Event & , const edm::EventSetup &  );
  
 private: 

  template <typename T>
  class Tokens {
  public:
    using type = T;

    Tokens() {}
    
    void set(const edm::InputTag& , edm::ConsumesCollector&);    
    void getData(const edm::Event& iEvent, edm::Handle<T>& handle) const;
    
  private:
    edm::EDGetTokenT<T> myToken;
  };


  std::tuple<
    Tokens<ScoutingCaloJetCollection>,
    Tokens<ScoutingPFJetCollection>
    > onlineTokens;

  std::tuple<
    Tokens<reco::CaloJetCollection>,
    Tokens<reco::PFJetCollection>
    > offlineTokens;


};
#endif
