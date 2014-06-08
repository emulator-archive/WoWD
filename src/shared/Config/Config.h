// (c) WoWD Group

#if !defined (CONFIG_H)
#define CONFIG_H

class ConfigFile
{
public:
    ConfigFile();
    ~ConfigFile();

    bool SetSource(const char *file, bool ignorecase = true);

    bool GetString(const char* name, std::string *value);
    std::string GetStringDefault(const char* name, const char* def);
    std::string GetStringVA(const char* def, const char * name, ...);

    bool GetBool(const char* name, bool *value);
    bool GetBoolDefault(const char* name, const bool def = false);

    bool GetInt(const char* name, int *value);
    int GetIntDefault(const char* name, const int def);
    int GetIntVA(int def, const char* name, ...);

    bool GetFloat(const char* name, float *value);
    float GetFloatDefault(const char* name, const float def);
    float GetFloatVA(float def, const char* name, ...);


private:
    DOTCONFDocument *mConf;
};


class ConfigMgr
{
    // Mainly used for WS, others will probably only have one.
public:
    ConfigFile MainConfig;
    ConfigFile RealmConfig;
};

extern ConfigMgr Config;

#endif
