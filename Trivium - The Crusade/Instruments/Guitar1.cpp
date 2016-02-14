#include <cmath>

#include "Guitar1.h"

Guitar1::Guitar1(gig::File* gig) {
    pOrigInstrument = gig->GetFirstInstrument();
    
    cout << "Duplicating instrument: " << pOrigInstrument->pInfo->Name << endl;
    
    pInstrument = gig->AddDuplicateInstrument(pOrigInstrument);
    pInstrument->pInfo->Name = "FS GM Guitar 1";
    pInstrument->MoveTo(pOrigInstrument);
}

void Guitar1::configure() {
    cout << "Configuring instrument: " << pInstrument->pInfo->Name << endl;
    
    setDimensionKeyRange();
    setDimensionDefinitions();
    configureRegions();
}

void Guitar1::setDimensionKeyRange() {
    cout << "Setting dimension key range..." << endl;
    
    pInstrument->DimensionKeyRange.low = 36;
    pInstrument->DimensionKeyRange.high = 39;
}

void Guitar1::setDimensionDefinitions() {
    cout << "Setting dimension definitions..." << endl;
    
    setLayerDefinition();
    setKeyswitchDefinition();
    setModwheelDefinition();
    setVelocityDefinition();
}

void Guitar1::setLayerDefinition() {
    layerDef.dimension = gig::dimension_layer;
    layerDef.bits = 2;
    layerDef.zones = 3;
}

void Guitar1::setKeyswitchDefinition() {
    keyswitchDef.dimension = gig::dimension_keyboard;
    keyswitchDef.bits = 1;
    keyswitchDef.zones = 2;
}

void Guitar1::setModwheelDefinition() {
    modwheelDef.dimension = gig::dimension_modwheel;
    modwheelDef.bits = 3;
    modwheelDef.zones = 5;
}

void Guitar1::setVelocityDefinition() {
    velocityDef.dimension = gig::dimension_velocity;
    velocityDef.bits = 2;
    velocityDef.zones = 4;
}

int Guitar1::getLayer(int index) {
    return index & ((int) pow(2, layerDef.bits) - 1);
}

int Guitar1::getKeyswitch(int index) {
    return (index >> layerDef.bits) & ((int) pow(2, keyswitchDef.bits) - 1);
}

int Guitar1::getOrigDimensionRegionIndex(int index) {
    int origIndex = index >> (layerDef.bits + keyswitchDef.bits);
    return origIndex & ((int) pow(2, modwheelDef.bits + velocityDef.bits) - 1);
}

int Guitar1::getChordUnityNote(int layer, int unityNote) {
    switch (layer) {
        case 1:
            return unityNote + 7;
        case 2:
            return unityNote + 12;
        default:
            return unityNote;
    }
}
 
void Guitar1::deleteDimensions(gig::Region* pRegion) {
    pRegion->DeleteDimension(pRegion->GetDimensionDefinition(gig::dimension_modwheel));
    pRegion->DeleteDimension(pRegion->GetDimensionDefinition(gig::dimension_velocity));
}
 
void Guitar1::addDimensions(gig::Region* pRegion) {
    pRegion->AddDimension(&layerDef);
    pRegion->AddDimension(&keyswitchDef);
    pRegion->AddDimension(&modwheelDef);
    pRegion->AddDimension(&velocityDef);
}

void Guitar1::copyDimensionRegion(gig::DimensionRegion* pDimRegion,
                                  gig::DimensionRegion* pOrigDimRegion) {
    
    // CopyAssign() override UnityNote so we save it to restore it after copy
    int unityNote = pDimRegion->UnityNote;
    
    pDimRegion->CopyAssign(pOrigDimRegion);
    
    pDimRegion->UnityNote = unityNote;
    
    pDimRegion->DimensionUpperLimits[2] = pOrigDimRegion->DimensionUpperLimits[0];
    pDimRegion->DimensionUpperLimits[3] = pOrigDimRegion->DimensionUpperLimits[1];
}

void Guitar1::configureRegions() {
    cout << "Configuring regions..." << endl;
    
    gig::Region* pRegion = pInstrument->GetFirstRegion();
    for (int i = 0; i < 33; i++) {
        configureRegion(pRegion);
        pRegion = pInstrument->GetNextRegion();
    }
}

void Guitar1::configureRegion(gig::Region* pRegion) {
    deleteDimensions(pRegion);
    addDimensions(pRegion);
    configureDimensionRegions(pRegion);
}

void Guitar1::configureDimensionRegions(gig::Region* pRegion) {
    for (uint i = 0; i < pRegion->DimensionRegions; i++) {
        configureDimensionRegion(pRegion, i);
    }
}

void Guitar1::configureDimensionRegion(gig::Region* pRegion, int index) {    
    int layer = getLayer(index);
    int keyswitch = getKeyswitch(index);
    
    gig::DimensionRegion* pDimRegion = pRegion->pDimensionRegions[index];
    
    if (keyswitch == 0 && layer != 0) {
        pDimRegion->pSample = NULL;
    } else {
        int chordUnityNote = getChordUnityNote(layer, pDimRegion->UnityNote);
        gig::Region* pOrigRegion = pOrigInstrument->GetRegion(chordUnityNote);
        
        int origIndex = getOrigDimensionRegionIndex(index);
        
        copyDimensionRegion(pDimRegion, pOrigRegion->pDimensionRegions[origIndex]);
    }
}