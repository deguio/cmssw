import FWCore.ParameterSet.Config as cms

onlineOfflineComparator = cms.EDAnalyzer("OnlineOfflineComparator",
                                         onlineInputTag = cms.InputTag("hltScoutingCaloPacker"),
                                         offlineInputTag = cms.InputTag("ak4CaloJets")
                                         )

