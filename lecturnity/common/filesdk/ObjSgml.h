#pragma once
#include "SgmlParser.h"
#include "export.h"

class FILESDK_EXPORT CObjSgml :
    public SgmlFile
{
public:
    CObjSgml(void);
    ~CObjSgml(void);

public:
    HRESULT GetImageFileNames(CStringArray &aImageFilenames);
};
