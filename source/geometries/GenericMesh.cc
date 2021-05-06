// ----------------------------------------------------------------------------
// nexus | GenericMesh.cc
//
// Co GenericMesh at WIS
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "GenericMesh.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4ThreeVector.hh>
#include <Randomize.hh>

namespace nexus {

using namespace CLHEP;

GenericMesh::GenericMesh():
  visibility_    (false)
, wire_material_("")
, wire_shape_   ("")
, mesh_shape_   ("")

, mesh_diam_ (0)
, mesh_width_(0)

, wire_pitch_    (0)
, wire_diam_     (0)
, wire_thickness_(0)
, wire_width_    (0)

{
  msg_ = new G4GenericMessenger(this, "/Geometry/GenericMesh/",
                                "Control commands of a generic mesh geometry.");
  msg_->DeclareProperty("visibility", visibility_,
                        "Visibility of this geometry");

  msg_->DeclareProperty("wire_material_" , wire_material_ , "");
  msg_->DeclareProperty("wire_shape_"    , wire_shape_    , "");
  msg_->DeclareProperty("mesh_shape_"    , mesh_shape_    , "");
  msg_->DeclareProperty("mesh_diam_"     , mesh_diam_     , "");
  msg_->DeclareProperty("mesh_width_"    , mesh_width_    , "");
  msg_->DeclareProperty("wire_pitch_"    , wire_pitch_    , "");
  msg_->DeclareProperty("wire_diam_"     , wire_diam_     , "");
  msg_->DeclareProperty("wire_thickness_", wire_thickness_, "");
  msg_->DeclareProperty("wire_width_"    , wire_width_    , "");

}

void GenericMesh::Construct()
{
  mesh_shape_.toLower();
  wire_shape_.toLower();

  G4double  length = 0;
  G4VSolid* wire   = nullptr;

       if (mesh_shape_ == "rectangular") length = mesh_width_;
  else if (mesh_shape_ == "circular"   ) length = mesh_diam_ ;
  else     G4cerr << "Invalid mesh shape: " << mesh_shape_ << G4endl;

       if (wire_shape_ == "rectangular") wire = new G4Box ("wire", wire_width_/2, length/2, wire_thickness_/2);
  else if (wire_shape_ == "circular"   ) wire = new G4Tubs("wire", 0, wire_diam_/2, length/2, 0, 2*pi);
  else     G4cerr << "Invalid wire shape: " << wire_shape_ << G4endl;

  G4Material*      wire_material = G4NistManager::Instance()->FindOrBuildMaterial(wire_material_);
  G4LogicalVolume* wire_logic    = new G4LogicalVolume(wire, wire_material, "wire");

  G4RotationMatrix* along_x_axis = new G4RotationMatrix(); along_x_axis->rotateY(90 * deg);
  G4RotationMatrix* along_y_axis = new G4RotationMatrix(); along_y_axis->rotateX(90 * deg);

  int n_wires = std::ceil(length / wire_pitch_) - 1;
  int k       = 0;

  // HORIZONTAL WIRES
  for (G4int i=1; i<n_wires; i++)
    new G4PVPlacement( along_x_axis
                     , G4ThreeVector(0,  -length/2. + wire_pitch_ * i, 0)
                     , wire_logic
                     , "HWIRE_" + std::to_string(i)
                     , nullptr
                     , false
                     , k++
                     , false
                     );

  // Vertical WIRES
  for (G4int i=1; i<n_wires; i++)
    new G4PVPlacement( along_y_axis
                     , G4ThreeVector(-length/2. + wire_pitch_ * i, 0, 0)
                     , wire_logic
                     , "VWIRE_" + std::to_string(i)
                     , nullptr
                     , false
                     , k++
                     , false
                     );

  this->SetLogicalVolume(wire_logic);

   if (visibility_) {
    G4VisAttributes white = nexus::White();
    white.SetForceSolid(true);
    wire_logic->SetVisAttributes(white);

  }
}

G4ThreeVector GenericMesh::GenerateVertex(const G4String& region) const{
  return G4ThreeVector{};
}


} // namespace nexus
