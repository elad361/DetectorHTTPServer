//Elad Shoham 205439649
#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_
#include "SimpleAnomalyDetector.h"
#include "minCircle.h"
class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector() : SimpleAnomalyDetector() {}
	virtual ~HybridAnomalyDetector();
private:
	// check the correlation of two features and adding correlatedFeature to cf if nedded (in the needed way)
	virtual void checkCorrlation(std::string f1, std::string f2, const TimeSeries& ts);

	//send the report to the currect way to find anomalies (by line\circle
	virtual void checkAnomalies(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar);

	//finding anomalies by a minCircle
	virtual void findAnomaliesByMinCircle(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar);

	void updateMinCircleCorrelatedFeatures(float corr, std::string f1, std::string f2, const TimeSeries& ts, std::vector<Point*>& v);

};

float dis(Point p1, Point p2);

#endif /* HYBRIDANOMALYDETECTOR_H_ */
