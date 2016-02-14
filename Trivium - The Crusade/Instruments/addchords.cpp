#include <iostream>
#include <string>
#include <map>
#include <cstdlib>

#include "libgig/src/gig.h"

using namespace std;

void loadSamples(map<string, gig::Sample*>* mSamples, gig::File* gig, string groupName) {
    gig::Group* pGroup = gig->GetGroup(groupName);
    cout << "Group name: " << pGroup->Name << endl;

    gig::Sample* pSample = pGroup->GetFirstSample();
    while (pSample) {
        (*mSamples)[pSample->pInfo->Name] = pSample;
        pSample = pGroup->GetNextSample();
    }
}

void walkZones(gig::Region* pRegion, string* groups, map<string, gig::Sample*>* mSamples,
               uint8_t* zones, int iDimension, int iLayer, int iVelocity, int iKeySwitch) {
    if (iDimension < 0) {
        gig::DimensionRegion* pDimensionRegion = pRegion->GetDimensionRegionByBit(zones);
        cout << "    Unity Note: " << (uint)pDimensionRegion->UnityNote;
        cout << " - layer " << iLayer << " - " << "velocity " << iVelocity;
        gig::Sample* pSample = pDimensionRegion->pSample;
        cout << " - Sample: " << (pSample ? pSample->pInfo->Name : "NULL") << endl;

      	int unityNote = pRegion->KeyRange.low;
      	switch (iLayer) {
            case 1:
                unityNote += 7;
      	        break;
      	    case 2:
      	        unityNote += 12;
      	        break;
      	}

        if (iKeySwitch == 0 && iLayer > 0) {
            if (pDimensionRegion->pSample) {
                cout << "      New Sample: NULL" << endl;
                pDimensionRegion->pSample = NULL;
            }
        } else {
          	string sampleName = "norm" + groups[iVelocity] + " " + std::to_string(unityNote);
            gig::Sample* newSample = (*mSamples)[sampleName];

            if (newSample && newSample != pSample) {
                cout << "      New Sample: " << newSample->pInfo->Name << endl;
                pDimensionRegion->pSample = newSample;
            }
        }

        unityNote = pRegion->KeyRange.low;
        if (unityNote != pDimensionRegion->UnityNote) {
            cout << "      New Unity Note: " << unityNote << endl;
            pDimensionRegion->UnityNote = unityNote;
        }

        return;
    }

    gig::dimension_def_t DimensionDef = pRegion->pDimensionDefinitions[iDimension];
    for (zones[iDimension] = 0; zones[iDimension] < DimensionDef.zones; zones[iDimension]++) {
        switch (DimensionDef.dimension) {
            case gig::dimension_layer:
                iLayer = zones[iDimension];
                break;
            case gig::dimension_velocity:
                iVelocity = zones[iDimension];
                break;
            case gig::dimension_keyboard:
                iKeySwitch = zones[iDimension];
        }
        walkZones(pRegion, groups, mSamples, zones, iDimension - 1, iLayer, iVelocity, iKeySwitch);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "No file argument found!" << endl;
        return EXIT_FAILURE;
    }
    FILE* hFile = fopen(argv[1], "r");
    if (!hFile) {
        cout << "Invalid file argument!" << endl;
        return EXIT_FAILURE;
    }
    fclose(hFile);
    try {
        RIFF::File* riff = new RIFF::File(argv[1]);
        gig::File* gig  = new gig::File(riff);

        map<string, gig::Sample*> mSamples;

        string groups[4] = {"GMMuted", "GMLow", "GMMed", "GMHigh"};

        for (int i = 0; i < 4; i++) {
            loadSamples(&mSamples, gig, groups[i]);
        }

        gig::Instrument* pInstrument = gig->GetInstrument(1);

        cout << "Instrument name: " << pInstrument->pInfo->Name << endl;

        int iRegion = 1;
        gig::Region* pRegion = pInstrument->GetFirstRegion();
        while (pRegion) {

            cout << "  Region " << iRegion++;
            cout << " - Lower Key: " << pRegion->KeyRange.low << endl;

            uint8_t zones[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            walkZones(pRegion, groups, &mSamples, zones, pRegion->Dimensions - 1, 0, 0, 0);

            pRegion = pInstrument->GetNextRegion();
        }

        gig->Save();

        cout << "File saved: " << gig->GetFileName() << endl;

        delete gig;
        delete riff;
    }
    catch (RIFF::Exception e) {
        e.PrintMessage();
        return EXIT_FAILURE;
    }
    catch (...) {
        cout << "Unknown exception while trying to parse file." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
