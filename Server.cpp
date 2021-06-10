#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <sstream>
#include "mongoose/mongoose.h"
#include "detectorFiles/SimpleAnomalyDetector.h"
#include "detectorFiles/HybridAnomalyDetector.h"
#include "detectorFiles/timeseries.h"

static struct mg_serve_http_opts s_http_server_opts;
using namespace std;

struct Files {
	std::string file1;
	std::string file2;
	std::string algo;
};

Files extractData(char* buf) {
	Files files;
	files.file1 = strtok(buf, "*");
	files.file2 = strtok(NULL, "*");
	files.algo = strtok(NULL, "*");
	return files;
}

std::string convertVecrtorToJASON(std::vector<AnomalyReport>& reports) {
	std::stringstream json;
	json << "[" << endl;
	for (AnomalyReport report : reports) {
		json << " {" << std::endl;
		json << "  \"timeStep\":" << "\"" << report.timeStep << "\"," << endl;
		json << "  \"columns\":" << "\"" << report.description << "\"" << endl;
		json << " }," << endl;
	}
	json << "];";
	return json.str();
}

static void ev_handler(struct mg_connection* nc, int ev, void* p) {

	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message* test = (struct http_message*)p;
		char* data = new char[strlen((test->body.p)) + 1];
		strcpy(data, test->body.p);
		Files files = extractData(data);

		TimeSeriesAnomalyDetector* detector;
		if (files.algo.compare("hybrid")) {
			detector = new HybridAnomalyDetector();
		}
		else {
			detector = new SimpleAnomalyDetector();
		}
		std::thread l([files, detector]() -> void {
			TimeSeries t(files.file1.c_str());
			detector->learnNormal(t); });
		TimeSeries ts(files.file2.c_str());
		l.join();
		std::vector<AnomalyReport> v = detector->detect(ts);

		string s = convertVecrtorToJASON(v);
		cout << "Sent now" << endl;
		struct http_message* hm = (struct http_message*)p;
		cout << s << endl;
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: "
			"application/json\r\nContent-Length: %d\r\n\r\n%s",
			(int)strlen(s.c_str()), s.c_str());

		delete[] data;
		delete detector;
	}
}

int initServer(int port) {
	struct mg_mgr mgr;
	struct mg_connection* nc;
	std::string portToChar = std::to_string(port);
	static char const* sPort = portToChar.c_str();
	mg_mgr_init(&mgr, NULL);
	std::cout << "Starting web server on port " << sPort << std::endl;
	nc = mg_bind(&mgr, sPort, ev_handler);
	if (nc == NULL) {
		std::cout << "failed to create listener" << std::endl;
	}
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";
	s_http_server_opts.enable_directory_listing = "yes";
	while (true) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	mg_mgr_free(&mgr);
	return 0;
}


int main(void) {
	int port;
	std::cout << "Select server port" << std::endl;
	std::cin >> port;
	if (std::cin.fail()) {
		port = 3030;
	}
	initServer(port);
	return 0;
}
