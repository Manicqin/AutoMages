/*
 * ComponentsExports.h
 *
 *  Created on: Dec 2, 2013
 *      Author: manicqin
 */

#ifndef COMPONENTSEXPORTS_H_
#define COMPONENTSEXPORTS_H_

#include "../EntityComponents/AllComponents.h"

#include <boost/python.hpp>
#include <python2.7/Python.h>
#include <python2.7/import.h>

template <class T>
boost::shared_ptr<T>	getComponent(std::size_t id)
{
	auto registry = EntityComponent::ComponentRegistry<T>::getSingletonPtr();
	if(registry)
	{
		return	registry->getComponent(id);
	}
	return nullptr;
}



struct CameraComponent_pickle_suite : boost::python::pickle_suite
{
	static
	boost::python::tuple
	getinitargs(const EntityComponent::CameraComponent & comp)
	{
		return boost::python::make_tuple(comp.getCamera()->getName(),Standard::Vector3(comp.getCamera()->getPosition()));
	}
};

auto tt = &EntityComponent::ComponentRegistry<EntityComponent::CameraComponent>::getComponent;
BOOST_PYTHON_MODULE(ComponentsExports)
{
	boost::python::class_<EntityComponent::CameraComponent,boost::shared_ptr<EntityComponent::CameraComponent>>("CameraComponent",boost::python::no_init)
			.def_pickle(CameraComponent_pickle_suite());

	//boost::python::def("getCameraComponent",&getComponent<EntityComponent::CameraComponent>);
}


#endif /* COMPONENTSEXPORTS_H_ */
