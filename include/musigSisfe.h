#ifndef SISFE_H
#define SISFE_H

#include <iostream>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4SystemOfUnits.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4RotationMatrix.hh>
#include <G4Colour.hh>

namespace MuSiG {

struct ThreeDimensions{
    public:
    ThreeDimensions(G4double x_in, G4double y_in, G4double z_in) :  x(x_in), y(y_in), z(z_in) {};

    G4double x;
    G4double y;
    G4double z;

};

class sisfeGeometry
{
public:
    sisfeGeometry();
    sisfeGeometry(G4String);
    sisfeGeometry(G4LogicalVolume *, G4String, G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot);
    ~sisfeGeometry();

    void DefineMaterials();
    void DefineMaterials(G4Material* Vacuum, G4Material* LiqHe, G4Material* Si);
    void DefineMaterials(G4String);
    void MakeGeometry(G4LogicalVolume *, G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot);
    void MakeGeometry(G4LogicalVolume *, G4String ,G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot);

    void SetNameID(G4String);
    void SetContainerColour(G4String colorContainer);
    void SetLiqHeColour(G4String colorLiqHe);
    void SetSiColour(G4String colorSi);
    void SetColours(G4String colorContainer, G4String colorLiqHe, G4String colorSi);

    const G4String GetNameID();
    const G4String GetNameSolidContainer();
    const G4String GetNameSolidLiqHe();
    const G4String GetNameSolidSi();
    const G4String GetNameLogicContainer();
    const G4String GetNameLogicLiqHe();
    const G4String GetNameLogicSi();
    const G4String GetNamePhysContainer();
    const G4String GetNamePhysLiqHe();
    const G4String GetNamePhysSi();

    const ThreeDimensions GetContainerDimensions();
    const ThreeDimensions GetLiqHeDimensions();
    const ThreeDimensions GetSiDimensions();

    const G4Box* GetSolidContainer();
    const G4Box* GetSolidLiqHe();
    const G4Box* GetSolidSi();

    const G4LogicalVolume* GetLogicalContainer();
    const G4LogicalVolume* GetLogicalLiqHe();
    const G4LogicalVolume* GetLogicalSi();

    const G4VPhysicalVolume* GetPhysicalVolumeContainer();
    const G4VPhysicalVolume* GetPhysicalVolumeLiqHe();
    const G4VPhysicalVolume* GetPhysicalVolumeSi();



private:
    void Geometry(G4LogicalVolume *logicWorld);
    G4VisAttributes* ifColors(G4String color);
    G4Material *m_Vacuum = nullptr;
    G4Material *m_Si = nullptr;
    G4Material *m_LiqHe = nullptr;
    G4Box *m_solidContainer = nullptr;
    G4Box *m_solidLiqHe = nullptr;
    G4Box *m_solidSi = nullptr;
    G4LogicalVolume *m_logicContainer = nullptr;
    G4LogicalVolume *m_logicLiqHe = nullptr;
    G4LogicalVolume *m_logicSi = nullptr;
    G4VPhysicalVolume *m_physContainer = nullptr;
    G4VPhysicalVolume *m_physLiqHe = nullptr;
    G4VPhysicalVolume *m_physSi = nullptr;
    // names
    G4String m_nameID = "";
    G4String m_nameSolidContainer = "";
    G4String m_nameLogicContainer = "";
    G4String m_namePhysContainer = "";
    G4String m_nameSolidLiqHe = "";
    G4String m_nameLogicLiqHe = "";
    G4String m_namePhysLiqHe = "";
    G4String m_nameSolidSi = "";
    G4String m_nameLogicSi = "";
    G4String m_namePhysSi = "";
    G4int m_nLiqHe = 0; // number of LiqHe pillars
    G4int m_nSi = 0;    // number of Si pillars
    // dimensions of LiqHe pillars
    G4double m_LiqHeDimX = 0.;
    G4double m_LiqHeDimY = 0.;
    G4double m_LiqHeDimZ = 0.;
    // dimensions of Si pillars
    G4double m_SiDimX = 0.;
    G4double m_SiDimY = 0.;
    G4double m_SiDimZ = 0.;
    // dimensions of the container block
    G4double m_WorldDimX = 0.;
    G4double m_WorldDimY = 0.;
    G4double m_WorldDimZ = 0.;
    // position of the container
    G4ThreeVector m_position = {0., 0., 0.};
    //rotation
    G4RotationMatrix *m_rot;
    // colour
    G4VisAttributes* m_colorContainer;
    G4VisAttributes* m_colorLiqHe;
    G4VisAttributes* m_colorSi;
};
}
#endif