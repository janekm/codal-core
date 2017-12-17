#include "MSCSNORFS.h"

#if CONFIG_ENABLED(DEVICE_USB)

#include "CodalCompat.h"
#include "CodalDmesg.h"
#include "CodalDevice.h"

#define LOG DMESG

namespace codal::snorfs {

MSC::MSC(FS &fs) : fs(fs)
{
    currFile = NULL;
}

void MSC::addFiles()
{
    MSCUF2::addFiles();

    addFile(10, "spiflash.bin", fs.rawSize());
    addDirectory(20, "SPIFLASH");

    fs.dirRewind();
    auto d = fs.dirRead();
    while (d)
    {
        addFile(d->fileID ^ 0xff00, d->name, d->size, 20);
        d = fs.dirRead();
    }
}

void MSC::readFileBlock(uint16_t id, int blockAddr, char *dst)
{
    if (id & 0xff00)
    {
        id ^= 0xff00;

        if (currFile)
        {
            if (currFile->fileID() != id)
            {
                delete currFile;
                currFile = NULL;
            }
        }

        if (!currFile)
            currFile = fs.open(id);

        currFile->seek(blockAddr * 512);
        currFile->read(dst, 512);
        return;
    }

    switch (id)
    {
    case 10:
        fs.readFlashBytes(blockAddr * 512, dst, 512);
        break;
    default:
        MSCUF2::readFileBlock(id, blockAddr, dst);
        break;
    }
}

}
#endif
