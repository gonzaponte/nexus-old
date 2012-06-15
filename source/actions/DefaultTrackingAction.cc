// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Apr 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultTrackingAction.h"

#include "IonizationElectron.h"
#include "DriftTrackInfo.h"
#include "BhepUtils.h"

#include <G4Track.hh>
#include <G4VProcess.hh>
#include <G4OpticalPhoton.hh>

#include <bhep/bhep_svc.h>
#include <bhep/particle.h>


namespace nexus {


  // Creates a bhep particle associated to the Geant4 tracking particle
  // and sets basic properties.
  void DefaultTrackingAction::PreUserTrackingAction(const G4Track* track)
  {
    G4ParticleDefinition* pdef = track->GetDefinition();

    // Exclude optical photons and ionization electrons
    if ((pdef == G4OpticalPhoton::Definition()) ||
     	(pdef == IonizationElectron::Definition())) return;

    // create a new bhep mc particle
    G4String particle_name = track->GetDefinition()->GetParticleName();
    bhep::particle* particle = new bhep::particle(bhep::TRUTH, particle_name);
      
    // set Geant4 track id
    particle->add_property("G4TrackID", track->GetTrackID());

    // family relations and general properties
    G4int parent_id = track->GetParentID();

    if (parent_id == 0) {  // particle is primary
      particle->set_primary(true);
      particle->add_property("CreatorProcess", "none");
    }
    else {                 // particle is secondary
      particle->set_primary(false);
      bhep::particle& parent = BhepUtils::GetBParticle(parent_id);
      particle->set_mother(parent);
      parent.add_daughter(*particle);
      particle->add_property("CreatorProcess", 
			     track->GetCreatorProcess()->GetProcessName());
    }
      
    // adding the particle to the bhep event
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    bevt.add_true_particle(particle);
  }
  
  
  
  // Set additional properties to the bhep particle once tracking has finished
  void DefaultTrackingAction::PostUserTrackingAction(const G4Track* track)
  {
    G4ParticleDefinition* pdef = track->GetDefinition();

    // Exclude optical photons and ionization electrons
    if ((pdef == G4OpticalPhoton::Definition()) ||
	(pdef == IonizationElectron::Definition())) return;

    // Make sure that tracking has not been suspended by a process that tracks
    // secondaries first, or properties would be set more than once
    if (track->GetTrackStatus() == fSuspend)
      return ;

    // get bhep particle associated to the Geant4 track
    bhep::particle& bparticle = BhepUtils::GetBParticle(track->GetTrackID());

    // kinematics at production vertex ................................

    // vertex position and volume
    G4ThreeVector pos = track->GetVertexPosition();
    bparticle.set_vertex(pos.x(), pos.y(), pos.z());

    G4String vertex_volume = track->GetLogicalVolumeAtVertex()->GetName();
    bparticle.add_property("VertexVolume", vertex_volume);
      
    // initial momentum
    G4double kin_energy = track->GetVertexKineticEnergy();
    G4double mass = track->GetDefinition()->GetPDGMass();
    G4double mom_module = sqrt(sqr(mass + kin_energy) - sqr(mass));
    G4ThreeVector mom_direction = track->GetVertexMomentumDirection();
    
    G4ThreeVector mom = mom_module * mom_direction;
    bparticle.set_p(mom.x(), mom.y(), mom.z());
    
    // kinematics at decay vertex .....................................

    // decay position and volume
    pos = track->GetPosition();
    bparticle.set_decay_vertex(pos.x(), pos.y(), pos.z());
    
    G4String decay_volume = track->GetVolume()->GetName();
    bparticle.add_property("DecayVolume", decay_volume);

    // momentum
    mom = track->GetMomentum();
    bparticle.set_decay_p(mom.x(), mom.y(), mom.z());
    
    // track length
    G4double track_length = track->GetTrackLength();
    bparticle.add_property("TrackLength", track_length);

    // // special properties of the ionization clusters .................
    // if (pdef == IonizationCluster::Definition()) {
    //   DriftTrackInfo* drift_info =
    // 	dynamic_cast<DriftTrackInfo*>(track->GetUserInformation());
	
    //   G4int num_charges = drift_info->GetNumberOfCharges();
    //   bparticle.add_property("NumCharges", num_charges);

    //   G4double transv_sigma = drift_info->GetTransverseSpread();
    //   bparticle.add_property("TransvSigma", transv_sigma);
	
    //   G4double longit_sigma = drift_info->GetLongitudinalSpread();
    //   bparticle.add_property("LongitSigma", longit_sigma);

    //   G4double time_sigma = drift_info->GetTimeSpread();
    //   bparticle.add_property("TimeSigma", time_sigma);

    //   G4double drift_time = drift_info->GetDriftTime();
    //   bparticle.add_property("DriftTime", drift_time);
    // }

  }

} // end namespace nexus
