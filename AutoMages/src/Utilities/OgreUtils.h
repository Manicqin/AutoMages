/*
 * OgreUtils.h
 *
 *  Created on: Nov 29, 2012
 *      Author: manicqin
 */

#ifndef OGREUTILS_H_
#define OGREUTILS_H_

#include <string>
#include <fstream>
#include <ctype.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <Ogre.h>

Ogre::ManualObject* createManualObjectFromFile(std::string fileName)
{
	Ogre::ManualObject*	retVal = nullptr;
	#define	fastCast(x) boost::lexical_cast<Ogre::Real>(x)
	if(boost::filesystem::exists(fileName))
	{
		std::ifstream file(fileName);

		if(file.is_open())
		{
			std::string name;
			std::string mtl;
			std::string smooth;
			std::vector<Ogre::Vector3>	vectors;
			std::vector<Ogre::Vector3>	faces;
			do{
				char	line[256] = "";
				file.getline(line,256);
				std::vector<std::string>	res;
				boost::split(res,line,isblank);


				if(!res.empty())
				{
					if(res[0] == "#")
					{
						//This is a comment
					}
					else if(res[0] == "o")
					{
						name = res[1];
					}
					else if(res[0] == "v")
					{
						vectors.push_back(Ogre::Vector3(fastCast(res[1]),fastCast(res[2]),fastCast(res[3])));
					}
					else if(res[0] == "usemtl")
					{
						mtl = res[1];
					}
					else if(res[0] == "s")
					{
						smooth = res[1];
					}
					else if(res[0] == "f")
					{
						faces.push_back(Ogre::Vector3(fastCast(res[1]),fastCast(res[2]),fastCast(res[3])));
					}
				}

			}while(!file.eof());
			retVal = new Ogre::ManualObject(name);
			retVal->begin(mtl);
			std::for_each(std::begin(vectors),std::end(vectors),[&](Ogre::Vector3 const& val)
				{
				retVal->position(val);
				});
//			std::for_each(std::begin(faces),std::end(faces),[&](Ogre::Vector3 const& val)
//				{
//					manu.position(val);
//				});
			retVal->end();
		}
	}
	else
	{
		throw(boost::filesystem::filesystem_error(std::string("createManualObjectFromFile File doesn't exists"),boost::system::error_code()));
	}
	#undef	fastCast
	return retVal;
}


#endif /* OGREUTILS_H_ */
