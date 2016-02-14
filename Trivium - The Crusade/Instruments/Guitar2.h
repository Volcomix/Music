#ifndef _GUITAR2_H
#define _GUITAR2_H

#include <libgig/gig.h>

using namespace std;

class Guitar2 {
    
    public:
        Guitar2(gig::File* gig);
        
        void configure();
    
    private:
        gig::Instrument* pOrigInstrument;
        gig::Instrument* pInstrument;
        
        gig::dimension_def_t keyswitchDef;
        gig::dimension_def_t modwheelDef;
        gig::dimension_def_t velocityDef;
        
        void setDimensionKeyRange();
        void setDimensionDefinitions();
        
        void setKeyswitchDefinition();
        void setModwheelDefinition();
        void setVelocityDefinition();
        
        int getKeyswitch(int index);
        int getOrigDimensionRegionIndex(int index);
        
        void deleteDimensions(gig::Region* pRegion);
        void addDimensions(gig::Region* pRegion);
        void copyDimensionRegion(gig::DimensionRegion* pDimRegion,
                                 gig::DimensionRegion* pOrigDimRegion);
        
        void configureRegions();
        void configureRegion(gig::Region* pRegion);
        
        void configureDimensionRegions(gig::Region* pRegion);
        void configureDimensionRegion(gig::Region* pRegion, int index);
};

#endif // _GUITAR2_H