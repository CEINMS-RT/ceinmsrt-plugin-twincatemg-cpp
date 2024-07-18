#ifndef PLUGINEMGTWINCAT_H
#define PLUGINEMGTWINCAT_H

#include "execution.hxx"
#include "XMLInterpreter.h"
#include "ExecutionEmgXml.h"
#include <ProducersPluginVirtual.h>
#include "NMSmodel.hxx"
#include <tcAdsClient.h>
#include "OpenSimFileLogger.h"
#include <GetTime.h>
#include <thread>
#include <mutex>

#define M_PI 3.14159265359

#ifdef WIN32
	class __declspec(dllexport) PluginEMGTwinCat : public ProducersPluginVirtual {
#endif
#ifdef UNIX
	class  PluginEMGTwinCat : public ProducersPluginVirtual {
#endif
public:
	PluginEMGTwinCat();
	~PluginEMGTwinCat();

	void init(std::string xmlName, std::string executionName);

	void reset()
	{
	}

	/**
	* Get the time stamp of the EMG capture.
	*/
	const double& getTime()
	{

		mtxTime_.lock();
		timeStampSafe_= timeStamp_;
		mtxTime_.unlock();
		return  timeStampSafe_;
	}

	/**
	* Get a set of the channel name
	*/
	const std::set<std::string>& GetNameSet()
	{
		return nameSet_;
	}
	const std::map<std::string, double>& GetDataMap();

	const std::map<std::string, double>& GetDataMapTorque()
	{
		return _torque;
	}

	void stop();

	void setDirectories(std::string outDirectory, std::string inDirectory = std::string())
	{
		_outDirectory = outDirectory;
		_inDirectory = inDirectory;
	}

	void setVerbose(int verbose)
	{
	}

	void setRecord(bool record)
	{
		record_ = record;
	}

protected:

	std::string _outDirectory;
	std::string _inDirectory;

	void emgThread();

	double timeStamp_;
	double timeStampSafe_;
	tcAdsClient* client_;
	std::vector<unsigned long> varNameVect_;
	OpenSimFileLogger<double>* logger_;
	bool record_;
	std::set<std::string> nameSet_;
	std::map<std::string, double> dataEMG_;
	std::map<std::string, double> dataEMGSafe_;
	ExecutionEmgXml* _executionEmgXml;
	std::vector<std::string> nameVect_; //!< Vector of channel names
	std::map<std::string, double> _torque;
	std::vector<double> maxAmp_;
	std::thread *emgThread_;
	std::mutex mtxEMG_;
	std::mutex mtxTime_;
	bool threadStop_;
};

#endif