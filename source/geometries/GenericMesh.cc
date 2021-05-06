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
#include <G4MultiUnion.hh>
#include <G4Transform3D.hh>
#include <G4SubtractionSolid.hh>
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
, mesh_angle_(0)

, wire_pitch_    (0)
, wire_diam_     (0)
, wire_thickness_(0)
, wire_width_    (0)

{
  msg_ = new G4GenericMessenger{this, "/Geometry/GenericMesh/",
                                "Control commands of a generic mesh geometry."};
  msg_->DeclareProperty("visibility", visibility_,
                        "Visibility of this geometry");

  msg_->DeclareProperty("wire_material_" , wire_material_ , "");
  msg_->DeclareProperty("wire_shape_"    , wire_shape_    , "");
  msg_->DeclareProperty("mesh_shape_"    , mesh_shape_    , "");
  msg_->DeclareProperty("mesh_diam_"     , mesh_diam_     , "");
  msg_->DeclareProperty("mesh_width_"    , mesh_width_    , "");
  msg_->DeclareProperty("mesh_angle_"    , mesh_angle_    , "");
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

       if (wire_shape_ == "rectangular") wire = new G4Box {"wire", wire_width_/2, length/2, wire_thickness_/2};
  else if (wire_shape_ == "circular"   ) wire = new G4Tubs{"wire", 0, wire_diam_/2, length/2, 0, 2*pi};
  else     G4cerr << "Invalid wire shape: " << wire_shape_ << G4endl;

  G4MultiUnion* all_wires = new G4MultiUnion{"wires"};

  int n_wires = std::ceil(length / wire_pitch_) - 1;

  G4RotationMatrix* along_x_axis = new G4RotationMatrix{}; along_x_axis->rotateY(90 * deg);
  G4RotationMatrix* along_y_axis = new G4RotationMatrix{}; along_y_axis->rotateX(90 * deg);

  // HORIZONTAL WIRES
  for (G4int i=1; i<n_wires; i++)
  {
    G4ThreeVector pos       = G4ThreeVector{0,  -length/2. + wire_pitch_ * i, 0};
    G4Transform3D transform = G4Transform3D(*along_x_axis, pos);
    all_wires->AddNode(*wire, transform);
  }

  // VERTICAL WIRES
  for (G4int i=1; i<n_wires; i++)
  {
    G4ThreeVector pos       = G4ThreeVector{-length/2. + wire_pitch_ * i, 0, 0};
    G4Transform3D transform = G4Transform3D(*along_y_axis, pos);
    all_wires->AddNode(*wire, transform);
  }

  all_wires->Voxelize();

  G4VSolid* wires = all_wires;

  if (mesh_shape_ == "circular") {
    G4double thickness = std::max(wire_thickness_, wire_diam_);
    G4Tubs* disk = new G4Tubs{"disk", mesh_diam_/2, mesh_diam_, thickness/2, 0, 2*pi};

    G4SubtractionSolid* framed_wires = new G4SubtractionSolid{ "framed_wires"
                                                             , all_wires
                                                             , disk
                                                           };
    wires = framed_wires;
  }

  G4Material*      wire_material = G4NistManager::Instance()->FindOrBuildMaterial(wire_material_);
  G4LogicalVolume* wire_logic    = new G4LogicalVolume{wires, wire_material, "wires"};

  G4RotationMatrix* rotation = new G4RotationMatrix{}; rotation->rotateZ(mesh_angle_);
  new G4PVPlacement( rotation
                   , G4ThreeVector{}
                   , wire_logic
                   , "WIRES"
                   , nullptr
                   , false
                   , 0
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
