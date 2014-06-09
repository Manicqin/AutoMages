/*
 * propertyTreeUtils.h
 *
 *  Created on: Nov 19, 2012
 *      Author: manicqin
 */

#ifndef PROPERTYTREEUTILS_H_
#define PROPERTYTREEUTILS_H_
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <iostream>
#include <map>

boost::property_tree::ptree	loadConfigFile(std::string const& cnfgFile);
void print(boost::property_tree::ptree const& pt);
void merge( boost::property_tree::ptree& pt, const boost::property_tree::ptree& updates );
boost::property_tree::ptree retNodes(std::string const& tagName, const boost::property_tree::ptree& tree);

#endif /* PROPERTYTREEUTILS_H_ */
