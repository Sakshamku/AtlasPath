#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Location.h"

using namespace std;

static vector<string> splitCsvLine(const string& line){
	vector<string> fields;
	string field;
	bool inQuotes = false;

	for(size_t i = 0; i < line.size(); i++){
		char c = line[i];

		if(inQuotes){
			if(c == '"'){
				if(i + 1 < line.size() && line[i + 1] == '"'){
					field.push_back('"');
					i++;
				}
				else{
					inQuotes = false;
				}
			}
			else{
				field.push_back(c);
			}
		}
		else{
			if(c == ','){
				fields.push_back(field);
				field.clear();
			}
			else if(c != '\r'){
				if(c == '"'){
					inQuotes = true;
				}
				else{
					field.push_back(c);
				}
			}
		}
	}

	fields.push_back(field);
	return fields;
}

vector<Location*> locationParser(string filename, vector<Route*> routes){
	fstream locations(filename.c_str());

	vector<Location*> cities;

	string line;

	while(getline(locations, line)){
		if(line.size() == 0){
			continue;
		}

		vector<string> fields = splitCsvLine(line);
		if(fields.size() < 4){
			continue;
		}

		if(fields[0] == "Country" || fields[0] == "country"){
			continue;
		}

		Location* node = new Location(fields[0], fields[1], atof(fields[2].c_str()), atof(fields[3].c_str()));

		vector<Route*>::iterator it = routes.begin();

		while(it != routes.end()){
			if((*it) -> originS.compare(node -> capital) == 0){
				(*it) -> origin = node;
				node -> routes.push_back((*it));
			}
			else if((*it) -> destinationS.compare(node -> capital) == 0){
				(*it) -> destination = node;
			}
			++it;
		}

		cities.push_back(node);

	}
	cout << "Cities Parsed from: " << filename << endl;
	return cities;
}

vector<Route*> routeParser(string filename){

	fstream routes(filename.c_str());

	vector<Route*> allRoutes;
	string line;

	while(getline(routes, line)){
		if(line.size() == 0){
			continue;
		}

		vector<string> fields = splitCsvLine(line);
		if(fields.size() < 6){
			continue;
		}

		if(fields[0] == "Origin" || fields[0] == "origin"){
			continue;
		}

		string originS = fields[0];
		string destinationS = fields[1];
		string type = fields[2];
		float time = atof(fields[3].c_str());
		float cost = atof(fields[4].c_str());

		string lastUpdated = "";
		string note = fields.back();

		if(fields.size() >= 7){
			lastUpdated = fields[5];
			note = fields[6];
		}

		// Create Location objects for route connection - these will be fully populated later in locationParser
		// Use placeholder nullptrs; actual Location objects will be linked in locationParser
		Location* origin = nullptr;
		Location* destination = nullptr;
		Route* edge = new Route(origin, destination, type, time, cost, lastUpdated, note);
		edge -> destinationS = destinationS;
		edge -> originS = originS;

		allRoutes.push_back(edge);
	}

	cout << "Routes Parsed from: " << filename << endl;
	return allRoutes;
}



#endif
