#include <PluginEMGTwinCat.h>

PluginEMGTwinCat::PluginEMGTwinCat() : record_(false), _outDirectory("output"), threadStop_(false)
{

}

PluginEMGTwinCat::~PluginEMGTwinCat()
{

}

void PluginEMGTwinCat::init(std::string xmlName, std::string executionName)
{
	timeStamp_ = rtb::getTime();
	//pointer to subject XML
	std::auto_ptr<NMSmodelType> subjectPointer;

	try
	{
		subjectPointer = std::auto_ptr<NMSmodelType>(subject(xmlName, xml_schema::flags::dont_initialize));
	}
	catch (const xml_schema::exception& e)
	{
		cout << e << endl;
		exit(EXIT_FAILURE);
	}

	NMSmodelType::Channels_type& channels(subjectPointer->Channels());
	ChannelsType::Channel_sequence& channelSequence(channels.Channel());

	//std::cout << "Reading Execution XML from: " << executionName << std::endl;

	// Configuration XML
	std::auto_ptr<ExecutionType> executionPointer;

	try
	{
		std::auto_ptr<ExecutionType> temp(execution(executionName, xml_schema::flags::dont_initialize));
		executionPointer = temp;
	}
	catch (const xml_schema::exception& e)
	{
		cout << e << endl;
		exit(EXIT_FAILURE);
	}

	// Get the EMG XML configuration
	const std::string& EMGFile = executionPointer->ConsumerPlugin().EMGDeviceFile().get();

	// EMG XML class reader
	_executionEmgXml = new ExecutionEmgXml(EMGFile);

	//Ip and Port from the EMG xml
	std::string port = _executionEmgXml->getPort();


	//Get the EMG channel from the subject xml
	for (ChannelsType::Channel_iterator it = channelSequence.begin(); it != channelSequence.end(); it++)
	{
		//get name channel
		nameSet_.insert(it->name());
		nameVect_.push_back(it->name());

	}

	maxAmp_ = _executionEmgXml->getMaxEmg();

	if (nameVect_.size() > 16)
	{
		std::cout << "Error: exceeding amount of electrodes" << std::endl;
		exit(-1);
	}

	if (maxAmp_.size() != nameVect_.size())
	{
		std::cout << "Number of electrodes does not match executionEMG.xml" << std::endl;
		exit(-1);
	}

	client_ = new tcAdsClient(std::atoi(port.c_str()));//350
	std::string var = "EMGOBTwincat.Output.EMGFilt";
	varNameVect_.resize(1);
	varNameVect_[0] = client_->getVariableHandle(&var[0], var.size());

	if (varNameVect_[0] == -1)
		exit(-1);
	if (record_)
	{
		logger_ = new OpenSimFileLogger<double>(_outDirectory);
		logger_->addLog(Logger::EmgsFilter, nameVect_);
	}

	emgThread_ = new std::thread(&PluginEMGTwinCat::emgThread, this);
}

void PluginEMGTwinCat::emgThread()
{
	std::map<std::string, double> emgDataLocal;
	double timeLocal;

	while (!threadStop_)
	{
		timeLocal = rtb::getTime();
		int length = nameVect_.size();
		std::vector<double> data, dataSave;
		data.resize(length);
		client_->read(varNameVect_[0], &data[0], length * sizeof(double));

		for (int i = 0; i < nameVect_.size(); i++) {
			if (maxAmp_[i] < data[i])
				emgDataLocal[nameVect_[i]] = 1;
			else
				emgDataLocal[nameVect_[i]] = data[i] / maxAmp_[i];
			dataSave.push_back(emgDataLocal[nameVect_[i]]);
		}

		if (record_)
		{
			//std::cout << dataSave.at(0) << std::endl;
			logger_->log(Logger::EmgsFilter, timeStamp_, dataSave);
		}

		mtxEMG_.lock();
		dataEMG_ = emgDataLocal;
		mtxEMG_.unlock();
		mtxTime_.lock();
		timeStamp_ = timeLocal;
		mtxTime_.unlock();
	}
}

const std::map<std::string, double>& PluginEMGTwinCat::GetDataMap()
{

	mtxEMG_.lock();
	dataEMGSafe_ = dataEMG_;
	mtxEMG_.unlock();

	return dataEMGSafe_;
}


void PluginEMGTwinCat::stop()
{
	threadStop_ = true;
	emgThread_->join();
	delete emgThread_;
	if (record_)
	{
		logger_->stop();
		delete logger_;
	}
	for (std::vector<unsigned long>::const_iterator it = varNameVect_.begin(); it != varNameVect_.end(); it++)
		client_->releaseVariableHandle(*it);
	client_->disconnect();
	delete client_;
	_executionEmgXml->setMaxEmg(maxAmp_);
	_executionEmgXml->UpdateEmgXmlFile();
}

#ifdef UNIX
extern "C" ProducersPluginVirtual * create() {
	return new PluginEMGTwinCat;
}

extern "C" void destroy(ProducersPluginVirtual * p) {
	delete p;
}
#endif

#ifdef WIN32 // __declspec (dllexport) id important for dynamic loading
extern "C" __declspec (dllexport) ProducersPluginVirtual * __cdecl create() {
	return new PluginEMGTwinCat;
}

extern "C" __declspec (dllexport) void __cdecl destroy(ProducersPluginVirtual * p) {
	delete p;
}
#endif