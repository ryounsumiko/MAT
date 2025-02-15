#Find the source and header files.  This isn't great practice because you have to
#rerun cmake to detect new files to link.
file(GLOB SOURCE_FILES *.cxx)
file(GLOB HEADER_FILES *.h)

#Include build system for the rest of this project.  Add new top-level directories here.
#First, build a ROOT dictionary
#I modified this dictionary generation code from Heidi's CMakeLists.txt
REFLEX_GENERATE_DICTIONARY(G__${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../dict/PlotUtilsDict.h SELECTION ${CMAKE_CURRENT_SOURCE_DIR}/../dict/PlotUtilsDict.xml OPTIONS -D__GCC_XML__)

#MESSAGE("boost:$ENV{BOOSTDIR}")
#include_directories(PlotUtils $ENV{PLOTUTILSROOT} $ENV{BOOSTDIR}/include $ENV{BOOSTDIR})

#install the dictionary files ROOT generates where LD_LIBRARY_PATH can find them
#(after sourcing setup.sh of course)
#Taken from Clark McGrew's edepsim package
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/G__${PROJECT_NAME}Dict.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/G__${PROJECT_NAME}_rdict.pcm
    DESTINATION lib)

  #In the transition to ROOT 6, dictionary files generated by reflex
  #through ROOT adopted a different naming convention.  The commit
  #that changed this was https://github.com/root-project/root/commit/d62b4fed98805d45079f9863
  set(DICTIONARY_NAME G__${PROJECT_NAME}.cxx)
else(${ROOT_VERSION} VERSION_GREATER 6)
  set(DICTIONARY_NAME G__${PROJECT_NAME}_dict.cpp)
endif(${ROOT_VERSION} VERSION_GREATER 6)

#TODO: Does this give me something to link against to pick up type reflection information?
#The secret ingredient to producing PlotUtils_dict.o seems to be creating an OBJECT library.
add_library(${PROJECT_NAME}_dict OBJECT ${CMAKE_CURRENT_BINARY_DIR}/${DICTIONARY_NAME})
#install(TARGETS ${PROJECT_NAME}_dict EXPORT ${PROJECT_NAME}Targets OBJECTS DESTINATION lib) 

#Now, build libplotutils.  Note that this builds the ROOT dictionary for PlotUtils into libplotutils itself.
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} $<TARGET_OBJECTS:${PROJECT_NAME}_dict>)
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES})
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets DESTINATION lib)
install(FILES ${HEADER_FILES} ${SOURCE_FILES} DESTINATION include/PlotUtils) #Including SOURCE_FILES fo
install(EXPORT ${PROJECT_NAME}Targets DESTINATION lib/cmake/${PROJECT_NAME})

export(TARGETS ${PROJECT_NAME} FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
