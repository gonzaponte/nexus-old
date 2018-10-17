// ---------------------------------------------------------------------------
//  \file     2nu2KGenerator.cc
//  \author   Gonzalo Martínez Lema <gonzalo.martinez.lema@csic.es>
//  \date     01 Oct 2018
//  \version  0.0
//
//  Copyright (c) 2018 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------

#include "2nu2KGenerator.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>
#include <G4RunManager.hh>
#include <G4AtomicDeexcitation.hh>
#include <G4PrimaryVertex.hh>
#include <G4ThreeVector.hh>
#include <G4PrimaryParticle.hh>
#include <G4DynamicParticle.hh>

namespace nexus{

  TwoNuTwoK::TwoNuTwoK():
    G4VPrimaryGenerator(),
    _msg     ( 0),
    _geom    ( 0),
    _z       (52), // 124Te
    _shell_id( 1)  // K shell
  {
    _msg = new G4GenericMessenger(this, "/Generator/2NU2K/",
  				                        "Control commands of the ion gun primary generator.");

    _msg->DeclareProperty("region", _region,
                          "Set the region of the geometry where the vertex will be generated.");

    DetectorConstruction* detconst;
    detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    _geom    = detconst->GetGeometry();
  }


  TwoNuTwoK::~TwoNuTwoK()
  {
    delete _msg;
  }

  G4PrimaryParticle* TwoNuTwoK::GetPrimaryParticle(G4DynamicParticle* dyp)
  {
    G4ParticleDefinition* def = dyp->GetDefinition();

    G4double px     = dyp->GetMomentum    ().getX();
    G4double py     = dyp->GetMomentum    ().getY();
    G4double pz     = dyp->GetMomentum    ().getZ();
    G4double mass   = def->GetPDGMass     ();
    G4double charge = def->GetPDGCharge   ();
    G4double polx   = dyp->GetPolarization().x();
    G4double poly   = dyp->GetPolarization().y();
    G4double polz   = dyp->GetPolarization().z();


    G4PrimaryParticle* particle = new G4PrimaryParticle(def, px, py, pz);
    particle->SetMass        (mass            );
    particle->SetCharge      (charge          );
    particle->SetPolarization(polx, poly, polz);

    return particle;
  }

  std::vector<G4PrimaryParticle*> TwoNuTwoK::GeneratePrimaryParticles()
  {
    std::vector<G4PrimaryParticle*> primary_particles;
    for (int atom=0; atom<2; ++atom)
    {
      G4AtomicDeexcitation* AtomicDeexcitation = new G4AtomicDeexcitation();
      AtomicDeexcitation->ActivateAugerElectronProduction(true);

      std::vector<G4DynamicParticle*>* dyparticles = AtomicDeexcitation->GenerateParticles(_z, _shell_id);


      for (unsigned int i=0; i<dyparticles->size(); ++i)
      {
        G4PrimaryParticle* particle = GetPrimaryParticle((*dyparticles)[i]);
        primary_particles.emplace_back(particle);
      }

      delete AtomicDeexcitation;
    }
    return primary_particles;
  }

  void TwoNuTwoK::GeneratePrimaryVertex(G4Event* event)
  {
    // Generate a vertex with within the specified region at t = 0
    G4ThreeVector position = _geom->GenerateVertex(_region);
    G4double      time     = 0.;

    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
    std::vector<G4PrimaryParticle*> particles = GeneratePrimaryParticles();

    for (unsigned int i=0; i<particles.size(); ++i)
      vertex->SetPrimary(particles[i]);

    event->AddPrimaryVertex(vertex);
  }
}
