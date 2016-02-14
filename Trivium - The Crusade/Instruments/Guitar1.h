#ifndef _GUITAR1_H
#define _GUITAR1_H

#include <libgig/gig.h>

using namespace std;

class Guitar1 {
    
    public:
        Guitar1(gig::File* gig);
        
        void configure();
        
    private:
        gig::Instrument* pOrigInstrument;
        gig::Instrument* pInstrument;
        
        gig::dimension_def_t layerDef;
        gig::dimension_def_t keyswitchDef;
        gig::dimension_def_t modwheelDef;
        gig::dimension_def_t velocityDef;
        
        void setDimensionKeyRange();
        void setDimensionDefinitions();
        
        void setLayerDefinition();
        void setKeyswitchDefinition();
        void setModwheelDefinition();
        void setVelocityDefinition();
        
        int getLayer(int index);
        int getKeyswitch(int index);
        int getOrigDimensionRegionIndex(int index);
        int getChordUnityNote(int layer, int unityNote);
        
        void deleteDimensions(gig::Region* pRegion);
        void addDimensions(gig::Region* pRegion);
        void copyDimensionRegion(gig::DimensionRegion* pDimRegion,
                                 gig::DimensionRegion* pOrigDimRegion);
        
        void configureRegions();
        void configureRegion(gig::Region* pRegion);
        
        void configureDimensionRegions(gig::Region* pRegion);
        void configureDimensionRegion(gig::Region* pRegion, int index);
};

#endif // _GUITAR1_H