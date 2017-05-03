import FWCore.ParameterSet.Config as cms

#
# Ecal part
#
from RecoLocalCalo.Configuration.ecalLocalRecoSequenceCosmics_cff import *
from RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi import *

#defines a sequence ecalLocalRecoSequence

#
# Hcal part
#
#HCAL reconstruction
import RecoLocalCalo.Configuration.hcalLocalReco_cff as _hcalLocalReco_cff
from RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi import *
#
# sequence CaloLocalReco
#

def hbheCosmic(module):
    return module.clone(
        tsFromDB = cms.bool(False),
        recoParamsFromDB = cms.bool(False),
        algorithm = dict(
            useM2 = cms.bool(False),
            useM3 = cms.bool(False),
            firstSampleShift = cms.int32(-3),
            samplesToAdd = cms.int32(9),
            correctForPhaseContainment = cms.bool(False),
        )
    )

hbhereco = hbheCosmic(_hcalLocalReco_cff.hbheprereco)
hfreco = _hcalLocalReco_cff._default_hfreco.clone(
    firstSample = 1,
    samplesToAdd = 9, ### min(10,size) in the algo
    correctForTimeslew = False,
    correctForPhaseContainment = False,
    tsFromDB = False,
    recoParamsFromDB = cms.bool(False),
    digiTimeFromDB = False,
)
horeco = _hcalLocalReco_cff.horeco.clone(
    firstSample = 0,
    samplesToAdd = 10,
    correctForTimeslew = False,
    correctForPhaseContainment = False,
    tsFromDB = False,
    recoParamsFromDB = cms.bool(False),
)
zdcreco = _hcalLocalReco_cff.zdcreco.clone(
#    firstSample = 1,
#    samplesToAdd = 8,
    correctForTimeslew = True,
    correctForPhaseContainment = True,
    correctionPhaseNS = 10.,
)

# 2017 customs
from Configuration.Eras.Modifier_run2_HF_2017_cff import run2_HF_2017

_phase1_hfreco = _hcalLocalReco_cff._phase1_hfreco.clone(
    algorithm = dict(
        Class = cms.string("HFSimpleTimeCheck"),
        rejectAllFailures = cms.bool(False),
    )
)


run2_HF_2017.toReplaceWith(hfreco, _phase1_hfreco )

hfprereco = _hcalLocalReco_cff.hfprereco.clone(
    sumAllTimeSlices = cms.bool(True)
)

from RecoLocalCalo.HcalRecProducers.hbheplan1_cfi import hbheplan1

# redefine hcal sequence
hcalLocalRecoSequence = cms.Sequence(hbhereco+hfreco+horeco+zdcreco)

_phase1_hcalLocalRecoSequence = hcalLocalRecoSequence.copy()
_phase1_hcalLocalRecoSequence.insert(0,hfprereco)
run2_HF_2017.toReplaceWith(hcalLocalRecoSequence, _phase1_hcalLocalRecoSequence)

# shuffle modules so "hbheplan1" produces final collection of hits named "hbhereco"
_plan1_hcalLocalRecoSequence = _phase1_hcalLocalRecoSequence.copy()
hbheprereco = hbhereco.clone()
_plan1_hcalLocalRecoSequence.insert(0,hbheprereco)
from Configuration.Eras.Modifier_run2_HEPlan1_2017_cff import run2_HEPlan1_2017
run2_HEPlan1_2017.toReplaceWith(hbhereco, hbheplan1)
run2_HEPlan1_2017.toReplaceWith(hcalLocalRecoSequence, _plan1_hcalLocalRecoSequence)

calolocalrecoCosmics = cms.Sequence(ecalLocalRecoSequenceCosmics+hcalLocalRecoSequence)

#
# R.Ofierzynski (29.Oct.2009): add NZS sequence
#
from RecoLocalCalo.Configuration.hcalLocalRecoNZS_cff import *
calolocalrecoCosmicsNZS = cms.Sequence(ecalLocalRecoSequenceCosmics+hcalLocalRecoSequence+hcalLocalRecoSequenceNZS) 
