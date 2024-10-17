ADD_LIBRARY( PluginEMGTwinCat SHARED PluginEMGTwinCat.cpp )


TARGET_LINK_LIBRARIES( PluginEMGTwinCat
	TcAdsDll
	FileIO
	XMLExecutionInterpreter
	XMLEMGInterpreter
	${Boost_LIBRARIES}
	XercesC::XercesC
	${WINLIB}
	${WINMM}
	AdsClient
	DynLib
)

ADD_LIBRARY( PluginEMGTwinCatNoMax SHARED PluginEMGTwinCatNoMax.cpp )


TARGET_LINK_LIBRARIES( PluginEMGTwinCat
	TcAdsDll
	FileIO
	XMLExecutionInterpreter
	XMLEMGInterpreter
	${Boost_LIBRARIES}
	XercesC::XercesC
	${WINLIB}
	${WINMM}
	AdsClient
	FileLogger
	DynLib
)

TARGET_LINK_LIBRARIES( PluginEMGTwinCatNoMax
	TcAdsDll
	FileIO
	XMLExecutionInterpreter
	XMLEMGInterpreter
	${Boost_LIBRARIES}
	XercesC::XercesC
	${WINLIB}
	${WINMM}
	AdsClient
	FileLogger
	DynLib
)

ADD_EXECUTABLE(TestPluginEMGTwincat TestEMGAds.cpp
)

TARGET_LINK_LIBRARIES(TestPluginEMGTwincat
	PluginEMGTwinCat
)