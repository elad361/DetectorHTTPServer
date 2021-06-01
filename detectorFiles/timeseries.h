//Author: Elad Shoham 205439649

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <map>
#include <vector>
#include <string.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "anomaly_detection_util.h"
#include "minCircle.h"
using namespace std;

class TimeSeries {
	std::vector<std::string> features; // list of the features
	std::map<std::string, std::vector<float>> values;
	int size;
	friend class TimeSeriesAnomalyDetector;

public:

	TimeSeries(const char* file) : features(), values(), size(0) {
		std::stringstream fileStream(file);
		setFeatures(fileStream);
		insertLines(fileStream);
	}

	//return the val of wanted feature by a given feature and the matching val (by row)
	float getSameRowFeatureVal(std::string given, float val, std::string wanted);

	//return vector of all of the features
	const vector<std::string>& getFeatures() const { return features; }

	//return a vector of all the vals of a given feature
	vector<float> getFeatureVals(std::string feature) const { return values.at(feature); }

	//return the num of rows
	int getSize() const { return size; }

	//insert Two-dimensional points of feature1/2 into a given vector
	void putPointsIntoVector(std::string feature1, std::string feature2, std::vector<Point*>& v) const;

	//return the linear reg between two features
	Line getLineReg(std::string f1, std::string f2, std::vector<Point*>& v) const;

	Circle getMinCircle(std::string f1, std::string f2, std::vector<Point*>& v) const;

private:
	//reads the first line to know the titles
	void setFeatures(stringstream& file);

	//Reads the lines and pushes the values to the wright vector
	void insertLines(stringstream& file);
};

//return the index of a given value
template <class T>
int getIndex(const vector<T>& vector, T val);


#endif /* TIMESERIES_H_ */
