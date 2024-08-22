#include "musigDetectorMessenger.h"

#include <vector>
#include <tuple>

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "globals.hh"

#include "musigDetectorConstruction.h"


namespace MuSiG {


    DetectorMessenger::DetectorMessenger(DetectorConstruction *myDet) : G4UImessenger(), fDetector(myDet) {
        fSetupDir = new G4UIdirectory("/setup/");
        fSetupDir->SetGuidance("Geometry setup control.");

        fStepMaxCmd = new G4UIcmdWithADoubleAndUnit("/setup/stepMax", this);
        fStepMaxCmd->SetGuidance("Define a step max");
        fStepMaxCmd->SetParameterName("stepMax", false);
        fStepMaxCmd->SetUnitCategory("Length");
        fStepMaxCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        auto unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("mm"));

/////////////////// Set world size //////////////

        fWorldLengthCmd = new G4UIcommand("/setup/worldsize", this);
        fWorldLengthCmd->SetGuidance("Set x, y, z full lengths for world box length.");

        auto sizeWorldXPrm = new G4UIparameter("worldSizeX", 'd', false);
        sizeWorldXPrm->SetGuidance("sizeX");
        sizeWorldXPrm->SetParameterRange("worldSizeX>0.");
        fWorldLengthCmd->SetParameter(sizeWorldXPrm);

        auto sizeWorldYPrm = new G4UIparameter("worldSizeY", 'd', false);
        sizeWorldYPrm->SetGuidance("sizeY");
        sizeWorldYPrm->SetParameterRange("worldSizeY>0.");
        fWorldLengthCmd->SetParameter(sizeWorldYPrm);

        auto sizeWorldZPrm = new G4UIparameter("worldSizeZ", 'd', false);
        sizeWorldZPrm->SetGuidance("sizeZ");
        sizeWorldZPrm->SetParameterRange("worldSizeZ>0.");
        fWorldLengthCmd->SetParameter(sizeWorldZPrm);

        auto unitWorldSizePrm = new G4UIparameter("unitDimWorld", 's', false);
        unitWorldSizePrm->SetGuidance("unit of dimensions");
        unitWorldSizePrm->SetParameterCandidates(unitList);
        fWorldLengthCmd->SetParameter(unitWorldSizePrm);

        fWorldLengthCmd->AvailableForStates(G4State_PreInit);

//////////////////////////// Set boxes //////////////////////////

        fBoxDefCmd = new G4UIcommand("/setup/box", this);
        fBoxDefCmd->SetGuidance("Set box nb, box dimensions.");
        fBoxDefCmd->SetGuidance("  box number : from 0 to boxNumber");
        fBoxDefCmd->SetGuidance("  material name");
        fBoxDefCmd->SetGuidance("  dimensions (3-vector with unit)");

        auto boxNamePrm = new G4UIparameter("boxName", 's', false);
        boxNamePrm->SetGuidance("box name");
        fBoxDefCmd->SetParameter(boxNamePrm);

        auto boxMatPrm = new G4UIparameter("boxMat", 's', false);
        boxMatPrm->SetGuidance("Box material");
        fBoxDefCmd->SetParameter(boxMatPrm);

        auto sizeBoxXPrm = new G4UIparameter("boxSizeX", 'd', false);
        sizeBoxXPrm->SetGuidance("sizeX");
        sizeBoxXPrm->SetParameterRange("boxSizeX>0.");
        fBoxDefCmd->SetParameter(sizeBoxXPrm);

        auto sizeBoxYPrm = new G4UIparameter("boxSizeY", 'd', false);
        sizeBoxYPrm->SetGuidance("sizeY");
        sizeBoxYPrm->SetParameterRange("boxSizeY>0.");
        fBoxDefCmd->SetParameter(sizeBoxYPrm);

        auto sizeBoxZPrm = new G4UIparameter("boxSizeZ", 'd', false);
        sizeBoxZPrm->SetGuidance("sizeZ");
        sizeBoxZPrm->SetParameterRange("boxSizeZ>0.");
        fBoxDefCmd->SetParameter(sizeBoxZPrm);

        auto unitBoxSizePrm = new G4UIparameter("unitDimBox", 's', false);
        unitBoxSizePrm->SetGuidance("unit of dimensions");
        unitBoxSizePrm->SetParameterCandidates(unitList);
        fBoxDefCmd->SetParameter(unitBoxSizePrm);

        auto posiBoxXPrm = new G4UIparameter("boxPosiX", 'd', false);
        posiBoxXPrm->SetGuidance("position X");
        fBoxDefCmd->SetParameter(posiBoxXPrm);

        auto posiBoxYPrm = new G4UIparameter("boxPosiY", 'd', false);
        posiBoxYPrm->SetGuidance("position Y");
        fBoxDefCmd->SetParameter(posiBoxYPrm);

        auto posiBoxZPrm = new G4UIparameter("boxPosiZ", 'd', false);
        posiBoxZPrm->SetGuidance("position Z");
        fBoxDefCmd->SetParameter(posiBoxZPrm);

        auto unitBoxPosPrm = new G4UIparameter("unitBoxPos", 's', false);
        unitBoxPosPrm->SetGuidance("unit of position");
        unitBoxPosPrm->SetParameterCandidates(unitList);
        fBoxDefCmd->SetParameter(unitBoxPosPrm);

        auto rotBoxXPrm = new G4UIparameter("rotaBoxX", 'd', false);
        rotBoxXPrm->SetGuidance("rotation X");
        fBoxDefCmd->SetParameter(rotBoxXPrm);

        auto rotBoxYPrm = new G4UIparameter("rotaBoxY", 'd', false);
        rotBoxYPrm->SetGuidance("rotation Y");
        fBoxDefCmd->SetParameter(rotBoxYPrm);

        auto rotBoxZPrm = new G4UIparameter("rotaBoxZ", 'd', false);
        rotBoxZPrm->SetGuidance("rotation Z");
        fBoxDefCmd->SetParameter(rotBoxZPrm);

        auto boxMother = new G4UIparameter("boxMother", 's', false);
        boxMother->SetGuidance("mother volume");
        fBoxDefCmd->SetParameter(boxMother);

        auto boxBool = new G4UIparameter("boxBool", 's', false);
        boxBool->SetGuidance(
                "is it boolean? A = alone (not a boolean), B = mother of booleans, add = add to mother, sub = subtract from mother, inter = intersection with mother");
        fBoxDefCmd->SetParameter(boxBool);

        fBoxDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

//////////////////////////// Set tubes //////////////////////////

        fTubsDefCmd = new G4UIcommand("/setup/tubs", this);
        fTubsDefCmd->SetGuidance("Set tubs nb, and dimensions.");
        fTubsDefCmd->SetGuidance("  tubs number : from 0 to tubsNumber");
        fTubsDefCmd->SetGuidance("  material name");
        fTubsDefCmd->SetGuidance("  dimensions (3-vector with unit)");

        auto fTubsNbPrm = new G4UIparameter("tubsNb", 's', false);
        fTubsNbPrm->SetGuidance("Tubs name string");
        fTubsDefCmd->SetParameter(fTubsNbPrm);

        auto fTubsMatPrm = new G4UIparameter("tubsMat", 's', false);
        fTubsMatPrm->SetGuidance("Tubs material");
        fTubsDefCmd->SetParameter(fTubsMatPrm);

        auto sizeR0 = new G4UIparameter("r0", 'd', false);
        sizeR0->SetGuidance("r0 inner radius of tube, 0 if full tube");
        sizeR0->SetParameterRange("r0 >= 0.");
        fTubsDefCmd->SetParameter(sizeR0);

        auto sizeRmax = new G4UIparameter("rmax", 'd', false);
        sizeRmax->SetGuidance("rmax outer radius of tube");
        sizeRmax->SetParameterRange("rmax > 0.");
        fTubsDefCmd->SetParameter(sizeRmax);

        auto sizeHeight = new G4UIparameter("height", 'd', false);
        sizeHeight->SetGuidance("Full height (length) of tube");
        sizeHeight->SetParameterRange("height  > 0.");
        fTubsDefCmd->SetParameter(sizeHeight);

        auto unitTubsSizePrm = new G4UIparameter("unitTubsSize", 's', false);
        unitTubsSizePrm->SetGuidance("unit of dimensions");
        unitTubsSizePrm->SetParameterCandidates(unitList);
        fTubsDefCmd->SetParameter(unitTubsSizePrm);

        auto posiTubsXPrm = new G4UIparameter("posiTubsX", 'd', false);
        posiTubsXPrm->SetGuidance("position X");
        fTubsDefCmd->SetParameter(posiTubsXPrm);

        auto posiTubsYPrm = new G4UIparameter("posiTubsY", 'd', false);
        posiTubsYPrm->SetGuidance("position Y");
        fTubsDefCmd->SetParameter(posiTubsYPrm);

        auto posiTubsZPrm = new G4UIparameter("posiTubsZ", 'd', false);
        posiTubsZPrm->SetGuidance("position Z");
        fTubsDefCmd->SetParameter(posiTubsZPrm);

        auto unitTubsPosPrm = new G4UIparameter("unitTubsPos", 's', false);
        unitTubsPosPrm->SetGuidance("unit of position");
        unitTubsPosPrm->SetParameterCandidates(unitList);
        fTubsDefCmd->SetParameter(unitTubsPosPrm);

        auto rotTubsXPrm = new G4UIparameter("rotaTubsX", 'd', false);
        rotTubsXPrm->SetGuidance("rotation X");
        fTubsDefCmd->SetParameter(rotTubsXPrm);

        auto rotTubsYPrm = new G4UIparameter("rotaTubsY", 'd', false);
        rotTubsYPrm->SetGuidance("rotation Y");
        fTubsDefCmd->SetParameter(rotTubsYPrm);

        auto rotTubsZPrm = new G4UIparameter("rotaTubsZ", 'd', false);
        rotTubsZPrm->SetGuidance("rotation Z");
        fTubsDefCmd->SetParameter(rotTubsZPrm);

        auto tubsMother = new G4UIparameter("tubsMother", 's', false);
        tubsMother->SetGuidance("mother volume");
        fTubsDefCmd->SetParameter(tubsMother);

        auto tubsBool = new G4UIparameter("tubsBool", 's', false);
        tubsBool->SetGuidance(
                "is it boolean? A = alone (not a boolean), B = mother of booleans, add = add to mother, sub = subtract from mother, inter = intersection with mother");
        fTubsDefCmd->SetParameter(tubsBool);

        fTubsDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

//////////////////// Replicas ////////////////////////////////  

        fRepDefCmd = new G4UIcommand("/setup/replica", this);
        fRepDefCmd->SetGuidance("Replicate simple tubs or box");

        auto repNamePrm = new G4UIparameter("replicaName", 's', false);
        repNamePrm->SetGuidance("Set name of a box or tubs to be replicated which was defined EARLIER");
        fRepDefCmd->SetParameter(repNamePrm);

        auto repNrPrm = new G4UIparameter("number", 'i', false);
        repNrPrm->SetGuidance("Set number of replicas");
        fRepDefCmd->SetParameter(repNrPrm);

        auto repTypePrm = new G4UIparameter("replicaType", 's', false);
        repTypePrm->SetGuidance("Set type: lin or rot for replica transformation");
        fRepDefCmd->SetParameter(repTypePrm);

        auto shiftRepXPrm = new G4UIparameter("shiftRepX", 'd', false);
        shiftRepXPrm->SetGuidance("shift X");
        fRepDefCmd->SetParameter(shiftRepXPrm);

        auto shiftRepYPrm = new G4UIparameter("shiftRepY", 'd', false);
        shiftRepYPrm->SetGuidance("shift Y");
        fRepDefCmd->SetParameter(shiftRepYPrm);

        auto shiftRepZPrm = new G4UIparameter("shiftRepZ", 'd', false);
        shiftRepZPrm->SetGuidance("shift Z");
        fRepDefCmd->SetParameter(shiftRepZPrm);

        auto unitRepPosPrm = new G4UIparameter("unitRepPos", 's', false);
        unitRepPosPrm->SetGuidance("unit of shift");
        unitRepPosPrm->SetParameterCandidates(unitList);
        fRepDefCmd->SetParameter(unitRepPosPrm);

        fRepDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

//////////////////// Detectors ////////////////////////////////  

        fDetDefCmd = new G4UIcommand("/setup/detector", this);
        fDetDefCmd->SetGuidance("Set name of sensitive detector, one per line.");

        auto detNamePrm = new G4UIparameter("detectorName", 's', false);
        detNamePrm->SetGuidance("detector name, defined EARLIER as a box or tubs");
        fDetDefCmd->SetParameter(detNamePrm);
        fDetDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


//////////////////// Colors ////////////////////////////////  

        fColorDefCmd = new G4UIcommand("/setup/color", this);
        fColorDefCmd->SetGuidance("Name of volume plus color.");

        auto objNamePrm = new G4UIparameter("objName", 's', false);
        objNamePrm->SetGuidance("name of the the body");
        fColorDefCmd->SetParameter(objNamePrm);


        auto colNamePrm = new G4UIparameter("objCol", 's', false);
        colNamePrm->SetGuidance("red green blue yellow or invisible");
        fColorDefCmd->SetParameter(colNamePrm);

        fColorDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

//////////////////// Step limit ////////////////////////////////  

        fStepDefCmd = new G4UIcommand("/setup/steplimit", this);
        fStepDefCmd->SetGuidance("Set name of sensitive detector, one per line.");

        auto stepObjNamePrm = new G4UIparameter("objName", 's', false);
        stepObjNamePrm->SetGuidance("name of the the body");
        fStepDefCmd->SetParameter(stepObjNamePrm);

        auto stepNamePrm = new G4UIparameter("objCol", 's', false);
        stepNamePrm->SetGuidance("red green blue yellow or invisible");
        fStepDefCmd->SetParameter(stepNamePrm);

        fStepDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
//////////////////////////// Set Grid Colums Design //////////////

        fSisfeDefCmd = new G4UIcommand("/setup/sisfe", this);
        fSisfeDefCmd->SetGuidance("Set LiqHe and Si columns nb, and dimensions.");
        fSisfeDefCmd->SetGuidance("  columns number : from 0");
        fSisfeDefCmd->SetGuidance("  material name");
        fSisfeDefCmd->SetGuidance("  dimensions LiqHe columns (3-vector with unit)");
        fSisfeDefCmd->SetGuidance("  dimensions Si columns (3-vector with unit)");

        auto fNameIDPrm = new G4UIparameter("nameID", 's', false);
        fNameIDPrm->SetGuidance("Grid name string");
        fSisfeDefCmd->SetParameter(fNameIDPrm);

        auto fNLiqHePrm = new G4UIparameter("nLiqHe", 'i', false);
        fNLiqHePrm->SetGuidance("Number of LiqHe columns");
        fNLiqHePrm->SetParameterRange("nLiqHe >= 0");
        fSisfeDefCmd->SetParameter(fNLiqHePrm);

        auto LiqHeDimX = new G4UIparameter("LiqHeDimX", 'd', false);
        LiqHeDimX->SetGuidance("LiqHe column dimension X");
        fSisfeDefCmd->SetParameter(LiqHeDimX);

        auto LiqHeDimY = new G4UIparameter("LiqHeDimY", 'd', false);
        LiqHeDimY->SetGuidance("LiqHe column dimension Y");
        fSisfeDefCmd->SetParameter(LiqHeDimY);

        auto LiqHeDimZ = new G4UIparameter("LiqHeDimZ", 'd', false);
        LiqHeDimZ->SetGuidance("LiqHe column dimension Z");
        fSisfeDefCmd->SetParameter(LiqHeDimZ);

        auto unitLiqHeColumnSizePrm = new G4UIparameter("unitLiqHeColumnSize", 's', false);
        unitLiqHeColumnSizePrm->SetGuidance("unit of dimensions");
        unitLiqHeColumnSizePrm->SetParameterCandidates(unitList);
        fSisfeDefCmd->SetParameter(unitLiqHeColumnSizePrm);

        auto SiDimX = new G4UIparameter("SiDimX", 'd', false);
        SiDimX->SetGuidance("Si column dimension X");
        fSisfeDefCmd->SetParameter(SiDimX);

        auto SiDimY = new G4UIparameter("SiDimY", 'd', false);
        SiDimY->SetGuidance("Si column dimension Y");
        fSisfeDefCmd->SetParameter(SiDimY);

        auto SiDimZ = new G4UIparameter("SiDimZ", 'd', false);
        SiDimZ->SetGuidance("Si column dimension Z");
        fSisfeDefCmd->SetParameter(SiDimZ);

        auto unitSiColumnSizePrm = new G4UIparameter("unitSiColumnSize", 's', false);
        unitSiColumnSizePrm->SetGuidance("unit of dimensions");
        unitSiColumnSizePrm->SetParameterCandidates(unitList);
        fSisfeDefCmd->SetParameter(unitSiColumnSizePrm);

        auto posiGridXPrm = new G4UIparameter("posiGridX", 'd', false);
        posiGridXPrm->SetGuidance("position X");
        fSisfeDefCmd->SetParameter(posiGridXPrm);

        auto posiGridYPrm = new G4UIparameter("posiGridY", 'd', false);
        posiGridYPrm->SetGuidance("position Y");
        fSisfeDefCmd->SetParameter(posiGridYPrm);

        auto posiGridZPrm = new G4UIparameter("posiGridZ", 'd', false);
        posiGridZPrm->SetGuidance("position Z");
        fSisfeDefCmd->SetParameter(posiGridZPrm);

        auto unitGridPosPrm = new G4UIparameter("unitGridPos", 's', false);
        unitGridPosPrm->SetGuidance("unit of position");
        unitGridPosPrm->SetParameterCandidates(unitList);
        fSisfeDefCmd->SetParameter(unitGridPosPrm);

        auto rotGridXPrm = new G4UIparameter("rotaGridX", 'd', false);
        rotGridXPrm->SetGuidance("rotation X");
        fSisfeDefCmd->SetParameter(rotGridXPrm);

        auto rotGridYPrm = new G4UIparameter("rotaGridY", 'd', false);
        rotGridYPrm->SetGuidance("rotation Y");
        fSisfeDefCmd->SetParameter(rotGridYPrm);

        auto rotGridZPrm = new G4UIparameter("rotaGridZ", 'd', false);
        rotGridZPrm->SetGuidance("rotation Z");
        fSisfeDefCmd->SetParameter(rotGridZPrm);

        auto GridMother = new G4UIparameter("GridMother", 's', false);
        GridMother->SetGuidance("mother volume");
        fSisfeDefCmd->SetParameter(GridMother);

        fSisfeDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        //////////////////// Colors ////////////////////////////////

        fColorSisfeDefCmd = new G4UIcommand("/setup/color/sisfe", this);
        fColorSisfeDefCmd->SetGuidance("Name of volume plus color.");

        auto containerColNamePrm = new G4UIparameter("containerColor", 's', false);
        containerColNamePrm->SetGuidance("red green blue yellow magenta or invisible");
        fColorSisfeDefCmd->SetParameter(containerColNamePrm);

        auto LiqHeColNamePrm = new G4UIparameter("LiqHeColor", 's', false);
        LiqHeColNamePrm->SetGuidance("red green blue yellow magenta or invisible");
        fColorSisfeDefCmd->SetParameter(LiqHeColNamePrm);

        auto SiColNamePrm = new G4UIparameter("SiColor", 's', false);
        SiColNamePrm->SetGuidance("red green blue yellow magenta or invisible");
        fColorSisfeDefCmd->SetParameter(SiColNamePrm);

        fColorSisfeDefCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

////////////////////////////////////////////////////////////

        fUpdateCmd = new G4UIcmdWithoutParameter("/setup/update", this);
        fUpdateCmd->SetGuidance("Update the geometry.");
        fUpdateCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorMessenger::~DetectorMessenger() {
        delete fStepMaxCmd;
        delete fWorldLengthCmd;
        delete fTubsDefCmd;
        delete fBoxDefCmd;
        delete fRepDefCmd;
        delete fDetDefCmd;
        delete fColorDefCmd;
        delete fStepDefCmd;
        delete fUpdateCmd;
        delete fSetupDir;
    }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {

        if (command == fStepMaxCmd) {
            fDetector->SetMaxStep(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
        } else if (command == fBoxDefCmd) {
            G4String nam;
            G4String mat;
            G4double v1, v2, v3;
            G4String unt_size;
            G4double p1, p2, p3;
            G4String unt_pos;
            G4double d1, d2, d3;
            G4String mother;
            G4String isbool;

            std::istringstream is(newValue);

            is >> nam >> mat >> v1 >> v2 >> v3 >> unt_size >> p1 >> p2 >> p3 >> unt_pos >> d1 >> d2 >> d3 >> mother
               >> isbool;

            G4ThreeVector vec1(v1, v2, v3);
            vec1 *= G4UIcommand::ValueOf(unt_size);

            G4ThreeVector vec2(p1, p2, p3);
            vec2 *= G4UIcommand::ValueOf(unt_pos);

            G4ThreeVector vec3(d1, d2, d3);

            fDetector->SetBoxDefinition(DetBoxTubsDefinition{nam, mat, vec1, vec2, vec3, mother, isbool});

        } else if (command == fTubsDefCmd) {
            G4String nam;
            G4String mat;
            G4double r1, r2, h;
            G4String unt_size;
            G4double p1, p2, p3;
            G4String unt_pos;
            G4double d1, d2, d3;
            G4String mother;
            G4String isbool;

            std::istringstream is(newValue);

            is >> nam >> mat >> r1 >> r2 >> h >> unt_size >> p1 >> p2 >> p3 >> unt_pos >> d1 >> d2 >> d3 >> mother
               >> isbool;

            G4ThreeVector vec1(r1, r2, h);
            vec1 *= G4UIcommand::ValueOf(unt_size);

            G4ThreeVector vec2(p1, p2, p3);
            vec2 *= G4UIcommand::ValueOf(unt_pos);

            G4ThreeVector vec3(d1, d2, d3);

            fDetector->SetTubsDefinition(DetBoxTubsDefinition{nam, mat, vec1, vec2, vec3, mother, isbool});

        } else if (command == fRepDefCmd) {
            G4String nam;
            G4String typ;
            G4double v1, v2, v3;
            G4int num;
            G4String unt_pos;

            std::istringstream is(newValue);

            is >> nam >> num >> typ >> v1 >> v2 >> v3 >> unt_pos;

            G4ThreeVector vec1(v1, v2, v3);
            vec1 *= G4UIcommand::ValueOf(unt_pos);

            fDetector->SetRepDefinition(DetReplica{nam, num, typ, vec1});

        } else if (command == fWorldLengthCmd) {
            G4double d1, d2, d3;
            G4String dim;
            std::istringstream is(newValue);
            is >> d1 >> d2 >> d3 >> dim;

            G4ThreeVector vec1(d1, d2, d3);
            vec1 *= G4UIcommand::ValueOf(dim);

            fDetector->SetWorldLength(vec1);
        } else if (command == fDetDefCmd) {
            G4String nam;
            std::istringstream is(newValue);
            is >> nam;

            fDetector->SetDetDefinition(nam);
        } else if (command == fColorDefCmd) {
            G4String nam, color;
            std::istringstream is(newValue);
            is >> nam >> color;

            fDetector->SetColDefinition({nam, color});
        } else if (command == fStepDefCmd) {
            G4String nam;
            G4double step;
            std::istringstream is(newValue);
            is >> nam >> step;

            fDetector->SetSmallStep(DetMaxStepLength{nam, step});
        } else if (command == fSisfeDefCmd){
            G4String name;
            G4int nLiqHe;
            G4double LiqHeDimX, LiqHeDimY, LiqHeDimZ;
            G4String LiqHeSizeDim;
            G4double SiDimX, SiDimY, SiDimZ;
            G4String SiSizeDim;
            G4double posX, posY, posZ;
            G4String GridPosDim;
            G4double rotX, rotY, rotZ;
            G4String mother;

            std::istringstream is(newValue);
            is >> name >> nLiqHe >> LiqHeDimX >> LiqHeDimY >> LiqHeDimZ >> LiqHeSizeDim >> SiDimX >> SiDimY >> SiDimZ >> SiSizeDim >> posX >> posY >> posZ >> GridPosDim >> rotX >> rotY >> rotZ >> mother;

            G4ThreeVector sizeLiqHe(LiqHeDimX, LiqHeDimY, LiqHeDimZ);
            sizeLiqHe *= G4UIcommand::ValueOf(LiqHeSizeDim);

            G4ThreeVector sizeSi(SiDimX, SiDimY, SiDimZ);
            sizeSi *= G4UIcommand::ValueOf(SiSizeDim);

            G4ThreeVector pos(posX, posY, posZ);
            pos *= G4UIcommand::ValueOf(GridPosDim);

            G4ThreeVector rot(rotX, rotY, rotZ);

            fDetector->SetSisfe(SisfeGeometryDefinition{name, nLiqHe, sizeLiqHe, sizeSi, pos, rot, mother, true});
        } else if (command == fColorSisfeDefCmd){
            G4String containerCol, LiqHeCol, SiCol;
            std::istringstream is(newValue);

            is >> containerCol >> LiqHeCol >> SiCol;

            fDetector->SetSisfeColour(SisfeColDefinition{containerCol, LiqHeCol, SiCol, true});

        } else if (command == fUpdateCmd) {
            fDetector->UpdateGeometry();
        }

    }


}
