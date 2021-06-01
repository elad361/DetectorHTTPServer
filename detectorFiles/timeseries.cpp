//Author: Elad Shoham 205439649

#include "timeseries.h"

#include <iostream>

void TimeSeries::setFeatures(stringstream& file) {
	std::string feature, line;
	std::getline(file, line);
	std::stringstream sStream(line);
	while (std::getline(sStream, feature, ','))
	{
		values.insert({ feature,std::vector<float>() });
		features.push_back(feature);
	}
}

void TimeSeries::insertLines(stringstream& file)
{
	std::string line;
	while (std::getline(file, line))
	{
		int counter = 0;
		std::string  val;
		std::stringstream sStream(line);
		while (std::getline(sStream, val, ',') && counter < features.size())
		{
			values[features[counter]].push_back(std::stof(val));
			counter++;
		}
		++size;
	}
}

template <class T>
int getIndex(const vector<T>& vector, const T val)
{
	auto it = std::find(vector.begin(), vector.end(), val);
	return (it != vector.end()) ? it - vector.begin() : -1;
}

float TimeSeries::getSameRowFeatureVal(const std::string given, float val, const std::string wanted)
{
	int index = getIndex(values[given], val);
	return (index != -1) ? values[wanted].at(index) : -1;
}

void TimeSeries::putPointsIntoVector(std::string feature1, std::string feature2, std::vector<Point*>& v) const {
	for (int i = 0; i < size; i++) {
		v.push_back(new Point(values.at(feature1)[i], values.at(feature2)[i]));
	}
}

Line TimeSeries::getLineReg(std::string f1, std::string f2, std::vector<Point*>& v) const{
	return linear_reg(v.data(), size);
}

Circle TimeSeries::getMinCircle(std::string f1, std::string f2, std::vector<Point*>& v) const {
	return findMinCircle(v.data(), size);
}