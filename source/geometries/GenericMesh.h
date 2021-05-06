// ----------------------------------------------------------------------------
// nexus | GenericMesh.h
//
// A g eneric mesh
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef GENERIC_MESH_H
#define GENERIC_MESH_H

#include "BaseGeometry.h"

class G4GenericMessenger;

namespace nexus {

  class GenericMesh : public BaseGeometry
  {
  public:
    ///Constructor
    GenericMesh();

    ///Destructor
    ~GenericMesh(){}

    void Construct();

    G4ThreeVector GenerateVertex(const G4String&) const;

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
    G4bool   visibility_;

    G4String wire_material_;
    G4String wire_shape_   ; // circular | rectangular
    G4String mesh_shape_   ; // circular | rectangular

    G4double mesh_diam_ ; // for circular shape
    G4double mesh_width_; // for rectangular shape

    G4double wire_pitch_    ; // for circular and rectangular shapes
    G4double wire_diam_     ; // for circular shape
    G4double wire_thickness_; // for rectangular shape
    G4double wire_width_    ; // for rectangular shape

  };
}

#endif
