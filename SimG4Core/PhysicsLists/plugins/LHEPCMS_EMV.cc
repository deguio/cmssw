#include "LHEPCMS_EMV.hh"
#include "SimG4Core/PhysicsLists/interface/CMSMonopolePhysics.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4EmStandardPhysics_option1.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4HadronElasticPhysicsLHEP.hh"

#include "G4DataQuestionaire.hh"
#include "HadronPhysicsLHEP.hh"

LHEPCMS_EMV::LHEPCMS_EMV(G4LogicalVolumeToDDLogicalPartMap& map,
		 const HepPDT::ParticleDataTable * table_, 
		 sim::FieldBuilder *fieldBuilder_, 
		 const edm::ParameterSet & p) : PhysicsList(map, table_, fieldBuilder_, p) {

  G4DataQuestionaire it(photon);
  
  int  ver     = p.getUntrackedParameter<int>("Verbosity",0);
  bool emPhys  = p.getUntrackedParameter<bool>("EMPhysics",true);
  bool hadPhys = p.getUntrackedParameter<bool>("HadPhysics",true);
  edm::LogInfo("PhysicsList") << "You are using the simulation engine: "
			      << "LHEP_EMV with Flags for EM Physics "
			      << emPhys << " and for Hadronic Physics "
			      << hadPhys << "\n";

  if (emPhys) {
    // EM Physics
    RegisterPhysics( new G4EmStandardPhysics_option1(ver));

    // Synchroton Radiation & GN Physics
    RegisterPhysics( new G4EmExtraPhysics(ver));
  }

  // General Physics - i.e. decay
  RegisterPhysics( new G4DecayPhysics(ver));

  if (hadPhys) {
    // Hadron Elastic scattering
    RegisterPhysics( new G4HadronElasticPhysicsLHEP(ver));

    // Hadron Physics
    RegisterPhysics(  new HadronPhysicsLHEP(ver));

    // Ion Physics
    RegisterPhysics( new G4IonPhysics(ver));
  }

  // Monopoles
  RegisterPhysics( new CMSMonopolePhysics(table_,fieldBuilder_,p));
}


