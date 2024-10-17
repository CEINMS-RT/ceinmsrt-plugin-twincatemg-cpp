#include <PluginEMGTwinCat.h>
#include <cstdlib>
#include <csignal>
#include <windows.h>
namespace timeEMG{ // there already a getime function here
#include <GetTime.h>
};
#include <iomanip> 

bool endMain;

void SigintHandler(int sig)
{
	endMain = true;;
}

int main(void)
{
	endMain = false;
	signal(SIGINT, SigintHandler);

	PluginEMGTwinCat plugin;
	std::vector<std::string> muscleName;

	plugin.setDirectories("Test");
	plugin.setRecord(true);
	plugin.init("C:/Users/labuser/Documents/CEINMS/cfg/Guillaume/subjectCalibrated140218.xml", "C:/Users/labuser/Documents/CEINMS/cfg/Guillaume/executionRT_noEMG.xml");

	while (!endMain)
	{
		double timeInint = rtb::getTime();
		double time = plugin.getTime();
		std::cout << std::setprecision(16) << rtb::getTime() << " a" << std::endl << std::flush;
		std::map<std::string, double> temp = plugin.GetDataMap();
		std::cout << std::setprecision(16) << rtb::getTime() - timeInint << " b" << std::endl << std::flush;
		std::map<std::string, double> tempT = plugin.GetDataMapTorque();
		std::cout << std::setprecision(16) << rtb::getTime() - timeInint << " : " << tempT["tib_ant_r"] << " : " << tempT["tib_ant_l"] << std::endl << std::flush;
		//Sleep(100);
	}
	plugin.stop();

	return 0;
}

