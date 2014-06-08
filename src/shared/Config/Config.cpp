// (c) Abyss Group
#include "ConfigEnv.h"
ConfigMgr Config;

ConfigFile::ConfigFile() : mConf(0)
{
}


ConfigFile::~ConfigFile()
{
    if (mConf)
        delete mConf;
}


bool ConfigFile::SetSource(const char *file, bool ignorecase)
{
    mConf = new DOTCONFDocument(ignorecase ?
        DOTCONFDocument::CASEINSENSETIVE :
        DOTCONFDocument::CASESENSETIVE);

    if (mConf->setContent(file) == -1)
    {
        delete mConf;
        mConf = 0;
        return false;
    }

    return true;
}


bool ConfigFile::GetString(const char* name, std::string *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = node->getValue();

    return true;
}


std::string ConfigFile::GetStringDefault(const char* name, const char* def)
{
    if(!mConf)
        return std::string(def);

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(name);
    if(!node || !node->getValue())
        return std::string(def);

    return std::string(node->getValue());
}


bool ConfigFile::GetBool(const char* name, bool *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    const char* str = node->getValue();
    if(strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
        strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
        strcmp(str, "1") == 0)
    {
        *value = true;
    }
    else
        *value = false;

    return true;
}


bool ConfigFile::GetBoolDefault(const char* name, const bool def)
{
    bool val;
    return GetBool(name, &val) ? val : def;
}


bool ConfigFile::GetInt(const char* name, int *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = atoi(node->getValue());

    return true;
}

bool ConfigFile::GetFloat(const char* name, float *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = atof(node->getValue());

    return true;
}




int ConfigFile::GetIntDefault(const char* name, const int def)
{
    int val;
    return GetInt(name, &val) ? val : def;
}

float ConfigFile::GetFloatDefault(const char* name, const float def)
{
    float val;
    return (GetFloat(name, &val) ? val : def);
}

int ConfigFile::GetIntVA(int def, const char* name, ...)
{
    va_list ap;
    va_start(ap, name);
    char str[150];
    vsprintf(str, name, ap);
    va_end(ap);
    int val;
    return GetInt(str, &val) ? val : def;
}

float ConfigFile::GetFloatVA(float def, const char* name, ...)
{
    va_list ap;
    va_start(ap, name);
    char str[150];
    vsprintf(str, name, ap);
    va_end(ap);
    float val;
    return GetFloat(str, &val) ? val : def;
}

std::string ConfigFile::GetStringVA(const char* def, const char * name, ...)
{
    va_list ap;
    va_start(ap, name);
    char str[150];
    vsprintf(str, name, ap);
    va_end(ap);

    if(!mConf)
        return std::string(def);

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)mConf->findNode(str);
    if(!node || !node->getValue())
        return std::string(def);

    return std::string(node->getValue());
}
