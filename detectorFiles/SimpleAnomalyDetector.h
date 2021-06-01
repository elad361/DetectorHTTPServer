//Author: Elad Shoham 205439649

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>
#include "minCircle.h"

struct correlatedFeatures {
	string feature1, feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float x, y;
	float threshold;
	bool isACircle;
};


class SimpleAnomalyDetector :public TimeSeriesAnomalyDetector {

public:


	float threshold;

	SimpleAnomalyDetector() : threshold(0.9) {}

	virtual ~SimpleAnomalyDetector();

	//learn the data of a Proper flight
	virtual void learnNormal(const TimeSeries& ts);

	virtual void setThreshold(float t) { threshold = t; }

	//return a vector of anomaly report by a given time series
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);

	vector<correlatedFeatures> getNormalModel() {
		return cf;
	}
private:

	// check the correlation of two features and adding correlatedFeature to cf if nedded (in the needed way)
	virtual void checkCorrlation(std::string f1, std::string f2, const TimeSeries& ts);

protected:

	vector<correlatedFeatures> cf;

	//send the report to the currect way to find anomalies (by line\circle
	virtual void checkAnomalies(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar);

	//finding anomalies by a reg line
	virtual void findAnomaliesByLine(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar);
	
	//adding a reg correlated features to cf
	virtual void updateRegCorrelatedFeatures(float corr, std::string f1, std::string f2, const TimeSeries& ts, std::vector<Point*>& v);
};

#endif /* SIMPLEANOMALYDETECTOR_H_ */
