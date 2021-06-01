//Author: Elad Shoham 205439649

#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::~SimpleAnomalyDetector() { }

void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts)
{
	vector<std::string> features = ts.getFeatures();
	int size = ts.getSize();
	for (auto it = features.begin(); it != features.end(); ++it)
	{
		for (auto it2 = it + 1; it2 < features.end(); ++it2)
		{
			checkCorrlation(*it, *it2, ts);
		}
	}
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts) {
	vector<AnomalyReport> v;
	for (auto it = cf.begin(); it < cf.end(); ++it) {
		std::vector<Point*> points;
		ts.putPointsIntoVector(it->feature1, it->feature2, points);
		checkAnomalies(points, *it, v);
		for (auto it2 = points.begin(); it2 < points.end(); ++it2) {
			delete* it2;
		}
	}
	return v;
}

void SimpleAnomalyDetector::checkCorrlation(std::string f1, std::string f2, const TimeSeries& ts) {
	float corr = pearson(ts.getFeatureVals(f1).data(), ts.getFeatureVals(f2).data(), ts.getSize());
	if (abs(corr) >= threshold) {
		vector<Point*> v;
		ts.putPointsIntoVector(f1, f2, v);
		updateRegCorrelatedFeatures(corr, f1, f2, ts, v);
		for (auto it = v.begin(); it < v.end(); ++it) {
			delete* it;
		}
	}
}

void SimpleAnomalyDetector::updateRegCorrelatedFeatures(float corr, std::string f1, std::string f2, const TimeSeries& ts, std::vector<Point*>& v) {
	correlatedFeatures reg;
	reg.corrlation = corr;
	reg.feature1 = f1;
	reg.feature2 = f2;
	reg.isACircle = 0;
	reg.threshold = 0;
	reg.lin_reg = linear_reg(v.data(), ts.getSize());
	for (auto it = v.begin(); it < v.end(); ++it) {
		float d = dev(**it, reg.lin_reg);
		if (d > reg.threshold) {
			reg.threshold = d;
		}
	}
	reg.threshold *= 1.1;
	cf.push_back(reg);
}

void SimpleAnomalyDetector::findAnomaliesByLine(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar) {
	for (auto it = v.begin(); it < v.end(); ++it) {
		float d = dev(**it, corrf.lin_reg);
		if (d > corrf.threshold) {
			AnomalyReport report(corrf.feature1 + '-' + corrf.feature2, it - v.begin() + 1);
			ar.push_back(report);
		}
	}
}

void SimpleAnomalyDetector::checkAnomalies(vector<Point*>& v, correlatedFeatures& corrf, vector<AnomalyReport>& ar) {
	findAnomaliesByLine(v, corrf, ar);
}