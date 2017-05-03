import FWCore.ParameterSet.Config as cms

# Configuration parameters for Method 0
m0Parameters = cms.PSet(
    firstSample = cms.int32(1),
    samplesToAdd = cms.int32(9),
    correctForPhaseContainment = cms.bool(False),
    correctionPhaseNS = cms.double(6.0),
)
