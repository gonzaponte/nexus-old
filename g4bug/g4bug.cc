#include <G4MaterialPropertiesTable.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>

#include "OpticalMaterialProperties.h"

void test1(){
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  G4Material*                mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  G4double e_scale = 1e-6;
  G4double p_scale = 1e-2;

  G4double energies[] = {0, 0.999 * e_scale, 1 * e_scale, 2 * e_scale};
  G4double   values[] = {0, 0              , 1 * p_scale, 1 * p_scale};

  mpt->AddProperty("NAME", energies, values, 4);

  mat->SetMaterialPropertiesTable(mpt);

  G4int N = 10;
  for (int i=0; i<N; ++i){
    G4double e = 2.0/N * i * e_scale;
    G4MaterialPropertiesTable* mpt2 = mat->GetMaterialPropertiesTable();
    G4MaterialPropertyVector*  prop = mpt2->GetProperty("NAME");
    G4double v = prop->Value(e);
    G4cout << i << " " << e << " " << v << G4endl;
  }
}


void test2(){
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  G4Material*                mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  static constexpr G4double optPhotMinE_ =  0.2  * eV;
  static constexpr G4double optPhotMaxE_ = 11.5  * eV;

  G4double stainless_wf = 4.3  * eV;
  G4double prob         = 1e-2;

  G4double energies [] = {optPhotMinE_, stainless_wf - 0.1*eV, stainless_wf, optPhotMaxE_};
  G4double values   [] = {           0,                     0, prob        , prob        };
  mpt->AddProperty("NAME", energies, values, 4);

  mat->SetMaterialPropertiesTable(mpt);

  G4int N = 10;
  for (int i=0; i<N; ++i){
    G4double e = optPhotMinE_ + (optPhotMaxE_ - optPhotMinE_) / N * i;
    G4double v = mpt->GetProperty("NAME")->Value(e);
    G4cout << i << " " << e << " " << v << G4endl;
  }
}




void test3(){
  G4Material*                mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  G4MaterialPropertiesTable* mpt = OpticalMaterialProperties::FakeGrid(10 * atm,
                                                                       300 * Kelvin,
                                                                       0.9,
                                                                       1 * mm,
                                                                       1,
                                                                       1,
                                                                       1e-2);

  static constexpr G4double optPhotMinE_ =  0.2  * eV;
  static constexpr G4double optPhotMaxE_ = 11.5  * eV;

  G4double stainless_wf = 4.3  * eV;
  G4double prob         = 1e-2;

  mat->SetMaterialPropertiesTable(mpt);

  G4int N = 10;
  for (int i=0; i<N; ++i){
    G4double e = optPhotMinE_ + (optPhotMaxE_ - optPhotMinE_) / N * i;
    G4double v = mpt->GetProperty("NAME")->Value(e);
    G4cout << i << " " << e << " " << v << G4endl;
  }
}




int main(){
  test1();
  G4cout << G4endl << "===========================" << G4endl << G4endl;
  test2();
  G4cout << G4endl << "===========================" << G4endl << G4endl;
  test3();
}
