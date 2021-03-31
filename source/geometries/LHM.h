// -----------------------------------------------------------------------------
// nexus | LHM.h
//
// Main class that builds the LHM geometry.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef LHM_H
#define LHM_H

#include "BaseGeometry.h"
#include <G4RotationMatrix.hh>

class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class LHM: public BaseGeometry {
  public:
    /// Constructor
    LHM();

    /// Destructor
    ~LHM();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void Construct();

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    G4double tpc_radius_;
    G4double drift_length_;
    G4double gem_thickness_;
    G4double interface_height_;
    G4double elgap_length_;
    G4double hole_diam_;
    G4double hole_pitch_;
    G4double csi_radius_;
    G4double csi_thickness_;

    G4double vapor_pressure_;
    G4double outer_pressure_;
    G4double temperature_;

  };

} // end namespace nexus

#endif
