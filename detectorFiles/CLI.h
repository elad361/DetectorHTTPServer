
#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	float threshold;
	HybridAnomalyDetector detector;
	MacroCommand options;
	std::vector<AnomalyReport> reports;
	int numOfRows;

public:
	CLI(DefaultIO* dio) : dio(dio), detector(), options(dio), threshold(0.9), numOfRows(0){
		options.addComand( new UploadCSCFileCommand(dio));
		options.addComand( new UpdateThresholdCommand(dio, &threshold));
		options.addComand( new DetectCommand(dio, &threshold, &numOfRows, &reports));
		options.addComand( new PrintAnomallies(dio, &reports));
		options.addComand(new AnalyzeResultsCommand(dio, &reports, &numOfRows));
		options.addComand(new ExitCommand(dio));
	}
	void start();

	virtual ~CLI();
};
#endif /* CLI_H_ */
