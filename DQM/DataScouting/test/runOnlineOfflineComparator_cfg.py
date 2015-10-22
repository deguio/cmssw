import FWCore.ParameterSet.Config as cms

process = cms.Process("COMPARITOR")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

#load DQM
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQMServices.Core.DQM_cfg")

process.load('DQM.DataScouting.OnlineOfflineComparator_cfi')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
        "file:/build/deguio/DiJet/CMSSW_7_4_14_dqmModule/src/HLTrigger/Configuration/test/step3_RAW2DIGI_L1Reco_RECO_DQM.root"
        )
)


from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v2', '')

process.dqmSaver.workflow = '/DataScouting/DQM/Test'
process.dqmsave_step = cms.Path(process.dqmSaver)

# Path and EndPath definitions
process.comparitor_step = cms.Path(process.onlineOfflineComparator)

process.schedule = cms.Schedule(process.comparitor_step,
                                process.dqmsave_step)


process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
