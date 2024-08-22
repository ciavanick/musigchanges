#ifndef MUSIG_DETECTORMESSENGER_H
#define MUSIG_DETECTORMESSENGER_H

#include <globals.hh>
#include <G4UImessenger.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithoutParameter.hh>

#include "musigDetectorConstruction.h"


namespace MuSiG {


    class DetectorConstruction;


    class DetectorMessenger : public G4UImessenger {
    public:
        explicit DetectorMessenger(DetectorConstruction *);

        ~DetectorMessenger() override;

        void SetNewValue(G4UIcommand *, G4String) override;   

    private:

        DetectorConstruction *fDetector;

        G4UIdirectory *fSetupDir = nullptr;

        G4UIcmdWithADoubleAndUnit *fStepMaxCmd = nullptr;

        G4UIcommand *fWorldLengthCmd = nullptr;
        G4UIcommand *fTubsDefCmd = nullptr;
        G4UIcommand *fBoxDefCmd = nullptr;
        G4UIcommand *fRepDefCmd = nullptr;
        G4UIcommand *fDetDefCmd = nullptr;
        G4UIcommand *fColorDefCmd = nullptr;
        G4UIcommand *fColorSisfeDefCmd = nullptr;
        G4UIcommand *fStepDefCmd = nullptr;
        G4UIcommand *fSisfeDefCmd = nullptr;

        G4UIcmdWithoutParameter *fUpdateCmd = nullptr;

    };


}


#endif

