cmake_minimum_required(VERSION 2.6)
 
project(AutoMages)

#if(WIN32)
#	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
#	set(OGRE_SAMPLES_INCLUDEPATH
#		$ENV{OGRE_HOME}/Samples/include
#	)
#endif(WIN32)


 
set(HDRS
    src/ClassExports/ComponentsExports.h
    src/ClassExports/MessagesExports.h
    src/ClassExports/OISExports.h
    src/ClassExports/SystemsExports.h
    
    src/EntityComponents/AllComponents.h
    src/EntityComponents/AllMessages.h
    src/EntityComponents/Component.h
    src/EntityComponents/ComponentMessages.h
    src/EntityComponents/ComponentUtils.h
    src/EntityComponents/ControllerMessages.h
    src/EntityComponents/Entity.h
    src/EntityComponents/EntityMessages.h
    src/EntityComponents/EntityRegister.h
    src/EntityComponents/GraphicsMessages.h
    src/EntityComponents/GuiComponent.h
    src/EntityComponents/LogMessages.h
    src/EntityComponents/Messages.h
    src/EntityComponents/OgreMessages.h
    src/EntityComponents/OgreMiscComponent.h
    src/EntityComponents/PhysicsMessages.h
    src/EntityComponents/ScriptComponent.h
    src/EntityComponents/ScriptMessages.h
    src/EntityComponents/SystemsMessages.h

    src/GUI/FormattedListboxTextItem.h
    src/GUI/GameConsoleWindow.h

    src/Systems/CEGuiSystem.h    
    src/Systems/Channel.h    
    src/Systems/ComponentsSystem.h    
    src/Systems/ControllerSystem.h    
    src/Systems/EntitySystem.h    
    src/Systems/GuiSystem.h    
    src/Systems/LocomotionSystem.h    
    src/Systems/LoggingSystem.h    
    src/Systems/OgreSystem.h    
    src/Systems/PeriodicSystem.h    
    src/Systems/PhysicsSystem.h    
    src/Systems/PyGuiSystem.h    
    src/Systems/PythonBindings.h    
    src/Systems/ScriptManager.h    
    src/Systems/SystemBase.h    
    src/Systems/SystemsManager.h    

    src/Utilities/ConfigurationDirectory.h
    src/Utilities/CEGuiUtils.h
    src/Utilities/MacroUtils.h
    src/Utilities/OgreUtils.h
    src/Utilities/propertyTreeUtils.h
    src/Utilities/rapidxmlUtils.h
    src/Utilities/SelfEnumClasses.h
    src/Utilities/Vector3.h

    src/Utilities/Physics/DebugDrawer.h
    src/Utilities/Physics/Euler.h
    src/Utilities/Physics/OgreBulletUtils.h
    src/Utilities/Physics/PhysicsUtils.h

    src/Utilities/rapidxml/rapidxml.hpp
    src/Utilities/rapidxml/rapidxml_iterators.hpp
    src/Utilities/rapidxml/rapidxml_print.hpp
    src/Utilities/rapidxml/rapidxml_utils.hpp


 )
 
 set(SRCS
    src/EntityComponents/Component.cpp
    src/EntityComponents/ComponentUtils.cpp
    src/EntityComponents/Entity.cpp
    src/EntityComponents/EntityRegister.cpp
    src/EntityComponents/GuiComponent.cpp
    src/EntityComponents/LogMessages.cpp
    src/EntityComponents/Messages.cpp
    src/EntityComponents/OgreMiscComponent.cpp
    src/EntityComponents/PhysicsMessages.cpp
    src/EntityComponents/ScriptComponent.cpp
    src/EntityComponents/SystemsMessages.cpp

    src/GUI/FormattedListboxTextItem.cpp
    src/GUI/GameConsoleWindow.cpp

    src/Systems/CEGuiSystem.cpp
    src/Systems/Channel.cpp    
    src/Systems/ComponentsSystem.cpp
    src/Systems/ControllerSystem.cpp    
    src/Systems/EntitySystem.cpp    
    src/Systems/GuiSystem.cpp    
    src/Systems/LocomotionSystem.cpp    
    src/Systems/LoggingSystem.cpp    
    src/Systems/OgreSystem.cpp    
    src/Systems/PeriodicSystem.cpp    
    src/Systems/PhysicsSystem.cpp    
    src/Systems/PyGuiSystem.cpp    
    src/Systems/PythonBindings.cpp    
    src/Systems/ScriptManager.cpp    
    src/Systems/SystemBase.cpp    
    src/Systems/SystemsManager.cpp    

    src/Utilities/ConfigurationDirectory.cpp
    src/Utilities/propertyTreeUtils.cpp
    src/Utilities/rapidxmlUtils.cpp
    src/Utilities/SelfEnumClasses.cpp
    src/Utilities/Vector3.cpp

    src/Utilities/Physics/DebugDrawer.cpp
    src/Utilities/Physics/Euler.cpp
 )
 
 include_directories( ${BULLET_INCLUDE_DIRS}
 	${OGRE_INCLUDE_DIRS}
	${CEGUI_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
 )


ADD_LIBRARY( AutoMages ${SRCS} ${HDRS}) 
set_target_properties(AutoMages PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(AutoMages ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES})

 
# # post-build copy for win32
# if(WIN32 AND NOT MINGW)
# 	add_custom_command( TARGET OgreApp PRE_BUILD
# 		COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
# 	add_custom_command( TARGET OgreApp POST_BUILD
# 		COMMAND copy \"$(TargetPath)\" .\\dist\\bin )
# endif(WIN32 AND NOT MINGW)

# if(MINGW OR UNIX)
# 	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
# endif(MINGW OR UNIX)
 
# #if(WIN32)
# # 
# # 	install(TARGETS OgreApp
# # 		RUNTIME DESTINATION bin
# # 		CONFIGURATIONS All)
 
# # 	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
# # 		DESTINATION ./
# # 		CONFIGURATIONS Release RelWithDebInfo Debug
# # 	)
 
# # 	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
# # 		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
# # 		DESTINATION bin
# # 		CONFIGURATIONS Release RelWithDebInfo
# # 	)
 
# # 	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg
# # 		${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
# # 		DESTINATION bin
# # 		CONFIGURATIONS Debug
# # 	)
 
# #         # NOTE: for the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
# #         # so you'll have to change that to make it work with 1.7.1
# # 	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
# # 		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
# # 		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
# # 		${OGRE_PLUGIN_DIR_REL}/libOIS.dll
# # 		DESTINATION bin
# # 		CONFIGURATIONS Release RelWithDebInfo
# # 	)
 
# # 	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
# # 		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
# # 		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
# # 		${OGRE_PLUGIN_DIR_DBG}/libOIS_d.dll
# # 		DESTINATION bin
# # 		CONFIGURATIONS Debug
# # 	)
 
# #    # as of sdk 1.7.2 we need to copy the boost dll's as well
# #    # because they're not linked statically (it worked with 1.7.1 though)
# #    install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE}
# #       ${Boost_THREAD_LIBRARY_RELEASE}
# #       DESTINATION bin
# #       CONFIGURATIONS Release RelWithDebInfo
# #    )
 
# #    install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG}
# #       ${Boost_THREAD_LIBRARY_DEBUG}
# #       DESTINATION bin
# #       CONFIGURATIONS Debug
# #    )
# # endif(WIN32)

 # if(UNIX)
 
 # 	install(TARGETS AutoMages
 # 		RUNTIME DESTINATION bin
 # 		CONFIGURATIONS All)
 
 # 	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
 # 		DESTINATION ./
 # 		CONFIGURATIONS Release RelWithDebInfo Debug
 # 	)
 
 # 	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
 # 		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
 # 		DESTINATION bin
 # 		CONFIGURATIONS Release RelWithDebInfo Debug
 # 	)
 
 # endif(UNIX)
 
