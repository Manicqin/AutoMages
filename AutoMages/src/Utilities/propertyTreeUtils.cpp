/*
 * propertyTreeUtils.cpp
 *
 *  Created on: Nov 19, 2012
 *      Author: manicqin
 */

#include "propertyTreeUtils.h"
#include <boost/property_tree/xml_parser.hpp>

boost::property_tree::ptree	loadConfigFile(std::string const& cnfgFile)
{
	boost::property_tree::ptree	retVal;
	boost::property_tree::read_xml(cnfgFile,retVal);
	return retVal;

}
void print(boost::property_tree::ptree const& pt)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        print(it->second);
    }
}

void merge( boost::property_tree::ptree& pt, const boost::property_tree::ptree& updates )
{
   BOOST_FOREACH( auto& update, updates )
   {
      pt.add_child( update.first, update.second );
   }
}


boost::property_tree::ptree retNodes(std::string const& tagName, const boost::property_tree::ptree& tree) {
	using boost::property_tree::ptree;

	ptree retVal;

    ptree::const_iterator end = tree.end();
    for (ptree::const_iterator it = tree.begin(); it != end; ++it) {

    	if(it->first == tagName)
    	{
    		retVal.add_child(it->first,it->second);
    	}
    	else
    	{
    		auto ret = retNodes(tagName,it->second);
    		if(!ret.empty())
    		{
    			merge(retVal,ret);
    		}
    	}
    }

    return retVal;
};



