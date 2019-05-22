#run with: cmsRun hgcsiNoiseMapTester_cfg.py doseMap=SimCalorimetry/HGCalSimProducers/data/doseParams_3000fb.txt

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from Configuration.StandardSequences.Eras import eras

options = VarParsing()
options.register ("doseMap", "",  VarParsing.multiplicity.singleton, VarParsing.varType.string)
options.parseArguments()

process = cms.Process("demo",eras.Phase2C8)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )
process.source = cms.Source("EmptySource")

process.plotter = cms.EDAnalyzer("HGCSiNoiseMapAnalyzer",
                                 doseMap          = cms.string( options.doseMap ),
                                 ePerMipPerMicron = cms.double(73)
                             )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("dosemap_output.root")
)

process.p = cms.Path(process.plotter)
