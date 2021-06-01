//Elad Shoham 205439649
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::~HybridAnomalyDetector() { }

void HybridAnomalyDetector::updateMinCircleCorrelatedFeatures(float corr, std::string f1, std::string f2, const TimeSeries& ts, std::vector<Point*>& v) {
	correlatedFeatures circle;
	circle.corrlation = corr;
	circle.feature1 = f1;
	circle.feature2 = f2;
	circle.isACircle = 1;
	Circle c = findMinCircle(v.data(), ts.getSize());
	circle.threshold = c.radius * 1.1;
	circle.x = c.center.x;
	cf.push_back(circle);
}

void HybridAnomalyDetector::checkCorrlation(std::string f1, std::string f2, const TimeSeries& ts) {
	float corr = pearson(ts.getFeatureVals(f1).data(), ts.getFeatureVals(f2).data(), ts.getSize());
	if (abs(corr) >= threshold) {
		vector<Point*> v;
		ts.putPointsIntoVector(f1, f2, v);
		updateRegCorrelatedFeatures(corr, f1, f2, ts, v);
		for (auto it = v.begin(); it < v.end(); ++it) {
			delete* it;
		}
	}
	else if (abs(corr) > 0.5) {
		vector<Point*> v;
		ts.putPointsIntoVector(f1, f2, v);
		updateMinCircleCorrelatedFeatures(corr, f1, f2, ts, v);
		for (auto it = v.begin(); it < v.end(); ++it) {
			delete* it;
		}
	}
}

void HybridAnomalyDetector::checkAnomalies(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar) {
	if (corrf.isACircle) { findAnomaliesByMinCircle(v, corrf, ar); }
	else { findAnomaliesByLine(v, corrf, ar); }
}

void HybridAnomalyDetector::findAnomaliesByMinCircle(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar) {
	for (auto it = v.begin(); it < v.end(); ++it) {
		Point p(corrf.x, corrf.y);
		if (dis(**it, p) > corrf.threshold) {
			AnomalyReport report(corrf.feature1 + '-' + corrf.feature2, it - v.begin() + 1);
			ar.push_back(report);
		}
	}
}

float dis(Point p1, Point p2) {
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}