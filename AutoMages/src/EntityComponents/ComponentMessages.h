/*
 * ComponentMessages.h
 *
 *  Created on: Sep 28, 2012
 *      Author: manicqin
 */

#ifndef COMPONENTMESSAGES_H_
#define COMPONENTMESSAGES_H_
#include "Messages.h"
#include <tuple>
#include <memory>
#include "../Utilities/rapidxml/rapidxml.hpp"
#include "../Utilities/rapidxml/rapidxml_utils.hpp"
#include "../Utilities/rapidxml/rapidxml_print.hpp"

struct	configData
{
	configData(std::string const& strXml)
	{
		mStrXml.reset( new std::vector<char>(std::begin(strXml),std::end(strXml)));
		mStrXml->emplace_back('\0');

		mDocXml.reset(new rapidxml::xml_document<char>);
		mDocXml->parse<0>(mStrXml->data());
	}

	rapidxml::xml_node<char>* getDoc()
	{
		return mDocXml.get();
	}
private:
	std::unique_ptr<std::vector<char>>	mStrXml;
	std::unique_ptr<rapidxml::xml_document<char>>	mDocXml;
};

template<class T>
T	createFromData(std::string const &	rawData)
{
	auto xml = std::make_shared<configData>(rawData);
	return T(std::shared_ptr<rapidxml::xml_node<>>(xml,xml->getDoc()));
}

template<class T>
T	createFromFile(std::string const &	fileName, std::string const & path = "")
{
    std::string absPath = path + fileName;
    rapidxml::file<>	rxFile(absPath.c_str());

	auto xml = std::make_shared<configData>(rxFile.data());
	return T(std::shared_ptr<rapidxml::xml_node<>>(xml,xml->getDoc()));
}


DataMessageDeclare2(CreateEntityMsg,std::shared_ptr<rapidxml::xml_node<>>,std::size_t,EntityId);
DataMessageDeclare(CreateSceneMsg,std::shared_ptr<rapidxml::xml_node<>>);

#endif /* COMPONENTMESSAGES_H_ */
