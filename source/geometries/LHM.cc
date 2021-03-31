// -----------------------------------------------------------------------------
// nexus | LHM.cc
//
// Main class that builds the LHM geometry.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "LHM.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4MultiUnion.hh>
#include <Randomize.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  LHM::LHM():
    BaseGeometry(),
    msg_(0),
    tpc_radius_      (10   * cm),
    drift_length_    (5    * cm),
    gem_thickness_   (0.4  * mm),
    interface_height_(5    * mm),
    elgap_length_    (1    * cm),
    hole_diam_       (0.3  * mm),
    hole_pitch_      (1    * mm),
    csi_radius_      (5    * mm),
    csi_thickness_   (0.01 * mm),
    vapor_pressure_  (1.2  * bar),
    outer_pressure_  (1.3  * bar),
    temperature_     (170  * kelvin)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/LHM/", "Control commands of geometry LHM.");

    G4GenericMessenger::Command&
    tpc_radius_cmd = msg_->DeclareProperty("tpc_radius", tpc_radius_, "TPC radius");
    tpc_radius_cmd.SetUnitCategory("Length");
    tpc_radius_cmd.SetParameterName("tpc_radius", false);
    tpc_radius_cmd.SetRange("tpc_radius>0.");

    G4GenericMessenger::Command&
    drift_length_cmd = msg_->DeclareProperty("drift_length", drift_length_, "Drift length");
    drift_length_cmd.SetUnitCategory("Length");
    drift_length_cmd.SetParameterName("drift_length", false);
    drift_length_cmd.SetRange("drift_length>0.");

    G4GenericMessenger::Command&
    gem_thickness_cmd = msg_->DeclareProperty("gem_thickness", gem_thickness_, "GEM thickness");
    gem_thickness_cmd.SetUnitCategory("Length");
    gem_thickness_cmd.SetParameterName("gem_thickness", false);
    gem_thickness_cmd.SetRange("gem_thickness>0.");

    G4GenericMessenger::Command&
    interface_height_cmd = msg_->DeclareProperty("interface_height", interface_height_, "Liquid level from the top of the GEM");
    interface_height_cmd.SetUnitCategory("Length");
    interface_height_cmd.SetParameterName("interface_height", false);
    interface_height_cmd.SetRange("interface_height>0.");

    G4GenericMessenger::Command&
    elgap_length_cmd = msg_->DeclareProperty("elgap_length", elgap_length_, "Length of EL gap");
    elgap_length_cmd.SetUnitCategory("Length");
    elgap_length_cmd.SetParameterName("elgap_length", false);
    elgap_length_cmd.SetRange("elgap_length>0.");

    G4GenericMessenger::Command&
    hole_diam_cmd = msg_->DeclareProperty("hole_diam", hole_diam_, "Diameter of GEM holes");
    hole_diam_cmd.SetUnitCategory("Length");
    hole_diam_cmd.SetParameterName("hole_diam", false);
    hole_diam_cmd.SetRange("hole_diam>0.");

    G4GenericMessenger::Command&
    hole_pitch_cmd = msg_->DeclareProperty("hole_pitch", hole_pitch_, "Spacing of GEM holes");
    hole_pitch_cmd.SetUnitCategory("Length");
    hole_pitch_cmd.SetParameterName("hole_pitch", false);
    hole_pitch_cmd.SetRange("hole_pitch>hole_diam");

    G4GenericMessenger::Command&
    csi_radius_cmd = msg_->DeclareProperty("csi_radius", csi_radius_, "CsI radius");
    csi_radius_cmd.SetUnitCategory("Length");
    csi_radius_cmd.SetParameterName("csi_radius", false);
    csi_radius_cmd.SetRange("csi_radius>0.");

    G4GenericMessenger::Command&
    csi_thickness_cmd = msg_->DeclareProperty("csi_thickness", csi_thickness_, "CsI thickness");
    csi_thickness_cmd.SetUnitCategory("Length");
    csi_thickness_cmd.SetParameterName("csi_thickness", false);
    csi_thickness_cmd.SetRange("csi_thickness>0.");


    G4GenericMessenger::Command&
    vapor_pressure_cmd = msg_->DeclareProperty("vapor_pressure", vapor_pressure, "Vapor pressure");
    vapor_pressure_cmd.SetUnitCategory("Pressure");
    vapor_pressure_cmd.SetParameterName("vapor_pressure", false);
    vapor_pressure_cmd.SetRange("vapor_pressure>0.");


    G4GenericMessenger::Command&
    outer_pressure_cmd = msg_->DeclareProperty("outer_pressure", outer_pressure_, "Outer pressure");
    outer_pressure_cmd.SetUnitCategory("Pressure");
    outer_pressure_cmd.SetParameterName("outer_pressure", false);
    outer_pressure_cmd.SetRange("outer_pressure>0.");


    G4GenericMessenger::Command&
    temperature_cmd = msg_->DeclareProperty("temperature", temperature_, "Temperature");
    temperature_cmd.SetUnitCategory("Temperature");
    temperature_cmd.SetParameterName("temperature", false);
    temperature_cmd.SetRange("temperature>0.");

  }

  LHM::~LHM()
  {
  }

  void LHM::Construct()
  {
    // LAB
    // ------------------------------------------
    G4double         lab_size  = std::max(tpc_radius_, drift_length_) * 3;
    G4Box*           lab_solid = new G4Box("Lab", lab_size, lab_size, lab_size);
    G4Material*      air       = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4LogicalVolume* lab_logic = new G4LogicalVolume(lab_solid, air, "Lab");

    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // new G4PVPlacement(0, G4ThreeVector(), lab_logic, "Lab", 0, false, 0);

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic);


    // LXe
    // ------------------------------------------
    G4double         lxe_length = drift_length_ + gem_thickness_ + interface_height_;
    G4Tubs*          lxe_solid  = new G4Tubs("LXe", 0, tpc_radius_, lxe_length/2, 0, twopi);
    G4Material*      lxe        = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    G4LogicalVolume* lxe_logic  = new G4LogicalVolume(lxe_solid, lxe, "LXe");

    lxe      ->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    lxe_logic->SetVisAttributes(nexus::DarkGreen);

    // Bottom of the Th gem at z=0
    G4ThreeVector        lxe_center(0, 0, -lxe_length/2 + interface_height_ + gem_thickness_);
    new G4PVPlacement(0, lxe_center, lxe_logic, "LXe", lab_logic, false, 0, false);


    // GXe
    // ------------------------------------------
    G4Tubs*          gxe_solid  = new G4Tubs("GXe", 0, tpc_radius_, elgap_length_/2, 0, twopi);
    G4Material*      gxe        = MaterialsList::GXe(1 * bar, 166);
    G4LogicalVolume* gxe_logic  = new G4LogicalVolume(gxe_solid, gxe, "GXe");

    gxe      ->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(1 * bar, 166 * kelvin));
    gxe_logic->SetVisAttributes(nexus::LightGreen);

    G4ThreeVector        gxe_center(0, 0, gem_thickness_ + interface_height_ + elgap_length_/2);
    new G4PVPlacement(0, gxe_center, gxe_logic, "GXe", lab_logic, false, 0, false);


    // THGEM
    // ------------------------------------------
    G4double      eps   = 1e-9 * mm;
    G4Tubs*       hole  = new G4Tubs("Ins_hole", 0, hole_diam_/2, gem_thickness_/2 + eps, 0, twopi);
    G4MultiUnion* holes = new G4MultiUnion("Ins_holes");

    G4RotationMatrix rotation = G4RotationMatrix();
    for   (G4double x=-tpc_radius_; x<tpc_radius_; x += hole_pitch_){
      for (G4double y=-tpc_radius_; y<tpc_radius_; y += hole_pitch_){
        if (tpc_radius_ - std::sqrt(x*x + y*y) < hole_diam_/2) continue;

        G4Transform3D transform(rotation, G4ThreeVector(x, y, 0));
        holes->AddNode(*hole, transform);
      }
    }

    holes->Voxelize();

    G4Tubs*             full_ins_solid    = new G4Tubs("Insulator", 0, tpc_radius_, gem_thickness_/2, 0, twopi);
    G4SubtractionSolid*      ins_solid    = new G4SubtractionSolid("Insulator", full_ins_solid, holes);
    G4Material*              ins_material = MaterialsList::FR4();
    G4LogicalVolume*         ins_logic    = new G4LogicalVolume(ins_solid, ins_material, "Insulator");

    gem_logic->SetVisAttributes(nexus::DarkGrey);

    G4ThreeVector        ins_center (0, 0, lxe_length/2 - interface_height_ - gem_thickness_/2);
    new G4PVPlacement(0, ins_center, ins_logic, "Insulator", lxe_logic, false, 0, false);



    // CsI
    // ------------------------------------------
    G4double      eps       = 1e-9 * mm;
    G4Tubs*       csi_hole  = new G4Tubs("Hole", 0, hole_diam_/2, csi_thickness_/2 + eps, 0, twopi);
    G4MultiUnion* csi_holes = new G4MultiUnion("CsI_holes");

    G4RotationMatrix rotation = G4RotationMatrix();
    for   (G4double x=-tpc_radius_; x<tpc_radius_; x += hole_pitch_){
      for (G4double y=-tpc_radius_; y<tpc_radius_; y += hole_pitch_){
        if (std::sqrt(x*x + y*y) - csi_radius_ > hole_diam_/2) continue;

        G4Transform3D transform(rotation, G4ThreeVector(x, y, 0));
        holes->AddNode(*hole, transform);
      }
    }

    holes->Voxelize();

    G4Tubs*             csi_full  = new G4Tubs("CsI_full", 0, csi_radius_, csi_thickness_/2, 0, twopi);
    G4SubtractionSolid* csi_solid = new G4SubtractionSolid("CsI", csi_full, csi_holes);
    G4Material*         csi       = MaterialsList::CsI();
    G4LogicalVolume*    csi_logic = new G4LogicalVolume(csi_solid, csi, "CsI");

    csi_logic->SetVisAttributes(nexus::Black);

    G4ThreeVector        csi_center (0, 0, lxe_length/2 - interface_height_ - gem_thickness_/2 - csi_thickness_/2);
    new G4PVPlacement(0, csi_center, csi_logic, "CsI", lxe_logic, false, 0, false);

  }



  G4ThreeVector LHM::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    if      (region == "ELGAP_CENTER") {
      G4double z = interface_height_ + G4UniformRand() * elgap_length_;
      vertex.set(0, 0, z);
    }
    else if (region == "ELGAP_SPAN") {
      G4double x =             - 2.5 + G4UniformRand() * 5            ;
      G4double y =             - 2.5 + G4UniformRand() * 5            ;
      G4double z = interface_height_ + G4UniformRand() * elgap_length_;
      vertex.set(x, y, z);
    }
    else if (region == "ACTIVE") {
      G4double x, y, r, z;
      r = tpc_radius_ * 2;
      while (r > tpc_radius_){
        x = -tpc_radius + G4UniformRand() * tpc_radius_ * 2;
        y = -tpc_radius + G4UniformRand() * tpc_radius_ * 2;
        r = std::sqrt(x*x + y*y);
      }
      z = -G4UniformRand() * drift_length_;
      vertex.set(x, y, z);
    }
    else if (region == "AD_HOC") {
      return vertex;
    }
    else {
      G4Exception("[LHM]", "GenerateVertex()", FatalException, "Unknown vertex generation region!");
    }

    return vertex;
  }


} //end namespace nexus
