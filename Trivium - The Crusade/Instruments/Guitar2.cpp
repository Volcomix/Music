#include <cmath>

#include "Guitar2.h"

Guitar2::Guitar2(gig::File* gig) {
    pOrigInstrument = gig->GetInstrument(1);
    
    cout << "Duplicating instrument: " << pOrigInstrument->pInfo->Name << endl;
    
    pInstrument = gig->AddDuplicateInstrument(pOrigInstrument);
    pInstrument->pInfo->Name = "FS GM Guitar 2";
    pInstrument->MoveTo(pOrigInstrument);
}

void Guitar2::configure() {
    cout << "Configuring instrument: " << pInstrument->pInfo->Name << endl;
    
    setDimensionKeyRange();
    setDimensionDefinitions();
    configureRegions();
}

void Guitar2::setDimensionKeyRange() {
    cout << "Setting dimension key range..." << endl;
    
    pInstrument->DimensionKeyRange.low = 36;
    pInstrument->DimensionKeyRange.high = 37;
}

void Guitar2::setDimensionDefinitions() {
    cout << "Setting dimension definitions..." << endl;
    
    setKeyswitchDefinition();
    setModwheelDefinition();
    setVelocityDefinition();
}

void Guitar2::setKeyswitchDefinition() {
    keyswitchDef.dimension = gig::dimension_keyboard;
    keyswitchDef.bits = 1;
    keyswitchDef.zones = 2;
}

void Guitar2::setModwheelDefinition() {
    modwheelDef.dimension = gig::dimension_modwheel;
    modwheelDef.bits = 3;
    modwheelDef.zones = 5;
}

void Guitar2::setVelocityDefinition() {
    velocityDef.dimension = gig::dimension_velocity;
    velocityDef.bits = 2;
    velocityDef.zones = 4;
}

int Guitar2::getKeyswitch(int index) {
    return index & ((int) pow(2, keyswitchDef.bits) - 1);
}

int Guitar2::getOrigDimensionRegionIndex(int index) {
    int origIndex = index >> keyswitchDef.bits;
    return origIndex & ((int) pow(2, modwheelDef.bits + velocityDef.bits) - 1);
}
 
void Guitar2::deleteDimensions(gig::Region* pRegion) {
    pRegion->DeleteDimension(pRegion->GetDimensionDefinition(gig::dimension_modwheel));
    pRegion->DeleteDimension(pRegion->GetDimensionDefinition(gig::dimension_velocity));
}
 
void Guitar2::addDimensions(gig::Region* pRegion) {
    pRegion->AddDimension(&keyswitchDef);
    pRegion->AddDimension(&modwheelDef);
    pRegion->AddDimension(&velocityDef);
}

void Guitar2::copyDimensionRegion(gig::DimensionRegion* pDimRegion,
                                  gig::DimensionRegion* pOrigDimRegion) {
    
    pDimRegion->CopyAssign(pOrigDimRegion);
    
    pDimRegion->DimensionUpperLimits[1] = pOrigDimRegion->DimensionUpperLimits[0];
    pDimRegion->DimensionUpperLimits[2] = pOrigDimRegion->DimensionUpperLimits[1];
}

void Guitar2::configureRegions() {
    cout << "Configuring regions..." << endl;
    
    gig::Region* pRegion = pInstrument->GetFirstRegion();
    for (int i = 0; i < 45; i++) {
        configureRegion(pRegion);
        pRegion = pInstrument->GetNextRegion();
    }
}

void Guitar2::configureRegion(gig::Region* pRegion) {
    deleteDimensions(pRegion);
    addDimensions(pRegion);
    configureDimensionRegions(pRegion);
}

void Guitar2::configureDimensionRegions(gig::Region* pRegion) {
    for (uint i = 0; i < pRegion->DimensionRegions; i++) {
        configureDimensionRegion(pRegion, i);
    }
}

void Guitar2::configureDimensionRegion(gig::Region* pRegion, int index) {    
    int keyswitch = getKeyswitch(index);
    
    gig::DimensionRegion* pDimRegion = pRegion->pDimensionRegions[index];
    
    if (keyswitch == 0) {
        pDimRegion->pSample = NULL;
    } else {
        int unityNote = pDimRegion->UnityNote;
        gig::Region* pOrigRegion = pOrigInstrument->GetRegion(unityNote);
        
        int origIndex = getOrigDimensionRegionIndex(index);
        
        copyDimensionRegion(pDimRegion, pOrigRegion->pDimensionRegions[origIndex]);
    }
}