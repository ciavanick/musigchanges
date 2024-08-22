#include "musigSisfe.h"

namespace MuSiG {

sisfeGeometry::sisfeGeometry()
{
    // setting every block colour to white
    m_colorContainer = new G4VisAttributes(G4Colour(1, 1, 1));
    m_colorLiqHe = new G4VisAttributes(G4Colour(1, 1, 1));
    m_colorSi = new G4VisAttributes(G4Colour(1, 1, 1));
}

sisfeGeometry::sisfeGeometry(G4String nameID)
{
    // setting names
    SetNameID(nameID);

    // setting every block colour to white
    m_colorContainer = new G4VisAttributes(G4Colour(1, 1, 1));
    m_colorLiqHe = new G4VisAttributes(G4Colour(1, 1, 1));
    m_colorSi = new G4VisAttributes(G4Colour(1, 1, 1));
}

sisfeGeometry::sisfeGeometry(G4LogicalVolume *logicWorld, G4String nameID, G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot)
{
    // setting names
    SetNameID(nameID);
    // Define Materials
    DefineMaterials();
    // make the Geometry
    MakeGeometry(logicWorld, nLiqHe, LiqHeDimX, LiqHeDimY, LiqHeDimZ, SiDimX, SiDimY, SiDimZ, position, rot);
}

sisfeGeometry::~sisfeGeometry() {}

void sisfeGeometry::Geometry(G4LogicalVolume *logicWorld)
{
    // creating the geometry of the container
    m_solidContainer = new G4Box(m_nameSolidContainer, 0.5 * m_WorldDimX, 0.5 * m_WorldDimY, 0.5 * m_WorldDimZ);
    m_logicContainer = new G4LogicalVolume(m_solidContainer, m_Vacuum, m_nameLogicContainer, nullptr, nullptr, nullptr);
    m_logicContainer->SetVisAttributes(m_colorContainer);
    m_physContainer = new G4PVPlacement(m_rot, m_position, m_logicContainer, m_namePhysContainer, logicWorld, false, 0, true);

    // placing the LiqHe and Si columns
    m_solidLiqHe = new G4Box(m_nameSolidLiqHe, 0.5 * m_LiqHeDimX, 0.5 * m_LiqHeDimY, 0.5 * m_LiqHeDimZ);

    m_solidSi = new G4Box(m_nameSolidSi, 0.5 * m_SiDimX, 0.5 * m_SiDimY, 0.5 * m_SiDimZ);

    const auto nTot = m_nLiqHe + m_nSi;
    const auto start = -m_WorldDimX / 2 + m_SiDimX / 2;
    const auto step = m_SiDimX / 2 + m_LiqHeDimX / 2;
    // loop from 0 to the num of pillars and for every even index a Si pillar is placed, while for every odd index a LiqHe block is placed
    for (G4int i = 0; i != nTot; ++i)
    {
        if (i % 2 == 0)
        {
            m_logicSi = new G4LogicalVolume(m_solidSi, m_Si, m_nameLogicSi, nullptr, nullptr, nullptr);
            m_logicSi->SetVisAttributes(m_colorSi);
            m_physSi = new G4PVPlacement(0, G4ThreeVector(start + i * step, 0., 0.), m_logicSi, m_namePhysSi, m_logicContainer, false, i, true);
        }
        else
        {
            m_logicLiqHe = new G4LogicalVolume(m_solidLiqHe, m_LiqHe, m_nameLogicLiqHe, nullptr, nullptr, nullptr);
            m_logicLiqHe->SetVisAttributes(m_colorLiqHe);
            m_physLiqHe = new G4PVPlacement(0, G4ThreeVector(start + i * step, -(m_SiDimY / 2 - m_LiqHeDimY / 2), 0.), m_logicLiqHe, m_namePhysLiqHe, m_logicContainer, false, i, true);
        }
    }
}

void sisfeGeometry::SetNameID(G4String nameID)
{
    // setting namesID
    m_nameID = nameID;
    m_nameSolidContainer = nameID + "solidContainer";
    m_nameLogicContainer = nameID + "logicContainer";
    m_namePhysContainer = nameID + "phyContainer";

    m_nameSolidLiqHe = nameID + "solidLiqHe";
    m_nameLogicLiqHe = nameID + "logicLiqHe";
    m_namePhysLiqHe = nameID;

    m_nameSolidSi = nameID + "solidSi";
    m_nameLogicSi = nameID + "logicSi";
    m_namePhysSi = nameID + "phySi";
}

void sisfeGeometry::DefineMaterials()
{
    G4NistManager *nist = G4NistManager::Instance();
    m_Vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    m_Si = nist->FindOrBuildMaterial("G4_Si");
    m_LiqHe = new G4Material("sisfeLiqHe", 2, (4.0 * g / mole), (0.145 * g / cm3));
}
void sisfeGeometry::DefineMaterials(G4Material* Vacuum, G4Material* LiqHe, G4Material* Si)
{
    m_Vacuum = Vacuum;
    m_Si = Si;
    m_LiqHe = LiqHe;
}
void sisfeGeometry::DefineMaterials(G4String nameID)
{
    // setting names
    SetNameID(nameID);
    // defining materials
    DefineMaterials();
}

void sisfeGeometry::MakeGeometry(G4LogicalVolume *logicWorld, G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot)
{
    if (m_nameID == "")
    {
        G4cout << "<><><><><> ERROR: no name for sisfe object \n";
        exit(1);
    }
    // setting LiqHe dimensions
    m_nLiqHe = nLiqHe;
    m_LiqHeDimX = LiqHeDimX;
    m_LiqHeDimY = LiqHeDimY;
    m_LiqHeDimZ = LiqHeDimZ;
    // setting Si dimensions
    m_nSi = nLiqHe + 1;
    m_SiDimX = SiDimX;
    m_SiDimY = SiDimY;
    m_SiDimZ = SiDimZ;

    // check if the dimensions are positive
    if (m_LiqHeDimX < 0. || m_LiqHeDimY < 0. || m_LiqHeDimZ < 0.)
    {
        G4cout << "<><><><><> ERROR: invalid LiqHe colum dimensions \n";
        exit(1);
    }
    if (m_SiDimX < 0. || m_SiDimY < 0. || m_SiDimZ < 0.)
    {
        G4cout << "<><><><><> ERROR: invalid Si colum dimensions \n";
        exit(1);
    }

    // check if the number of LiqHe column are positive
    if (m_nLiqHe < 0)
    {
        G4cout << "<><><><><> ERROR: invalid number of columns \n";
        exit(1);
    }

    // setting the container (world) dimensions
    m_WorldDimX = m_nLiqHe * m_LiqHeDimX + m_nSi * m_SiDimX;
    if (SiDimY >= LiqHeDimY)
        m_WorldDimY = m_SiDimY;
    else
    {
        G4cout << "<><><><><> ERROR: invalid LiqHe column Y dimension \n";
        exit(1);
    }
    if (SiDimZ >= LiqHeDimZ)
        m_WorldDimZ = m_SiDimZ;
    else
    {
        G4cout << "<><><><><> ERROR: invalid LiqHe column Z dimension \n";
        exit(1);
    }

    m_position = position;
    m_rot = rot;
    Geometry(logicWorld);
}
void sisfeGeometry::MakeGeometry(G4LogicalVolume *logicWorld, G4String nameID, G4int nLiqHe, G4double LiqHeDimX, G4double LiqHeDimY, G4double LiqHeDimZ, G4double SiDimX, G4double SiDimY, G4double SiDimZ, G4ThreeVector position, G4RotationMatrix *rot)
{
    SetNameID(nameID);
    MakeGeometry(logicWorld, nLiqHe, LiqHeDimX, LiqHeDimY, LiqHeDimZ, SiDimX, SiDimY, SiDimZ, position, rot);
}

G4VisAttributes *sisfeGeometry::ifColors(G4String color)
{
    auto RedVisAtt = new G4VisAttributes(G4Colour(1.0, 0.2, 0.2));
    auto BlueVisAtt = new G4VisAttributes(G4Colour(0.0, 127, 255));
    auto GreenVisAtt = new G4VisAttributes(G4Colour(0.5, 1.0, 0.5));
    auto YellowVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.));
    auto MagentaVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.));
    auto WhiteVisAtt = new G4VisAttributes(G4Colour(1, 1, 1));
    auto InvisibleVisAtt = new G4VisAttributes(G4VisAttributes::GetInvisible());
    if (color == "Red" || color == "red" || color == "RED")
    {
        return RedVisAtt;
    }
    else if (color == "Blue" || color == "blue" || color == "BLUE")
    {
        return BlueVisAtt;
    }
    else if (color == "Green" || color == "green" || color == "GREEN")
    {
        return GreenVisAtt;
    }
    else if (color == "Yellow" || color == "yellow" || color == "YELLOW")
    {
        return YellowVisAtt;
    }
    else if (color == "Magenta" || color == "magenta" || color == "MAGENTA")
    {
        return MagentaVisAtt;
    }
    else if (color == "invisible" || color == "Invisible" || color == "INVISIBLE")
    {
        return InvisibleVisAtt;
    }
    return WhiteVisAtt;
}

void sisfeGeometry::SetContainerColour(G4String colorContainer)
{
    m_colorContainer = ifColors(colorContainer);
}
void sisfeGeometry::SetLiqHeColour(G4String colorLiqHe)
{
    m_colorLiqHe = ifColors(colorLiqHe);
}
void sisfeGeometry::SetSiColour(G4String colorSi)
{
    m_colorSi = ifColors(colorSi);
}

void sisfeGeometry::SetColours(G4String colorContainer, G4String colorLiqHe, G4String colorSi)
{
    SetContainerColour(colorContainer);
    SetLiqHeColour(colorLiqHe);
    SetSiColour(colorSi);
}

const G4String sisfeGeometry::GetNameID()
{
    return m_nameID;
}

const G4String sisfeGeometry::GetNameSolidContainer()
{
    return m_nameSolidContainer;
}
const G4String sisfeGeometry::GetNameLogicContainer()
{
    return m_nameLogicContainer;
}
const G4String sisfeGeometry::GetNamePhysContainer()
{
    return m_namePhysContainer;
}
const G4String sisfeGeometry::GetNameSolidLiqHe()
{
    return m_nameSolidLiqHe;
}
const G4String sisfeGeometry::GetNameLogicLiqHe()
{
    return m_nameLogicLiqHe;
}
const G4String sisfeGeometry::GetNamePhysLiqHe()
{
    return m_namePhysLiqHe;
}
const G4String sisfeGeometry::GetNameSolidSi()
{
    return m_nameSolidSi;
}
const G4String sisfeGeometry::GetNameLogicSi()
{
    return m_nameLogicSi;
}
const G4String sisfeGeometry::GetNamePhysSi()
{
    return m_namePhysSi;
}

const ThreeDimensions sisfeGeometry::GetContainerDimensions()
{
    return ThreeDimensions(m_WorldDimX, m_WorldDimY, m_WorldDimZ);
}
const ThreeDimensions sisfeGeometry::GetLiqHeDimensions()
{
    return ThreeDimensions(m_LiqHeDimX, m_LiqHeDimY, m_LiqHeDimZ);
}
const ThreeDimensions sisfeGeometry::GetSiDimensions()
{
    return ThreeDimensions(m_SiDimX, m_SiDimY, m_SiDimZ);
}

const G4Box *sisfeGeometry::GetSolidContainer()
{
    return m_solidContainer;
}
const G4Box *sisfeGeometry::GetSolidLiqHe()
{
    return m_solidLiqHe;
}
const G4Box *sisfeGeometry::GetSolidSi()
{
    return m_solidSi;
}

const G4LogicalVolume *sisfeGeometry::GetLogicalContainer()
{
    return m_logicContainer;
}
const G4LogicalVolume *sisfeGeometry::GetLogicalLiqHe()
{
    return m_logicLiqHe;
}
const G4LogicalVolume *sisfeGeometry::GetLogicalSi()
{
    return m_logicSi;
}

const G4VPhysicalVolume *sisfeGeometry::GetPhysicalVolumeContainer()
{
    return m_physContainer;
}
const G4VPhysicalVolume *sisfeGeometry::GetPhysicalVolumeLiqHe()
{
    return m_physLiqHe;
}
const G4VPhysicalVolume *sisfeGeometry::GetPhysicalVolumeSi()
{
    return m_physSi;
}

}