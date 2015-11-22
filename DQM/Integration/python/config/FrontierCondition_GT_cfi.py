import FWCore.ParameterSet.Config as cms

def setGT(process):
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    process.GlobalTag.connect = cms.string("frontier://(proxyurl=http://localhost4:3128)(serverurl=http://localhost4:8000/FrontierOnProd)(serverurl=http://localhost4:8000/FrontierOnProd)(retrieve-ziplevel=0)(failovertoserver=no)/CMS_CONDITIONS")

    if (process.runType.getRunType() == process.runType.cosmic_run):
        process.GlobalTag.globaltag = "75X_dataRun2_HLT_v7"
    else:
        process.GlobalTag.globaltag = "75X_dataRun2_HLTHI_v4"

    es_prefer_GlobalTag = cms.ESPrefer('PoolDBESSource','GlobalTag')
    print 'GT in use:', process.GlobalTag.globaltag
