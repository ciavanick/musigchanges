#include "musigDetectorConstruction.h"
#include "musigDetectorMessenger.h"
#include "musigTrackerSD.h"

#include <G4PhysicalConstants.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4PVPlacement.hh>
#include <G4PVReplica.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4RotationMatrix.hh>
#include <G4GeometryTolerance.hh>
#include <G4Para.hh>
#include <G4UserLimits.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4SystemOfUnits.hh>
#include <G4ios.hh>

#include <vector>
#include <tuple>


///---- constructor with initializer list ------------------------

namespace MuSiG {

    DetectorConstruction::DetectorConstruction() : fWorldLength(0., 0., 0.) {
        sisfe = sisfeGeometry();
        DefineMaterials();

        detectorMessenger = new DetectorMessenger(this);
    }

///---- destructor ------------------------------------------------

    DetectorConstruction::~DetectorConstruction() {
        delete stepLimit;
        delete physiWorld;
        delete detectorMessenger;
    }


    void DetectorConstruction::DefineMaterials() {

        G4NistManager *manager = G4NistManager::Instance();
        manager->SetVerbose(0);

        auto H = manager->FindOrBuildElement(1);
        auto C = manager->FindOrBuildElement(6);
        auto N = manager->FindOrBuildElement(7);
        auto O = manager->FindOrBuildElement(8);
        auto Si = manager->FindOrBuildElement(14);
        auto Ge = manager->FindOrBuildElement(32);
        auto Sb = manager->FindOrBuildElement(51);
        auto I = manager->FindOrBuildElement(53);
        auto Cs = manager->FindOrBuildElement(55);
        auto Pb = manager->FindOrBuildElement(82);
        auto Bi = manager->FindOrBuildElement(83);
        auto Ca = new G4Element("Ca", "Ca", 31, (69.72 * g / mole));
        auto Na = new G4Element("Na", "Na", 11, (22.99 * g / mole));
        auto Mg = new G4Element("Mg", "Mg", 12, (24.305 * g / mole));
        auto K = new G4Element("K", "K", 19, (39.1 * g / mole));
        auto Mn = new G4Element("Mn", "Mn", 25, (54.93 * g / mole));
        auto Zn = new G4Element("Zn", "Zn", 30, (65.38 * g / mole));
        auto Ti = new G4Element("Ti", "Ti", 22, (47.86 * g / mole));
        auto Cr = new G4Element("Cr", "Cr", 24, (51.99 * g / mole));
        auto Al = new G4Element("Al", "Al", 13, (26.98 * g / mole));
        auto Cu = new G4Element("Cu", "Cu", 29, (63.55 * g / mole));
        auto Fe = new G4Element("Fe", "Fe", 26, (55.845 * g / mole));

        auto Paraffin = manager->FindOrBuildMaterial("G4_PARAFFIN");
        auto Graphite = manager->FindOrBuildMaterial("G4_GRAPHITE");
        auto Kapton = manager->FindOrBuildMaterial("G4_KAPTON");
        Mylar = manager->FindOrBuildMaterial("G4_MYLAR");

        // define an Element from isotopes, by relative abundance

        auto U5 = new G4Isotope("U235", 92, 235, (235.01 * g / mole));
        auto U8 = new G4Isotope("U238", 92, 238, (238.03 * g / mole));
        auto U = new G4Element("enriched Uranium", "U", 2);
        U->AddIsotope(U5, (90. * perCent));
        U->AddIsotope(U8, (10. * perCent));

        // define simple materials

        auto liquidH2 = new G4Material("liquidH2", 1, (1.008 * g / mole), (70.8 * mg / cm3));
        auto tungsten = new G4Material("Tungsten", 74, (183.85 * g / mole), (19.30 * g / cm3));
        auto gold = new G4Material("Gold", 79, (196.97 * g / mole), (19.32 * g / cm3));
        auto uranium = new G4Material("Uranium", 92, (238.03 * g / mole), (18.95 * g / cm3));
        auto iron = new G4Material("Iron", 26, (55.85 * g / mole), (7.870 * g / cm3));

        Aluminium = new G4Material("Aluminium", 13, (26.982 * g / mole), (2.699 * g / cm3));
        Titanium = new G4Material("Titanium", 22, (47.867 * g / mole), (4.54 * g / cm3));
        Copper = new G4Material("Copper", 29, (63.55 * g / mole), (8.960 * g / cm3));
        Berillium = new G4Material("Berillium", 4, (9.1 * g / mole), (1.85 * g / cm3));
        Lithium = new G4Material("Lithium", 3, (6.941 * g / mole), (0.525 * g / cm3));

        Ar_gas = new G4Material("Ar_gas", 18, (39.948 * g / mole),
                                (0.00178 * g / cm3)); // in NIST, 0.00166*g/cm3, in Wiki 0.00178*g/cm3
        Ar_gas->GetIonisation()->SetMeanExcitationEnergy(188.0 * eV);

        CNTforest = new G4Material("CNTforest", 6, (12.0107 * g / mole), (0.1 * g / cm3));


        //liquid helium (0.12496*g/cm3) or gaseous helium (0.012 g/cm3) at atmospheric pressure
        //5.8 K 3.44e21 1/cm3    --- 0.023 g/cm3 at 2 bar, 0.0154 at 1.5 bar, 0.00946 at 1 bar
        // 6  K 1.5 bar / 0.0146 g/cm3, 1.6 bar / 0.0158 g/cm3, 1.7 bar 0.0171 g/cm3

        LiqHe = new G4Material("LiqHe", 2, (4.0 * g / mole), (0.145 * g / cm3));  // 0.12...volt
        GasHe = new G4Material("GasHe", 2, (4.0 * g / mole), (0.00000171 * g / cm3));  // 0.023


        // define a material from elements.   case 1: chemical molecule

        Water = new G4Material("Water", (1. * g / cm3), 2);
        Water->AddElement(H, 2);
        Water->AddElement(O, 1);
        Water->GetIonisation()->SetMeanExcitationEnergy(78.0 * eV);
        Water->SetChemicalFormula("H_2O");

        Scintillator = new G4Material("Scintillator", (1.032 * g / cm3), 2);
        Scintillator->AddElement(H, (8.5 * perCent));  // 14
        Scintillator->AddElement(C, (91.5 * perCent));  // 86
        Scintillator->GetIonisation()->SetMeanExcitationEnergy(64.7 * eV);
        Scintillator->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

        Lucite = new G4Material("Lucite", (1.185 * g / cm3), 3);
        Lucite->AddElement(C, 59.97 * perCent);
        Lucite->AddElement(H, 8.07 * perCent);
        Lucite->AddElement(O, 31.96 * perCent);

        Silicon = new G4Material("Silicon", (2.330 * g / cm3), 1);
        Silicon->AddElement(Si, 1);

        FusedSilica = new G4Material("FusedSilica", (2.200 * g / cm3), 2);
        FusedSilica->AddElement(Si, 1);
        FusedSilica->AddElement(O, 2);

        CsI = new G4Material("CsI", (4.534 * g / cm3), 2);
        CsI->AddElement(Cs, 1);
        CsI->AddElement(I, 1);
        CsI->GetIonisation()->SetMeanExcitationEnergy(553.1 * eV);

        BGO = new G4Material("BGO", (7.10 * g / cm3), 3);
        BGO->AddElement(O, 12);
        BGO->AddElement(Ge, 3);
        BGO->AddElement(Bi, 4);

        // Concrete
        //G4Material* Sil = new G4Material("Sil", 14, 28.09*g/mole, 2.33*g/cm3);
        Concrete = new G4Material("Concrete", (2.31 * g / cm3), 9);
        Concrete->AddElement(H, 1.1 * perCent);
        Concrete->AddElement(O, 48.2 * perCent);
        Concrete->AddElement(Na, 2.0 * perCent);
        Concrete->AddElement(Mg, 1.1 * perCent);
        Concrete->AddElement(Al, 6.1 * perCent);
        Concrete->AddElement(Si, 22.4 * perCent);
        Concrete->AddElement(K, 1.0 * perCent);
        Concrete->AddElement(Ca, 12.4 * perCent);
        Concrete->AddElement(Fe, 5.7 * perCent);

        // define gaseous materials using G4 NIST database
        auto Air = manager->FindOrBuildMaterial("G4_AIR");
        manager->ConstructNewGasMaterial("Air20", "G4_AIR", (293. * kelvin), (1. * atmosphere));

        //-------- define a material from elements and others materials (mixture of mixtures)
        auto Lead = new G4Material("Lead", (11.35 * g / cm3), 1);
        Lead->AddElement(Pb, 1.0);

        auto LeadSb = new G4Material("LeadSb", (11.35 * g / cm3), 2);
        LeadSb->AddElement(Sb, (4. * perCent));
        LeadSb->AddElement(Pb, (96. * perCent));

        Aerogel = new G4Material("Aerogel", (0.029 * g / cm3), 3);
        Aerogel->AddMaterial(FusedSilica, (62.5 * perCent));
        Aerogel->AddMaterial(Water, (37.4 * perCent));
        Aerogel->AddElement(C, (0.1 * perCent));

        //------------- examples of gas in non STP conditions

        auto CO2 = new G4Material("CarbonicGas", (27. * mg / cm3), 2, kStateGas, (325. * kelvin), (50. * atmosphere));
        CO2->AddElement(C, 1);
        CO2->AddElement(O, 2);

        auto steam = new G4Material("WaterSteam", (1.0 * mg / cm3), 1, kStateGas, (273 * kelvin), (1 * atmosphere));
        steam->AddMaterial(Water, 1.);

        auto argonGas = new G4Material("ArgonGas", 18, (39.948 * g / mole), (1.782 * mg / cm3), kStateGas,
                                       273.15 * kelvin, 1 * atmosphere);

        // ------------- examples of vacuum

        Galactic = new G4Material("Galactic", 1., 1.008 * g / mole, universe_mean_density, kStateGas, (2.73 * kelvin), (3e-18 * pascal));

        Beamvacuum = new G4Material("Beam", (1e-5 * g / cm3), 1, kStateGas, STP_Temperature, (2e-2 * bar));
        Beamvacuum->AddMaterial(Air, 1.);


        Vacuum = new G4Material("Vacuum", 1., (1.01 * g / mole), (1.e-5 * g / cm3), kStateGas, (293 * kelvin),
                                (2.e-10 * bar));

        // General plastic (BC408 scintillator density, used in my simulations)
        Plastic = new G4Material("Plastic", (1.06 * g / cm3), 2);   // 1.032 - BC408
        Plastic->AddElement(H, 7.74 * perCent);  // 14
        Plastic->AddElement(C, 92.26 * perCent);  // 86


        // Antico from CERN workshop
        Antico = new G4Material("Antico", (2.7 * g / cm3), 9);
        Antico->AddElement(Al, 96.25 * perCent);
        Antico->AddElement(Cu, 0.1 * perCent);
        Antico->AddElement(Si, 1. * perCent);
        Antico->AddElement(Mn, 0.7 * perCent);
        Antico->AddElement(Mg, 0.9 * perCent);
        Antico->AddElement(Zn, 0.2 * perCent);
        Antico->AddElement(Fe, 0.5 * perCent);
        Antico->AddElement(Ti, 0.1 * perCent);
        Antico->AddElement(Cr, 0.25 * perCent);

        SiM = manager->FindOrBuildMaterial("G4_Si");
        sisfe.DefineMaterials(Galactic, LiqHe, SiM);
        // Print all the materials defined.
        G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
        G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    }


    G4VPhysicalVolume *DetectorConstruction::Construct() {

///---------------------------------------------------------------------------
///         World
///---------------------------------------------------------------------------


        G4double worldX;
        G4double worldY;
        G4double worldZ;

        if ((fWorldLength.x() != 0.) && (fWorldLength.y() != 0.) && (fWorldLength.z() != 0.)) {
            worldX = fWorldLength.x();
            worldY = fWorldLength.y();
            worldZ = fWorldLength.z();
        } else {
            worldX = 50.0 * cm;
            worldY = 50.0 * cm;
            worldZ = 50.0 * cm;
        }

        G4cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  " << G4endl;
        G4cout << "WORLD DIMENSIONS     : " << worldX << ", " << worldY << ", " << worldZ << G4endl;
        G4cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  " << G4endl;

        solidWorld = new G4Box("World", (0.5 * worldX), (0.5 * worldY), (0.5 * worldZ));
        logicWorld = new G4LogicalVolume(solidWorld, Galactic, "World", nullptr, nullptr, nullptr);
        physiWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);


///-----------------------------------------------------------------------------
///         Script generated simple boxes and tubes placement
///-----------------------------------------------------------------------------

        for (const auto &vol: fVolumes) {
            auto mother = G4LogicalVolumeStore::GetInstance()->GetVolume(vol.mother);
            if (!mother) {
                G4cout << "<><><><><> ERROR: Mother named: " << vol.mother << " was not found!" << G4endl;
                exit(1);
            }

            bool replica = false;
            for (const auto &rep: fReplica) {
                if (rep.name == vol.name) {
                    replica = true;
                }
            }
            if (replica) {
                continue;
            }


            new G4PVPlacement(vol.rot,      // rotation of daughter frame
                              vol.pos,      // position
                              vol.logicVol, // logical volume
                              vol.name,     // name
                              mother,       // mother volume
                              false,        // no boolean operation
                              0,            // copy number
                              true);        // check for overlaps

            G4cout << ">>>>>>>>>> name     : " << vol.name << G4endl;
            G4cout << "           kind     : " << vol.logicVol->GetSolid()->GetEntityType() << G4endl;
            G4cout << "           material : " << vol.logicVol->GetMaterial()->GetName() << G4endl;
            G4cout << "           denstity : " << vol.logicVol->GetMaterial()->GetDensity() / (g / cm3) << " g/cm3"
                   << G4endl;
            G4cout << "           position  : " << vol.pos.x() << ", " << vol.pos.y() << ", " << vol.pos.z() << " mm"
                   << G4endl;
            G4cout << "           rotation  : " << vol.rot->yz() / deg << " deg [X] " << vol.rot->xz() / deg
                   << " deg [Y] " << vol.rot->xy() / deg << " deg [Z]" << G4endl;
            G4cout << "           mother    : " << mother->GetName() << G4endl;
        }


///-----------------------------------------------------------------------------
///         Script generated periodic placement wo
///-----------------------------------------------------------------------------

        for (const auto &rep: fReplica) {
            auto replica = G4LogicalVolumeStore::GetInstance()->GetVolume(rep.name);
            if (!replica) {
                G4cout << "<><><><><> ERROR: Object to be replicated named: " << rep.name << " was not registered!"
                       << G4endl;
                exit(1);
            }

            if (!((rep.type == "lin") || (rep.type == "rot"))) {
                G4cout << "<><><><><> ERROR: Replica type: " << rep.type << " is invalid, options: lin, rot" << G4endl;
                exit(1);
            }

            G4ThreeVector pos;
            G4RotationMatrix *rot = nullptr;
            G4LogicalVolume *mother = nullptr;

            for (const auto &vol: fVolumes) {
                if (rep.name == vol.name) {
                    mother = G4LogicalVolumeStore::GetInstance()->GetVolume(vol.mother);
                    if (!mother) {
                        G4cout << "<><><><><> ERROR: Mother named: " << vol.mother << " was not found!" << G4endl;
                        exit(1);
                    }
                    pos = vol.pos;
                    rot = vol.rot;
                    break;
                }
            }


            for (G4int j = 0; j < rep.num; ++j) {
                if (rep.type == "lin") {
                    if (j != 0) {
                        pos += rep.shift;
                    }
                } else if (rep.type == "rot") {
                    if (j != 0) {
                        rot->rotateX(rep.shift.x());
                        rot->rotateY(rep.shift.y());
                        rot->rotateZ(rep.shift.z());
                    }
                }


                new G4PVPlacement(rot,      // rotation of daughter frame
                                  pos,      // position
                                  replica,  // logical volume
                                  rep.name, // name
                                  mother,   // mother volume
                                  false,    // no boolean operation
                                  j,        // copy number
                                  true);    // check for overlaps
            }

            G4cout << ">>>>>>>>>> group name : " << rep.name << G4endl;
            G4cout << "           kind     : " << replica->GetSolid()->GetEntityType() << G4endl;
            G4cout << "           material : " << replica->GetMaterial()->GetName() << G4endl;
            G4cout << "           denstity : " << replica->GetMaterial()->GetDensity() / (g / cm3) << " g/cm3"
                   << G4endl;
            G4cout << "           1st position  : " << pos.x() << ", " << pos.y() << ", " << pos.z() << " mm" << G4endl;
            G4cout << "           1st rotation  : " << rot->yz() / deg << " deg [X] " << rot->xz() / deg << " deg [Y] "
                   << rot->xy() / deg << " deg [Z]" << G4endl;
            G4cout << "           mother    : " << mother->GetName() << G4endl;
            G4cout << "           number of replicas    : " << rep.num << G4endl;
            G4cout << "           type of replicas    : " << rep.type << G4endl;
            G4cout << "           shift of replicas    : " << rep.shift.x() << " [X], " << rep.shift.y() << " [Y], "
                   << rep.shift.z() << " [Z] " << G4endl;
        }



///-----------------------------------------------------------------------------
///         Script generated boolean volumes placement
///-----------------------------------------------------------------------------

        if (fBoolMothers.size() <= fBoolVolumes.size()) {
            G4cout << "##########  Boolean mothers: " << fBoolMothers.size() << " pieces, daughters: "
                   << fBoolVolumes.size() << G4endl;

            for (auto &&mother: fBoolMothers) {
                for (const auto &vol: fBoolVolumes) {
                    if (vol.mother == mother.name) {
                        if (!mother.solid) {
                            G4cout << "<><><><><> ERROR: Mother named: " << vol.mother << " for boolean was not found!"
                                   << G4endl;
                            exit(1);
                        }

                        if (vol.type == "add") {
                            G4cout << "----------  adding: " << vol.name << " to motherSolid "
                                   << mother.solid->GetName() << G4endl;
                            mother.solid = new G4UnionSolid(vol.name, mother.solid, vol.solid, vol.rot, vol.pos);
                        } else if (vol.type == "sub") {
                            G4cout << "----------  subtracting : " << vol.name << " to motherSolid "
                                   << mother.solid->GetName() << G4endl;
                            mother.solid = new G4SubtractionSolid(vol.name, mother.solid, vol.solid, vol.rot, vol.pos);
                        } else if (vol.type == "inter") {
                            G4cout << "----------  intersecting : " << vol.name << " with motherSolid "
                                   << mother.solid->GetName() << G4endl;
                            mother.solid = new G4IntersectionSolid(vol.name, mother.solid, vol.solid, vol.rot, vol.pos);
                        } else {
                            G4cout << "<><><><><> ERROR: Boolean type: " << vol.type << " was not found!" << G4endl;
                            exit(1);
                        }
                    }
                }
            }


            for (const auto &vol: fBoolMothers) {
                auto mother = G4LogicalVolumeStore::GetInstance()->GetVolume(vol.mother);
                if (!mother) {
                    G4cout << "<><><><><> ERROR: Mother named: " << vol.mother << " was not found!" << G4endl;
                    exit(1);
                }

                auto lVol = new G4LogicalVolume(vol.solid, vol.mat, vol.name);

                new G4PVPlacement(vol.rot,  // rotation
                                  vol.pos,  // position
                                  lVol,     // logical volume
                                  vol.name, // name
                                  mother,   // mother volume
                                  false,    // no boolean operation
                                  0,        // copy number
                                  true);    // check for overlaps

                G4cout << ">>>>>>>>>> name     : " << vol.name << G4endl;
                G4cout << "           kind     : " << lVol->GetSolid()->GetEntityType() << G4endl;
                G4cout << "           material : " << lVol->GetMaterial()->GetName() << G4endl;
                G4cout << "           position  : " << vol.pos.x() << ", " << vol.pos.y() << ", " << vol.pos.z()
                       << " mm" << G4endl;
                G4cout << "           rotation  : " << vol.rot->yz() << " rad in X, " << vol.rot->xz() << " rad in Y, "
                       << vol.rot->xy() << " rad in Z" << G4endl;
                G4cout << "           mother    : " << mother->GetName() << G4endl;

            }

        } else {
            G4cout << "<><><><><> ERROR: some boolean mothers missing children!" << G4endl;
            exit(1);
        }

///--------------------------------------------------------------------------------- 
///                         Sensitive detectors
///--------------------------------------------------------------------------------- 



        auto SDman = G4SDManager::GetSDMpointer();

        G4String trackerDetectorSDname = "muonium/TrackerDetectorSD";
        auto trackerSD = new TrackerSD(trackerDetectorSDname);

        SDman->AddNewDetector(trackerSD);

        for (const auto &detName: fDetName) {
            auto log = G4LogicalVolumeStore::GetInstance()->GetVolume(detName);
            if (!log) {
                G4cout << "<><><><><> ERROR: Logical volume for sensitive detector " << detName << " was not found!"
                       << G4endl;
                exit(1);
            }
            log->SetSensitiveDetector(trackerSD);
            G4cout << ">>>>>>>>> Sensitive detector: "
                   << G4LogicalVolumeStore::GetInstance()->GetVolume(detName)->GetName() << " is set" << G4endl;
        }

//---------------------------- Visualization attributes -------------------------------


        logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

        auto RedVisAtt = new G4VisAttributes(G4Colour(1.0, 0.2, 0.2));
        auto BlueVisAtt = new G4VisAttributes(G4Colour(0.0, 127, 255));
        auto GreenVisAtt = new G4VisAttributes(G4Colour(0.5, 1.0, 0.5));
        auto YellowVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.));
        auto MagentaVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.));

        for (const auto &coldef: fColors) {
            auto vol = G4LogicalVolumeStore::GetInstance()->GetVolume(coldef.vol);
            if (vol) {
                if (coldef.col == "red") vol->SetVisAttributes(RedVisAtt);
                else if (coldef.col == "green") vol->SetVisAttributes(GreenVisAtt);
                else if (coldef.col == "blue") vol->SetVisAttributes(BlueVisAtt);
                else if (coldef.col == "yellow") vol->SetVisAttributes(YellowVisAtt);
                else if (coldef.col == "magenta") vol->SetVisAttributes(MagentaVisAtt);
                else if (coldef.col == "invisible") vol->SetVisAttributes(G4VisAttributes::GetInvisible());
            } else {
                G4cout << "<><><><><> WARNING: Logical volume >" << coldef.col << "< does not exist for color command "
                       << G4endl;
            }
        }
//--------------------------------------Limits ----------------------------------------

        // below is an example of how to set tracking constraints in a given
        // logical volume(see also in N02PhysicsList how to setup the processes
        // G4StepLimiter or G4UserSpecialCuts).
        // Sets a max Step length in the tracker region, with G4StepLimiter
        //

        G4double maxStep = 2 * mm;
        stepLimit = new G4UserLimits(maxStep);
        logicWorld->SetUserLimits(stepLimit);

        G4double maxsmallStep = 0.01 * mm;
        smallstepLimit = new G4UserLimits(maxsmallStep);

        for (const auto &smallStep: fSmallStep) {
            auto vol = G4LogicalVolumeStore::GetInstance()->GetVolume(smallStep.volume);
            if (vol) {
                vol->SetUserLimits(new G4UserLimits(smallStep.maxStepLength));
            } else {
                G4cout << "<><><><><> ERROR: Logical volume >" << smallStep.volume
                       << "< does not exist for step command " << G4endl;
                exit(1);
            }
        }
//--------------------------------------Sisfe grid construction ----------------------------------------
        for(const auto &fSisfeParams : fSisfeParamsV)
        if(fSisfeParams.isPlaced){
            auto sisfeMother = G4LogicalVolumeStore::GetInstance()->GetVolume(fSisfeParams.mother);
            auto gridRot = new G4RotationMatrix();
            gridRot->rotateX(fSisfeParams.rot.x() * deg);
            gridRot->rotateY(fSisfeParams.rot.y() * deg);
            gridRot->rotateZ(fSisfeParams.rot.z() * deg);
            sisfe.SetNameID(fSisfeParams.name);
            if(fSisfeColParams.isInv){
                sisfe.SetColours(fSisfeColParams.ContainerCol, fSisfeColParams.LiqHeCol, fSisfeColParams.SiCol);
            }
            sisfe.MakeGeometry(sisfeMother, fSisfeParams.nLiqHe, fSisfeParams.sizeLiqHe.x(),  fSisfeParams.sizeLiqHe.y(),  fSisfeParams.sizeLiqHe.z(), fSisfeParams.sizeSi.x(),  fSisfeParams.sizeSi.y(),  fSisfeParams.sizeSi.z(), fSisfeParams.pos, gridRot);
        }
//----------------------------------------------------------------------------
        return physiWorld;
    }


    void DetectorConstruction::SetMaxStep(const G4double maxStep) {
        if (stepLimit && (maxStep > 0.)) {
            stepLimit->SetMaxAllowedStep(maxStep);
        }
    }


    void DetectorConstruction::SetBoxDefinition(const DetBoxTubsDefinition &params) {

        auto mat = G4NistManager::Instance()->FindOrBuildMaterial(params.mat);

        if (mat && (fVolumes.size() < kMaxVolumes - 1)) {
            auto rot = new G4RotationMatrix();
            rot->rotateX(params.rot.x() * deg);
            rot->rotateY(params.rot.y() * deg);
            rot->rotateZ(params.rot.z() * deg);

            auto sBox = new G4Box(params.name, (params.size.x() / 2.), (params.size.y() / 2.), (params.size.z() / 2.));

            if (params.booltype == "A") {
                auto lBox = new G4LogicalVolume(sBox, mat, params.name);
                fVolumes.push_back(DetVolume{lBox, params.name, params.pos, rot, params.mother});
            } else if (params.booltype == "B") {
                fBoolMothers.push_back(DetBoolVolume{sBox, params.name, mat, params.pos, rot, params.mother, ""});
            } else if ((params.booltype == "add") || (params.booltype == "sub") || (params.booltype == "inter")) {
                fBoolVolumes.push_back(
                        DetBoolVolume{sBox, params.name, mat, params.pos, rot, params.mother, params.booltype});
            } else {
                G4cout << "<><><><><><> ERROR: bool type named " << params.booltype
                       << " not valid, options: A (alone); B (mother of bool); add, sub, inter with mother " << G4endl;
                exit(1);
            }

            G4RunManager::GetRunManager()->PhysicsHasBeenModified();
        } else {
            if (!mat) {
                G4cout << "<><><><><><> ERROR: material named " << params.mat << " not found" << G4endl;
            }
            if (fVolumes.size() >= (kMaxVolumes - 1)) {
                G4cout << "<><><><><><> ERROR: box exceeds max volumes number" << G4endl;
            }
            exit(1);
        }
    }


    void DetectorConstruction::SetTubsDefinition(const DetBoxTubsDefinition &params) {

        auto mat = G4NistManager::Instance()->FindOrBuildMaterial(params.mat);

        if (mat && (fVolumes.size() < kMaxVolumes - 1)) {

            auto rot = new G4RotationMatrix();
            rot->rotateX(params.rot.x() * deg);
            rot->rotateY(params.rot.y() * deg);
            rot->rotateZ(params.rot.z() * deg);

            auto sTubs = new G4Tubs(params.name, params.size.x(), params.size.y(), (params.size.z() / 2.), 0. * deg,
                                    360. * deg);

            if (params.booltype == "A") {
                auto lTubs = new G4LogicalVolume(sTubs, mat, params.name);
                fVolumes.push_back(DetVolume{lTubs, params.name, params.pos, rot, params.mother});
            } else if (params.booltype == "B") {
                fBoolMothers.push_back(DetBoolVolume{sTubs, params.name, mat, params.pos, rot, params.mother, ""});
            } else if ((params.booltype == "add") || (params.booltype == "sub") || (params.booltype == "inter")) {
                fBoolVolumes.push_back(
                        DetBoolVolume{sTubs, params.name, mat, params.pos, rot, params.mother, params.booltype});
            } else {
                G4cout << "<><><><><><> ERROR: bool type named " << params.booltype
                       << " not valid, options: A (alone); B (mother of bool); add, sub, inter with mother " << G4endl;
                exit(1);
            }

            G4RunManager::GetRunManager()->PhysicsHasBeenModified();
        } else {
            if (!mat) {
                G4cout << "<><><><><><> ERROR: material named " << params.mat << " not found" << G4endl;
            }
            if (fVolumes.size() >= kMaxVolumes - 1) {
                G4cout << "<><><><><><> ERROR: tubs exceeds max volumes number" << G4endl;
            }
            exit(1);
        }

    }


    void DetectorConstruction::SetRepDefinition(const DetReplica &replica) {
        fReplica.push_back(replica);
    }


    void DetectorConstruction::SetDetDefinition(const G4String &name) {
        fDetName.push_back(name);
    }

    void DetectorConstruction::SetColDefinition(const DetColDef &colDef) {
        fColors.push_back(colDef);
    }

    void DetectorConstruction::SetSmallStep(const DetMaxStepLength &maxStepLength) {
        fSmallStep.push_back(maxStepLength);
    }

    void DetectorConstruction::SetWorldLength(const G4ThreeVector &worldLength) {
        fWorldLength = worldLength;
    }

    void DetectorConstruction::UpdateGeometry() {
        G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    }


    void DetectorConstruction::SetSisfe(const SisfeGeometryDefinition &params){
        SisfeGeometryDefinition fSisfeParams;
        fSisfeParams.name = params.name;
        fSisfeParams.nLiqHe = params.nLiqHe;
        fSisfeParams.sizeLiqHe = params.sizeLiqHe;
        fSisfeParams.sizeSi = params.sizeSi;
        fSisfeParams.pos = params.pos;
        fSisfeParams.rot = params.rot;
        fSisfeParams.mother = params.mother;
        fSisfeParams.isPlaced = params.isPlaced;
        fSisfeParamsV.push_back(fSisfeParams);
    }
    void DetectorConstruction::SetSisfeColour(const SisfeColDefinition &params){
       
        fSisfeColParams.ContainerCol = params.ContainerCol;
        fSisfeColParams.LiqHeCol = params.LiqHeCol;
        fSisfeColParams.SiCol = params.SiCol;
        fSisfeColParams.isInv = params.isInv;
        

    }
}

