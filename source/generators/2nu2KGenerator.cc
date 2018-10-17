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
#include <G4IonTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4ThreeVector.hh>
#include <G4PrimaryParticle.hh>

namespace nexus{

  TwoNuTwoK::TwoNuTwoK():
    G4VPrimaryGenerator(),
    _msg (0),
    _ion (0),
    _geom(0),
    _z   (52) ,         // 124Xe to
    _a   (124),         // 124Te
    _e   (31.815 * keV) // K-shell binding energy

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

  void TwoNuTwoK::GeneratePrimaryVertex(G4Event* event)
  {
    if (!_ion) _ion = G4IonTable::GetIonTable()->GetIon(_z, _a, _e);

    // Generate a vertex with within the specified region at t = 0
    G4ThreeVector position = _geom->GenerateVertex(_region);
    G4double      time     = 0.;

    G4PrimaryVertex  * vertex    = new G4PrimaryVertex(position, time);
    G4PrimaryParticle* particle1 = new G4PrimaryParticle(_ion);
    G4PrimaryParticle* particle2 = new G4PrimaryParticle(_ion);

    vertex->SetPrimary(particle1);
    vertex->SetPrimary(particle2);
    event->AddPrimaryVertex(vertex);
  }
}
