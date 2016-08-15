import FWCore.ParameterSet.Config as cms

ledTask_904 = cms.EDAnalyzer(
	"LEDTask_904",

        #       standard
        name = cms.untracked.string("LEDTask_904"),
        debug = cms.untracked.int32(0),
        runkeyVal = cms.untracked.int32(0),
        runkeyName = cms.untracked.string("pp_run"),

        #       tag
        tagQIE11 = cms.untracked.InputTag("hcalDigis"),

        #       cuts
        cut = cms.untracked.double(20),
        ped = cms.untracked.int32(4)
)

