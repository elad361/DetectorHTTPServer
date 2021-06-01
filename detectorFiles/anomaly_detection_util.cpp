
#include <math.h>
#include "anomaly_detection_util.h"
#include <vector>

float avg(float* x, int size) {
	float sum = 0;
	for (int i = 0; i < size; sum += x[i], i++);
	return sum / size;
}

// returns the variance of X and Y
float var(float* x, int size) {
	float av = avg(x, size);
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += x[i] * x[i];
	}
	return sum / size - av * av;
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size) {
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += x[i] * y[i];
	}
	sum /= size;

	return sum - avg(x, size) * avg(y, size);
}


// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size) {
	return cov(x, y, size) / (sqrt(var(x, size)) * sqrt(var(y, size)));
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size) {
	std::vector<float> x, y;
	for (int i = 0; i < size; i++) {
		x.push_back(points[i]->x);
		y.push_back(points[i]->y);
	}
	float a = cov(x.data(), y.data(), size) / var(x.data(), size);
	float b = avg(y.data(), size) - a * (avg(x.data(), size));

	return Line(a, b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point** points, int size) {
	Line l = linear_reg(points, size);
	return dev(p, l);
}

// returns the deviation between point p and the line
float dev(Point p, Line l) {
	float x = p.y - l.f(p.x);
	if (x < 0)
		x *= -1;
	return x;
}




