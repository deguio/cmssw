#include "DQM/DataScouting/plugins/OnlineOfflineComparator.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"

#include <cmath>
#include <typeinfo>
#include <algorithm>

// general
template <size_t I, size_t N>
struct ForEachHelper {
  template <typename Tuple, typename Func>
  static
  void call(Tuple&& tpl, Func&& func) {
    func(std::get<I-1>(tpl));
    ForEachHelper<I+1, N>::call(std::forward<Tuple>(tpl), std::forward<Func>(func));
  }
};
// break recursion
template <size_t N>
struct ForEachHelper<N, N> {
  template <typename Tuple, typename Func>
  static
  void call(Tuple&& tpl, Func&& func) {
    func(std::get<N-1>(tpl));
  }
};

// helper function to provide nice interface
template <typename Tuple, typename Func>
void for_each(Tuple&& tpl, Func&& func) {
  constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
  ForEachHelper<1, size>::call(std::forward<Tuple>(tpl), std::forward<Func>(func));
}

template<typename T>
void OnlineOfflineComparator::Tokens<T>::set(const edm::InputTag& myInputTag, edm::ConsumesCollector& iC) {
  myToken = iC.mayConsume<T>(myInputTag);
}

template<typename T>
void OnlineOfflineComparator::Tokens<T>::getData(const edm::Event& iEvent, edm::Handle<T>& handle) const {
  iEvent.getByToken(myToken, handle);
}

OnlineOfflineComparator::OnlineOfflineComparator( const edm::ParameterSet & conf ):
  ScoutingAnalyzerBase(conf)
{
  const edm::InputTag& onlineInputTag  = conf.getParameter<edm::InputTag>("onlineInputTag");
  const edm::InputTag& offlineInputTag = conf.getParameter<edm::InputTag>("offlineInputTag");
  //edm::ConsumesCollector iC = consumesCollector();
  
  for_each(onlineTokens, [&](auto& tok) {
      //tok.set(onlineInputTag, iC);
      std::cout << tok << std::endl;
    });

  //  for_each(offlineTokens, [&](auto& tok) {
    //   tok.set(offlineInputTag, iC);
    // });
}

OnlineOfflineComparator::~OnlineOfflineComparator() {} 

// Function to book the Monitoring Elements.
void OnlineOfflineComparator::bookHistograms(DQMStore::IBooker & iBooker, edm::Run const &, edm::EventSetup const &)
{
  ScoutingAnalyzerBase::prepareBooking(iBooker);
}

// Usual analyze method
void OnlineOfflineComparator::analyze( const edm::Event & iEvent, const edm::EventSetup & c )
{
  using Type = typename std::decay<decltype(onlineTokens)>::type;

  std::cout << typeid(Type).name() << std::endl;

  // edm::Handle<Type> handle;
  // onlineTokens.getData(iEvent, handle);
  // if(!handle.isValid()) {
  //   std::cout << "AAAARRRGHHH!!!" << std::endl;
  //   return;
  // }
}
