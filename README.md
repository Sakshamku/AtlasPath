# Dijkstra Travel Planner 🌍✈️

## Overview

Dijkstra Travel Planner is a graph-based route optimization system developed in C++. The application models a global transportation network and computes the optimal travel path between cities using Dijkstra's Shortest Path Algorithm.

The planner supports multiple transportation modes, including flights, trains, buses, and boats, and allows users to optimize routes based on either travel time or travel cost.

The computed route is automatically visualized on an interactive world map through dynamically generated HTML and Google Maps integration.

---

## Features

* Graph-based travel network representation
* Dijkstra's shortest path algorithm implementation
* Dual optimization modes:

  * Fastest Route
  * Cheapest Route
* Multi-modal transportation support:

  * Plane
  * Train
  * Bus
  * Boat
* CSV-based dataset loading
* Path reconstruction using predecessor tracking
* Interactive route visualization on Google Maps
* Automatic HTML report generation

---

## System Architecture

```text
cities.csv
      │
      ▼
Location Parser
      │
      ▼
Location Nodes

routes.csv
      │
      ▼
Route Parser
      │
      ▼
Route Edges

      ▼
Graph Construction
      │
      ▼
Dijkstra Algorithm
      │
      ▼
Path Reconstruction
      │
      ▼
HTML Route Generator
      │
      ▼
Interactive Map Visualization
```

---

## Project Structure

```text
Dijkstra-Travel-Planner/
│
├── Main.cpp
├── GraphFunctions.h
├── Location.h
├── Route.h
├── Parser.h
├── FileOperations.h
│
├── cities.csv
├── routes.csv
│
├── output.html
├── README.md
└── travel.make
```

---

## Core Components

### Location (Vertex)

Represents a city within the transportation network.

Attributes:

* Country
* Capital/City Name
* Latitude
* Longitude
* Adjacent Routes
* Previous Node (for path reconstruction)
* Distance from Source

---

### Route (Edge)

Represents a transportation connection between two cities.

Attributes:

* Origin
* Destination
* Transportation Mode
* Travel Time
* Travel Cost
* Route Notes

---

### Graph

Stores all locations and routes using an adjacency-list representation.

Responsibilities:

* Graph construction
* Route lookup
* Neighbor discovery
* Dijkstra shortest-path computation
* Path reconstruction

---

## Algorithm

The planner uses Dijkstra's Algorithm to find the optimal path from a source city to a destination city.

### Fastest Route

Uses:

```text
Edge Weight = Travel Time
```

### Cheapest Route

Uses:

```text
Edge Weight = Travel Cost
```

### Time Complexity

```text
O((V + E) log V)
```

Where:

* V = Number of Cities
* E = Number of Routes

Priority queues are used to efficiently select the next city with the smallest tentative distance.

---

## Dataset

### cities.csv

Contains city information:

```csv
Country,Capital,Latitude,Longitude
India,New Delhi,28.6167,77.2167
France,Paris,48.8333,2.33333
```

### routes.csv

Contains transportation connections:

```csv
Origin,Destination,Transport,Time,Cost,Notes
Amsterdam,Berlin,train,6.3,185,...
Amsterdam,Berlin,bus,10.25,70.5,...
```

Multiple transportation modes may exist between the same pair of cities.

---

## Route Visualization

After computing the optimal path, the application generates an HTML file that:

* Displays cities as map markers
* Draws travel paths between cities
* Shows transportation details
* Provides route information on interaction

Example Route:

```text
Rome
 → Paris
 → Brussels
 → Amsterdam
 → Berlin
 → Astana
```

---

## How to Run

Compile:

```bash
g++ Main.cpp -o travelplanner
```

Execute:

```bash
travelplanner cities.csv routes.csv output.html Italy Kazakhstan cheapest
```

Parameters:

```text
1. Cities File
2. Routes File
3. Output HTML File
4. Origin City
5. Destination City
6. Preference (fastest | cheapest)
```

Example:

```bash
travelplanner cities.csv routes.csv output.html India Japan fastest
```

---

## Learning Outcomes

This project demonstrates:

* Graph Data Structures
* Adjacency Lists
* Dijkstra's Algorithm
* Priority Queues
* Path Reconstruction
* File Parsing
* Object-Oriented Programming
* Geographic Data Visualization
* HTML Generation

---

## Future Improvements

* Modern Google Maps API / OpenStreetMap integration
* A* Pathfinding Algorithm
* Real-time travel datasets
* Interactive web interface
* Route statistics dashboard
* Transportation mode filtering
* Distance calculation using Haversine Formula

---

## Author

Saksham Kumar

Graph Algorithms • Data Structures • C++ • Route Optimization
