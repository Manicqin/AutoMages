#include "ConfigurationDirectory.h"


Configuration::Configuration()
{
}

const std::string Configuration::get(const std::string &key) const
{
    return mConfig.at(key);
}

void Configuration::put(const std::string &key, const std::string &vlaue)
{
    mConfig[key] = vlaue;
}



bool Configuration::exists(const std::string &key) const
{
    return mConfig.find(key) != std::end(mConfig);
}

template<> ConfigurationDirectory* Ogre::Singleton<ConfigurationDirectory>::msSingleton = nullptr;

ConfigurationDirectory::ConfigurationDirectory()
{
}

const Configuration ConfigurationDirectory::get(const std::string &key) const
{
    return mConfigMap.at(key);
}

void ConfigurationDirectory::put(const std::string &key, const Configuration &vlaue)
{
    mConfigMap[key] = vlaue;
}

bool ConfigurationDirectory::exists(const std::string &key) const
{
    return mConfigMap.find(key) != std::end(mConfigMap);
}
