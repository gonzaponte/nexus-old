// ----------------------------------------------------------------------------
//  \file     2nu2KGenerator.h
//  \brief    Generator of double electron capture with 2-neutrino
//            emission. It is simulated following the approach in
//            https://arxiv.org/abs/1801.03251,
//            i.e. assuming two independent K-shell electron captures.
//            Two 124Te atoms with a K-shell vacancy are placed at
//            the same vertex
//  \author   Gonzalo Martínez Lema <gonzalo.martinez.lema@csic.es>
//  \date     01 Oct 2018
//  \version  0.0
//
//  Copyright (c) 2018 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __TwoNuTwoK__
#define __TwoNuTwoK__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

namespace nexus{

  class BaseGeometry;

  // Primary generator (concrete class of G4VPrimaryGenerator) for events
  // consisting of a decay of an  Ion. The user must specify via configuration
  // parameters the Isotope, the atomic number and the mass number

  class TwoNuTwoK: public G4VPrimaryGenerator
  {
  public:
     TwoNuTwoK();
    ~TwoNuTwoK();

    void GeneratePrimaryVertex(G4Event*);

  private:
    void SetParticleDefinition();

 private:
    G4GenericMessenger  * _msg;
    G4ParticleDefinition* _ion;
    const BaseGeometry  * _geom;

    G4String _region;
    G4int    _z; // atomic number
    G4int    _a; // mass number
    G4double _e; // excitation energy
  };

} // end namespace nexus

#endif
