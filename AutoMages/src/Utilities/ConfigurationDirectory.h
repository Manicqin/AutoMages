#ifndef CONFIGURATIONLIST_H
#define CONFIGURATIONLIST_H

#include <map>
#include <string>
#include <OgreSingleton.h>

class Configuration
{
public:
    Configuration();
    const std::string get(std::string const & key) const;
    void put(std::string const & key , std::string const & vlaue);
    bool exists(std::string const & key)   const;
 private:
    std::map<std::string,std::string> mConfig;
};

class ConfigurationDirectory : public Ogre::Singleton<ConfigurationDirectory>
{
public:
    ConfigurationDirectory();
    const Configuration get(std::string const & key) const;
    void put(std::string const & key , Configuration const & vlaue);
    bool exists(std::string const & key)   const;
 private:
    std::map<std::string,Configuration> mConfigMap;
};

#endif // CONFIGURATION_H
