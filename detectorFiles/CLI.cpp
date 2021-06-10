#include "CLI.h"

void CLI::start()
{
	dio->write("Welcome to the Anomaly Detection Server.\nPlease choose an option:\n");
	PrintMenuCommand(dio, *options.getVec()).execute();
	float x = 1;
	dio->read(&x);
	while (x != 6 && x <= options.getVec()->size()) {
		options.getVec()->at(int(x - 1))->execute();
		dio->write("Welcome to the Anomaly Detection Server.\nPlease choose an option:\n");
		PrintMenuCommand(dio, *options.getVec()).execute();
		dio->read(&x);
	}
}


CLI::~CLI() {
	for (auto it = options.getVec()->begin(); it < options.getVec()->end(); ++it) {
		delete* it;
	}
}
