#ifndef MUSIG_DETECTORCONSTRUCTION_H
#define MUSIG_DETECTORCONSTRUCTION_H


#include <vector>
#include <tuple>

#include <G4VUserDetectorConstruction.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4UserLimits.hh>

#include "musigDetectorMessenger.h"
#include "musigSisfe.h"

namespace MuSiG {


    class DetectorMessenger;


    typedef struct DetVolume {
        G4LogicalVolume *logicVol = nullptr;
        G4String name;
        G4ThreeVector pos;
        G4RotationMatrix *rot;
        G4String mother;
    } DetVolume;


    typedef struct DetReplica {
        G4String name;
        G4int num = 0;
        G4String type;
        G4ThreeVector shift;
    } DetReplica;


    typedef struct DetBoolVolume {
        G4VSolid *solid = nullptr;
        G4String name;
        G4Material *mat = nullptr;
        G4ThreeVector pos;
        G4RotationMatrix *rot = nullptr;
        G4String mother;
        G4String type;
    } DetBoolVolume;


    typedef struct DetMaxStepLength {
        G4String volume;
        G4double maxStepLength;
    } DetMaxStepLength;


    typedef struct DetColDef {
        G4String vol;
        G4String col;
    } DetColDef;


    typedef struct DetBoxTubsDefinition {
        G4String name;
        G4String mat;
        G4ThreeVector size;
        G4ThreeVector pos;
        G4ThreeVector rot;
        G4String mother;
        G4String booltype;
    } DetBoxTubsDefinition;

    typedef struct SisfeGeometryDefinition {
        G4String name;
        G4int nLiqHe;
        G4ThreeVector sizeLiqHe;
        G4ThreeVector sizeSi;
        G4ThreeVector pos;
        G4ThreeVector rot;
        G4String mother;
        G4bool isPlaced=false;
    } SisfeGeometryDefinition;

    typedef struct SisfeColDefinition {
        G4String ContainerCol;
        G4String LiqHeCol;
        G4String SiCol;
        G4bool isInv=false;
    } SisfeColDefinition;

    const G4int kMaxVolumes = 50;

    class DetectorConstruction : public G4VUserDetectorConstruction {
    public:

        DetectorConstruction();

        ~DetectorConstruction() override;

    public:
        G4VPhysicalVolume *Construct() override;

        void SetMaxStep(G4double);

        void UpdateGeometry();

        void SetBoxDefinition(const DetBoxTubsDefinition &);

        void SetTubsDefinition(const DetBoxTubsDefinition &);

        void SetDetDefinition(const G4String &);

        void SetRepDefinition(const DetReplica &);

        void SetColDefinition(const DetColDef &);

        void SetWorldLength(const G4ThreeVector &);

        void SetSmallStep(const DetMaxStepLength &);

        void SetSisfe(const SisfeGeometryDefinition &);
        void SetSisfeColour(const SisfeColDefinition &);

    private:
        void DefineMaterials();

        G4Box *solidWorld = nullptr;
        G4LogicalVolume *logicWorld = nullptr;
        G4VPhysicalVolume *physiWorld = nullptr;

        G4UserLimits *stepLimit = nullptr;
        G4UserLimits *smallstepLimit = nullptr;
        DetectorMessenger *detectorMessenger = nullptr;

        G4ThreeVector fWorldLength;

        sisfeGeometry sisfe;
       
        std::vector<DetVolume> fVolumes;
        std::vector<DetReplica> fReplica;
        std::vector<DetBoolVolume> fBoolMothers;
        std::vector<DetBoolVolume> fBoolVolumes;

        std::vector<DetMaxStepLength> fSmallStep;

        std::vector<DetColDef> fColors;
        std::vector<G4String> fDetName;

        std::vector<SisfeGeometryDefinition> fSisfeParamsV;
        SisfeColDefinition fSisfeColParams;

        G4Material *Vacuum = nullptr;
        G4Material *LiqHe = nullptr;
        G4Material *GasHe = nullptr;
        G4Material *Copper = nullptr;
        G4Material *CNTforest = nullptr;
        G4Material *Mylar = nullptr;
        G4Material *Plastic = nullptr;
        G4Material *Titanium = nullptr;
        G4Material *Aluminium = nullptr;
        G4Material *Water = nullptr;
        G4Material *Scintillator = nullptr;
        G4Material *Berillium = nullptr;
        G4Material *Lithium = nullptr;
        G4Material *Silicon = nullptr;
        G4Material *FusedSilica = nullptr;
        G4Material *Aerogel = nullptr;
        G4Material *CsI = nullptr;
        G4Material *Lucite = nullptr;
        G4Material *BGO = nullptr;
        G4Material *Concrete = nullptr;
        G4Material *Antico = nullptr;
        G4Material *Beamvacuum = nullptr;
        G4Material *Galactic = nullptr;
        G4Material *Ar_gas = nullptr;
        G4Material *SiM = nullptr;
    };


}


#endif
