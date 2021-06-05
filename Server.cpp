
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

static int mg_http_get_request_len(const char* s, int buf_len);

struct Files {
	std::string file1;
	std::string file2;
	std::string algo;
	int size;
};

Files extractData(const char* buf, size_t size) {
	Files files;
	std::stringstream s;
	s << buf;
	string line;
	for (int i = 0; i < 8; i++) {
		std::getline(s, line);
		if (i == 5) {
			files.size = stoi(line.substr(line.find(': '), line.length()));
		}
	}
	string line2;
	line = "";
	while (getline(s, line2))
	{
		line += line2;
	}
	line = s.str();
	string st = line.substr(mg_http_get_request_len(buf, size), files.size);
	std::stringstream newS(st.substr(0, files.size));
	//strtok data
	//char* copyOfData;
	//strcpy(copyOfData,newS.c_str());
	//char* pch;
	////char args[3];
	//pch = strtok(copyOfData, ":");
	//int i = 0;
	//while (pch != NULL)
	//{
	//	//args[i] = pch;
	//	pch = strtok(NULL, ":");
	//	i++;
	//}
	st = "";
	getline(newS, st);
	while (!st.compare("\n"))
	{
		files.file1 += st;
		getline(newS, st);
	}

	getline(newS, st);
	getline(newS, st);
	while (!st.compare("\n"))
	{
		files.file2 += st;
		getline(newS, st);
	}
	getline(newS, st);
	getline(newS, files.algo);
	return files;
}

void convertVecrtorToJASON(std::vector<AnomalyReport>& reports, string& s) {
	std::stringstream json;
	json << "[" << endl;
	for (AnomalyReport report : reports) {
		json << " {" << std::endl;
		json << "  \"timeStep\":" << report.timeStep << "," << endl;
		json << "  \"columns\":" << report.description << endl;
		json << " }," << endl;
	}
	json << "];";
	s = json.str();
}

static void ev_handler(struct mg_connection* nc, int ev, void* p) {

	if (ev == MG_EV_HTTP_REQUEST) {
		Files files = extractData(nc->recv_mbuf.buf, nc->recv_mbuf.len);
		TimeSeriesAnomalyDetector* detector;
		if (files.algo.compare("hybrid")) {
			detector = new HybridAnomalyDetector();
		}
		else {
			detector = new SimpleAnomalyDetector();
		}
		std::thread l([files, detector]() -> void {
			TimeSeries t(files.file1.c_str());
			cout << "file1: " << files.file1.c_str() << endl;
			detector->learnNormal(t); });
		TimeSeries ts(files.file2.c_str());
		l.join();
		std::vector<AnomalyReport> v = detector->detect(ts);
		string s;
		convertVecrtorToJASON(v, s);
		struct http_message* hm = (struct http_message*) p;

		hm->message.p = s.c_str();
		hm->message.len = s.length();
		mg_serve_http(nc, hm , s_http_server_opts);
		delete detector;
		//mg_serve_http(nc, (struct http_message*) p, s_http_server_opts);
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


