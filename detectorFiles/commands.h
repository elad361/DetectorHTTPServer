#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"
#include <algorithm>

using namespace std;

struct Sequence {
	int begin;
	int end;
};

class DefaultIO {
public:
	virtual string read() = 0;
	virtual void write(string text) = 0;
	virtual void write(float f) = 0;
	virtual void read(float* f) = 0;
	virtual ~DefaultIO() {}
};

class StandardIO : public DefaultIO
{
public:
	virtual std::string read() {
		std::string s;
		cin >> s;
		return s;
	}

	virtual void write(string text) { std::cout << text; }

	virtual void write(float f) { std::cout << f; }

	virtual void read(float* f) { std::cin >> *f; }

	virtual ~StandardIO() {}
};


class Command {
public:
	DefaultIO* dio;
	std::string description;
	Command(DefaultIO* dio, std::string desc) :dio(dio), description(desc) {}
	std::string getDescription() { return description; }
	virtual void execute() = 0;
	virtual ~Command() {}
};

class MacroCommand : public Command {
	vector<Command*> list;
public:
	MacroCommand(DefaultIO* dio) : Command(dio, "sequence of Commands") {}

	MacroCommand(DefaultIO* dio, std::vector<Command*> v) : Command(dio, "sequence of Commands"), list(v) {}

	vector<Command*>* getVec() { return &list; }

	void addComand(Command* c) { list.push_back(c); }

	void execute() {
		for (auto it = list.begin(); it < list.end(); ++it) {
			(*it)->execute();
		}
	}
};

//class that gets a file line by line and saves it
class UploadAFile : public Command {

	std::string savedName;

public:

	UploadAFile(DefaultIO* dio) : Command(dio, "Upload a File"), savedName(" ") { }

	virtual void changeName(std::string newName) { savedName = newName; }

	virtual void execute() {
		std::ofstream csvFile(savedName);
		std::string line = dio->read();
		while (line.compare("done") != 0) {
			csvFile << line << endl;
			line = dio->read();
		}
	}
};

//class for uploading test/train files to the server
class UploadCSCFileCommand : public Command {

public:

	UploadCSCFileCommand(DefaultIO* dio) : Command(dio, "upload a time series csv file") {}
	
	virtual void execute() {
		dio->write("Please upload your local train CSV file.\n");
		UploadAFile uploader(dio);
		uploader.changeName("anomalyTrain.csv");
		uploader.execute();
		dio->write("Upload complete.\nPlease upload your local test CSV file.\n");
		uploader.changeName("anomalyTest.csv");
		uploader.execute();
		dio->write("Upload complete.\n");
	}
};

class PrintMenuCommand : public Command
{
	std::vector<Command*> options;
public:
	PrintMenuCommand(DefaultIO* dio) : Command(dio, "Print Menu") {}

	PrintMenuCommand(DefaultIO* dio, std::vector<Command*> v) : Command(dio, "Print Menu"), options(v) {}

	void addOption(Command* c) { options.push_back(c); }

	void addSeveralOptions(vector<Command*> v) { options.insert(options.begin(), v.end(), v.end()); }


	virtual void execute() {
		int lineNum = 0;
		for (auto it = options.begin(); it < options.end(); ++it) {
			++lineNum;
			stringstream s;
			s << lineNum;
			dio->write(s.str() + "." + (*it)->getDescription() + "\n");
		}
	}
};

class UpdateThresholdCommand : public Command {
	float* thre;
public:
	UpdateThresholdCommand(DefaultIO* dio, float* f) : Command(dio, "algorithm settings"), thre(f) {}

	void changeThresholdPointer(float* f) { thre = f; }

	void execute() {
		stringstream s;
		s << *thre;
		dio->write("The current correlation threshold is " + s.str() + "\nType a new threshold\n");
		dio->read(thre);
		while (*thre >= 1 || *thre <= 0) {
			dio->write("please choose a value between 0 and 1.\n");
			dio->read(thre);
		}
	}
};

class DetectCommand : public Command {

	float* threshold;

	int* numOfRows;

	HybridAnomalyDetector detector;
	
	vector<AnomalyReport>* reports;
public:

	DetectCommand(DefaultIO* dio, float* f, int* rows, vector<AnomalyReport>* v) : Command(dio, "detect anomalies"), threshold(f) , reports(v), detector(), numOfRows(rows){}

	void execute() {
		detector.setThreshold(*threshold);
		TimeSeries train("anomalyTrain.csv");
		detector.learnNormal(train);
		TimeSeries test("anomalyTest.csv");
		*numOfRows = test.getSize();
		*reports = detector.detect(test);
		dio->write("anomaly detection complete.\n");
	}
};

class PrintAnomallies : public Command {
	vector<AnomalyReport>* reports;
public: 
	PrintAnomallies(DefaultIO* dio, vector<AnomalyReport>* anomallies) : Command(dio, "display results"), reports(anomallies) {}
	void execute() {
		for (auto it = reports->begin(); it < reports->end(); ++it) {
			stringstream s;
			s << it->timeStep;
			dio->write(s.str() + "	" + it->description + "\n");
		}
		dio->write("Done.\n");
	}
};

class AnalyzeResultsCommand : public Command {
	vector<AnomalyReport>* reports;

	int* numOfRows;

public:

	AnalyzeResultsCommand(DefaultIO* dio, vector<AnomalyReport>* v, int* rows) : 
						  Command(dio, "upload anomalies and analyze results"),  reports(v) , numOfRows(rows){}

	void execute() {
		//std::stable_sort(reports->begin(), reports->end(), [](const AnomalyReport& r1, const AnomalyReport& r2) { return r1.timeStep < r2.timeStep; });
		//std::stable_sort(reports->begin(), reports->end(), [](const AnomalyReport& r1, const AnomalyReport& r2)
			//{return ((int)(r1.description.at(0)) + (int)(r1.description.at(2)) < (int)(r2.description.at(0)) + (int)(r2.description.at(2))); });
		std::vector<Sequence> sequences;
		auto it = reports->begin();
		while (it < reports->end()) {
			Sequence s;
			int n = it->timeStep;
			s.begin = n;
			++it;
			while (it < reports->end() && (n + 1) == it->timeStep && it->description.compare((it - 1)->description) == 0) {
				++it;
				++n;
			}
			s.end = n;
			sequences.push_back(s);
		}
		std::vector<Sequence> fromUser;
		dio->write("Please upload your local anomalies file.\n");
		std::string line = dio->read();
		while (line.compare("done") != 0) {
			stringstream stream(line);
			std::string store;
			std::getline(stream, store, ',');
			Sequence s;
			s.begin = std::stoi(store);
			getline(stream, store);
			s.end = std::stoi(store);
			fromUser.push_back(s);
			line = dio->read();
		}
		dio->write("done\n");
		int fp = sequences.size();
		for (auto it = sequences.begin(); it < sequences.end(); ++it) {
			for (auto it2 = fromUser.begin(); it2 < fromUser.end(); ++it2) {
				if ((it->begin >= it2->begin && it->begin <= it2->end) || (it->end >= it2->begin && it->end <= it2->end) || (it2->begin >= it->begin && it2->end <= it->end)) {
					--fp;
					it2 = fromUser.end() - 1;
				}
			}
		}
		float truePositive = (sequences.size() - fp) / fromUser.size();
		truePositive = int(truePositive * 1000);
		truePositive = float(truePositive / 1000);
		std::stringstream s;
		s << truePositive;
		dio->write("True Positive Rate: " + s.str() + "\n");
		float falseAlarm = (float)((float)(fp) / ((float)(*numOfRows) - (float)(reports->size())));
		int temp = (int)(falseAlarm * 1000);
		falseAlarm = ((float)temp / 1000);
		s.str("");
		s << falseAlarm;
		dio->write("False Positive Rate: " + s.str() + "\n");
	}
};

class ExitCommand : public Command {
public:
	ExitCommand(DefaultIO* dio) : Command(dio, "exit") {}
	void execute() {}
};
#endif /* COMMANDS_H_ */