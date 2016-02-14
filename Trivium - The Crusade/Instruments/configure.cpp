#include <cstdlib>

#include <libgig/gig.h>

#include "Guitar1.h"

using namespace std;

const char* inputGuitar =
    "../../../Instruments/Guitar"
    "/FS_Collection/FS_Gibson_Les_Paul_Electric_Both_Pick-ups_Guitar_and_Amp"
    "/FS_Gibson_Les_Paul_Electric_Both_Pick-ups_Guitar_and_Amp.gig";

const char* outputGuitar = "Guitar.gig";

RIFF::File* outRiff;
gig::File* outGig;

bool copyRiff(RIFF::File* srcRiff, const char* dstPath) {
    cout << "Copying file..." << endl;
    
    try {
        srcRiff->Save(dstPath);
    } catch (RIFF::Exception e) {
        cerr << "Failed copying file:" << endl;
        e.PrintMessage();
        return false;
    } catch (...) {
        cerr << "Unknown exception while trying to copy file." << endl;
        return false;
    }
    
    return true;
}

bool copyRiff(const char* srcPath, const char* dstPath) {
    RIFF::File* srcRiff;
    
    cout << "Opening input file..." << endl;
    
    try {
        srcRiff = new RIFF::File(srcPath);
    } catch (RIFF::Exception e) {
        cerr << "Failed opening input file:" << endl;
        e.PrintMessage();
        return false;
    } catch (...) {
        cerr << "Failed opening input file!" << endl;
        cerr << "Unknown exception while trying to access input file." << endl;
        return false;
    }
    
    bool copyResult = copyRiff(srcRiff, dstPath);
    
    delete srcRiff;
    
    return copyResult;
}

bool openGig(const char* path) {
    cout << "Opening output file..." << endl;
    
    try {
        outRiff = new RIFF::File(path);
        outGig = new gig::File(outRiff);
    } catch (RIFF::Exception e) {
        cerr << "Failed opening output file!" << endl;
        e.PrintMessage();
        return false;
    } catch (...) {
        cerr << "Failed opening output file!" << endl;
        cerr << "Unknown exception while trying to access output file." << endl;
        return false;
    }
    
    return true;
}

bool createGuitar1() {
    try {
        Guitar1 guitar1(outGig);
        guitar1.configure();
    } catch (gig::Exception e) {
        cerr << "Failed creating Guitar1!" << endl;
        e.PrintMessage();
        return false;
    } catch (...) {
        cerr << "Failed creating Guitar1!" << endl;
        cerr << "Unknown exception while trying to create Guitar1." << endl;
        return false;
    }
    
    return true;
}

void save() {
    cout << "Saving file..." << endl;
    outGig->Save();
}

void cleanup() {
    delete outGig;
    delete outRiff;
}

int main(int argc, char *argv[]) {
    if (!copyRiff(inputGuitar, outputGuitar)) {
        return EXIT_FAILURE;
    }
    
    if (!openGig(outputGuitar)) {
        return EXIT_FAILURE;
    }
    
    if (!createGuitar1()) {
        cleanup();
        return EXIT_FAILURE;
    }
    
    save();
    cleanup();
    
    cout << "Done!" << endl;
    
    return EXIT_SUCCESS;
}