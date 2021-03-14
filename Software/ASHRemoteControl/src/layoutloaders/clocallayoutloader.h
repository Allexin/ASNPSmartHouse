#ifndef CLOCALLAYOUTLOADER_H
#define CLOCALLAYOUTLOADER_H

#include "../clayoutloader.h"

class cLocalLayoutLoader : public cLayoutLoader
{
public:
    virtual cBaseWidget* loadLayouts() override;
};

#endif // CLOCALLAYOUTLOADER_H
