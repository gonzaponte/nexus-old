// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 21 Nov 2011
//  
//  Copyright (c) 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100Shielding.h"
#include "MaterialsList.h"
#include "ConfigService.h"

#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <G4TransportationManager.hh>



namespace nexus {

  
  Next100Shielding::Next100Shielding(const G4double nozzle_ext_diam,
				     const G4double up_nozzle_ypos,
				     const G4double central_nozzle_ypos,
				     const G4double down_nozzle_ypos,
				     const G4double bottom_nozzle_ypos):
    BaseGeometry(),

    // Shielding internal dimensions
    _shield_x (155.  * cm),
    _shield_y (225.6 * cm),  // To be updated: 266.8 cm is the external dimension -> 266.8 - 2. * (lead_thick + steel_thick)
    _shield_z (253.0 * cm),

    // Box thickness
    _lead_thickness (20. * cm),
    _steel_thickness (6. * mm)

  {

    ///// Shielding is compound by two boxes, the external made of lead,
    ///// and the internal, made of a mix of Steel & Titanium

    ReadParameters();

    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();


    // Auxiliary solids
    G4Box* shielding_box_solid = new G4Box("SHIELD_BOX", _shield_x/2., _shield_y/2., _shield_z/2.);
    G4Tubs* nozzle_hole_solid = new G4Tubs("NOZZLE_HOLE", 0.*cm, nozzle_ext_diam/2.,
					   (_shield_z + 100.*cm)/2., 0.*deg, 360.*deg);
    

    // LEAD BOX   ///////////
    G4double lead_x = _shield_x + 2. * _steel_thickness + 2. * _lead_thickness;
    G4double lead_y = _shield_y + 2. * _steel_thickness + 2. * _lead_thickness;
    G4double lead_z = _shield_z + 2. * _steel_thickness + 2. * _lead_thickness;

    G4Box* lead_box_nh_solid = new G4Box("LEAD_BOX_NH", lead_x/2., lead_y/2., lead_z/2.);
    G4SubtractionSolid* lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_nh_solid, nozzle_hole_solid,
					    0, G4ThreeVector(0. , up_nozzle_ypos, 0.) );
    lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
					    0, G4ThreeVector(0., central_nozzle_ypos, 0.) );
    lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
					    0, G4ThreeVector(0., down_nozzle_ypos, 0.) );
    lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
					    0, G4ThreeVector(0., bottom_nozzle_ypos, 0.) );
    lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, shielding_box_solid);

    G4LogicalVolume* lead_box_logic = new G4LogicalVolume(lead_box_solid,
							  G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"),
							  "LEAD_BOX");
    this->SetLogicalVolume(lead_box_logic);


    // STEEL BOX   ///////////
    G4double steel_x = _shield_x + 2. * _steel_thickness;
    G4double steel_y = _shield_y + 2. * _steel_thickness;
    G4double steel_z = _shield_z + 2. * _steel_thickness;
    
    G4Box* steel_box_nh_solid = new G4Box("STEEL_BOX_NH", steel_x/2., steel_y/2., steel_z/2.);
    G4SubtractionSolid* steel_box_solid = new G4SubtractionSolid("STEEL_BOX", steel_box_nh_solid, nozzle_hole_solid,
								 0, G4ThreeVector(0. , up_nozzle_ypos, 0.) );
    steel_box_solid = new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , central_nozzle_ypos, 0.) );
    steel_box_solid = new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , down_nozzle_ypos, 0.) );
    steel_box_solid = new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , bottom_nozzle_ypos, 0.) );
    steel_box_solid = new G4SubtractionSolid("STEEL_BOX", steel_box_solid, shielding_box_solid);

    G4LogicalVolume* steel_box_logic = new G4LogicalVolume(steel_box_solid,
							  MaterialsList::Steel(),
							  "STEEL_BOX");

    G4PVPlacement* steel_box_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), steel_box_logic,
						       "STEEL_BOX", lead_box_logic, false, 0);




    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes grey(G4Colour(.3, .3, .3));
      lead_box_logic->SetVisAttributes(grey);
      steel_box_logic->SetVisAttributes(grey);
      //void_box_logic->SetVisAttributes(grey);
    }
    else {
      lead_box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      steel_box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      //void_box_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // Creating the vertex generators   //////////
    _lead_gen  = new BoxPointSampler(steel_x, steel_y, steel_z, _lead_thickness, G4ThreeVector(0.,0.,0.), 0);
    _steel_gen = new BoxPointSampler(_shield_x, _shield_y, _shield_z, _steel_thickness, G4ThreeVector(0.,0.,0.), 0);

  }

  
  
  Next100Shielding::~Next100Shielding()
  {
    delete _lead_gen;
    delete _steel_gen;
  }



  void Next100Shielding::ReadParameters()
  {
    const ParamStore& cfg_geom = ConfigService::Instance().Geometry();
    
    _visibility = cfg_geom.GetIParam("shielding_vis");
  }


  
  // G4LogicalVolume* Next100Shielding::GetInternalLogicalVolume()
  // {
  //   return _internal_logic_vol;
  // }



  G4ThreeVector Next100Shielding::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "SHIELDING_LEAD") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _lead_gen->GenerateVertex("WHOLE_VOL");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "LEAD_BOX");
    }

    else if (region == "SHIELDING_STEEL") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _steel_gen->GenerateVertex("WHOLE_VOL");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "STEEL_BOX");
    }

    return vertex;
  }


} //end namespace nexus
