#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>

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


/*
Parses and builds Route objects from file routes.csv
*/

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

		Location* origin = new Location();
		Location* destination = new Location();
		Route* edge = new Route(origin, destination, type, time, cost, lastUpdated, note);
		edge -> destinationS = destinationS;
		edge -> originS = originS;

		allRoutes.push_back(edge);
	}

	cout << "Routes Parsed from: " << filename << endl;
	return allRoutes;
}



/*
Generates an appropriately formatted html file that displays the route information passed in by the stack fo cities and routes
*/
void outputGenerator(string filename, stack<Location*> cities, stack<Route*> routes, bool costOrTime){
	
	ofstream output(filename.c_str());
	output << "<HTML><HEAD><TITLE>Shortest path from Italy to Kazakhstan</TITLE></HEAD><script type='text/javascript' src='http://maps.google.com/maps/api/js?sensor=false'></script><script>function initialize() { var myOptions = { zoom: 3, center: new google.maps.LatLng(0, 0), mapTypeId: google.maps.MapTypeId.ROADMAP};var map=new google.maps.Map(document.getElementById('map'), myOptions);\n";

	int markerCount = 0;
	int contentStringCount = 0;
	Location* origin;
	Location* destination;
	Route* route;

	float cost;

	

	while(!cities.empty() && !routes.empty()){

		//cout << routes.size() << endl << cities.size() << "--" << endl;		//debug

		origin = cities.top();
		cities.pop();
		destination = cities.top();
						
		output << "var marker" << markerCount << " = new google.maps.Marker({ position: new google.maps.LatLng(" << origin -> lat << ", " << origin -> lon << "), map: map, title: \"" << origin -> capital << ", " << origin -> country << "\"});\n";
		
		markerCount++;
			
		output << "var marker" << markerCount << " = new google.maps.Marker({ position: new google.maps.LatLng(" << destination -> lat << ", " << destination -> lon << "), map: map, title: \"" << destination -> capital << ", " << destination -> country << "\"});\n";
		
		markerCount++;

		route = routes.top();
		routes.pop();

		cost = route -> cost;

		output << "var contentString" << contentStringCount << " = \"" << origin -> capital << ", " << origin -> country << " --> " << destination -> capital << ", " << destination -> country << "(" << route -> transport << " - " << route -> time << " hours - $" << cost << ")\"; var path" << contentStringCount << " = new google.maps.Polyline({ path: [new google.maps.LatLng(" << origin -> lat << ", " << origin -> lon << "), new google.maps.LatLng(" << destination -> lat << ", " << destination -> lon << ")], strokeColor: '#0000FF', strokeOpacity: 1.0, strokeWeight: 2}); path"<< contentStringCount <<".setMap(map); google.maps.event.addListener(path" << contentStringCount << ", 'click', function(event) { alert(contentString" << contentStringCount << ");});\n";

		contentStringCount++;
		
	}

	output << "} google.maps.event.addDomListener(window, 'load', initialize); </script></HEAD><BODY><div id='map' style='width:100%;height:100%;'></div></BODY></HTML>";
	output.close();

	cout << "Output File Generated: " << filename << endl;
}


#endif
