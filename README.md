CoolCity: Sub-Street Chilled-Water Utility Simulation

Data Structures and Algorithms (C++) Mini Project

Student Details

Name: Yatharth Dharmesh Mishra

Roll Number: 150096725055

Class: Larry Page

Faculty: Dr. Aarti Pardeshi

College: ITM Skills University

Academic Year: 2025–2029

⸻

1. Project Title

CoolCity: Sub-Street Chilled-Water Utility Simulation

CoolCity is a C++-based simulation system designed to model and manage a city’s underground district cooling infrastructure. The project demonstrates the practical application of Data Structures and Algorithms in solving real-world utility management problems such as pipe tracking, pressure monitoring, network routing, component searching, and power allocation.

⸻

2. Problem Statement

District cooling systems transport chilled water through underground pipelines to cool multiple buildings from a centralized cooling plant. Managing such systems efficiently requires fast access to infrastructure data, safe pressure management, fair handling of restart requests, efficient searching mechanisms, and intelligent resource allocation.

Traditional systems often face challenges such as:

* Slow lookup of pipe information.
* Lack of rollback mechanisms for dangerous pressure spikes.
* Inefficient handling of cooling restart requests.
* Slow searching of pipes, valves, and pumps.
* Poor analysis of cooling usage.
* No structured underground network representation.
* Inability to determine minimum-energy delivery routes.
* Inefficient allocation of power resources.

The objective of this project is to develop a complete simulation system using Data Structures and Algorithms to address these challenges efficiently.

⸻

3. Objectives

The main objectives of the project are:

* Maintain an organized registry of cooling pipes.
* Store and rollback pressure changes.
* Process building restart requests fairly.
* Search infrastructure components efficiently.
* Sort city blocks according to cooling consumption.
* Represent the underground cooling network.
* Calculate minimum-energy routes.
* Allocate electrical power according to demand priority.
* Demonstrate real-world applications of Data Structures and Algorithms.

⸻

4. System Overview / Architecture

The CoolCity system consists of multiple modules, each responsible for a specific functionality.

CoolCity System
│
├── Pipe Registry
├── Pressure Log
├── Startup Queue
├── Component Finder
├── Usage Sorter
├── Utility Map
│     └── Energy Optimizer
└── Power Allocator

Module Description

Pipe Registry

Maintains records of cooling pipes and specifications.

Pressure Log

Stores pressure history and supports rollback operations.

Startup Queue

Handles cooling restart requests in the order they are received.

Component Finder

Searches for pipes, valves, and pumps using serial numbers.

Usage Sorter

Sorts city blocks based on cooling consumption.

Utility Map

Represents the underground network as a graph.

Energy Optimizer

Finds minimum-energy paths using Dijkstra’s Algorithm.

Power Allocator

Allocates power using priority-based scheduling.

⸻

5. Data Structures and Algorithms Used

Module	Data Structure / Algorithm
Pipe Registry	Hash Table (unordered_map)
Pressure Log	Stack
Startup Queue	Queue
Component Finder	Linear Search, Binary Search
Usage Sorter	STL Sort
Utility Map	Graph (Adjacency List)
Energy Optimizer	Dijkstra’s Algorithm
Power Allocator	Priority Queue (Max Heap)

⸻

6. Implementation Approach

The project is implemented using Object-Oriented Programming principles in C++.

Pipe Registry

Uses an unordered_map to provide fast insertion, deletion, and retrieval of pipe information.

Pressure Log

Uses a stack to maintain pressure history and support undo operations.

Startup Queue

Uses a queue to process restart requests in First-In-First-Out order.

Component Finder

Stores components in a vector and performs Linear Search and Binary Search.

Usage Sorter

Uses STL sort() to organize city blocks according to cooling usage.

Utility Map

Represents network connections using an adjacency list graph structure.

Energy Optimizer

Implements Dijkstra’s Algorithm to determine the least-energy pumping route.

Power Allocator

Uses a max-heap priority queue to prioritize cooling fans with the highest demand.

⸻

7. Time and Space Complexity Analysis

Operation	Time Complexity
Pipe Lookup	O(1) Average
Pipe Insertion	O(1) Average
Pressure Push/Pop	O(1)
Queue Enqueue/Dequeue	O(1)
Linear Search	O(n)
Binary Search	O(log n)
Sorting	O(n log n)
Graph Traversal	O(V + E)
Dijkstra Algorithm	O((V + E) log V)
Priority Queue Operations	O(log n)

Where:

* n = Number of elements
* V = Number of vertices
* E = Number of edges

Space Complexity

Module	Space Complexity
Hash Table	O(n)
Stack	O(n)
Queue	O(n)
Vector	O(n)
Graph	O(V + E)
Priority Queue	O(n)

⸻

8. Execution Steps

Prerequisites

* C++ Compiler (GCC or Clang)
* Visual Studio Code or any C++ IDE

Compilation

g++ coolcity.cpp -o coolcity

Execution

./coolcity

Program Flow

1. Launch the application.
2. Select a module from the main menu.
3. Perform operations such as adding pipes, recording pressure, searching components, or finding routes.
4. View results displayed in the console.
5. Exit the application when finished.

⸻

9. Sample Inputs and Outputs

Pipe Registry

Input:

Add Pipe
Serial Number: 101
Material: Steel
Diameter: 300

Output:

Pipe Added Successfully

⸻

Pressure Log

Input:

Record Pressure: 150
Record Pressure: 180
Undo

Output:

Pressure Rolled Back
Current Pressure: 150

⸻

Startup Queue

Input:

Building A
Building B
Building C

Output:

Processing Building A

⸻

Energy Optimizer

Output:

Shortest Path:
Plant -> Building A -> Building C
Total Energy Cost: 15

⸻

Power Allocator

Output:

Fan 3 Allocated First
Highest Demand Detected

⸻

10. Screenshots

Add screenshots of the following:

Main Menu

Pipe Registry

Pressure Log

Startup Queue

Component Search

Usage Sorting

Utility Map

Dijkstra Output

Power Allocation

⸻

11. Results and Observations

The CoolCity system successfully demonstrates the practical implementation of Data Structures and Algorithms in a realistic utility management scenario.

Observations

* Hash tables significantly improve lookup efficiency.
* Stack structures are effective for rollback operations.
* Queue structures ensure fair request processing.
* Binary Search improves search efficiency after sorting.
* Graph structures provide an intuitive representation of utility networks.
* Dijkstra’s Algorithm effectively identifies minimum-energy routes.
* Priority Queues optimize resource allocation based on demand.

Results

* All project modules were successfully implemented.
* The application provides efficient management of cooling infrastructure.
* The system demonstrates multiple DSA concepts within a single integrated project.

⸻

12. Conclusion

CoolCity: Sub-Street Chilled-Water Utility Simulation successfully integrates multiple Data Structures and Algorithms into a single practical application.

The project demonstrates the implementation of:

* Hash Tables
* Stacks
* Queues
* Searching Algorithms
* Sorting Algorithms
* Graphs
* Dijkstra’s Algorithm
* Priority Queues

The system improves organization, efficiency, and resource management while providing a realistic simulation of district cooling operations. This project highlights the importance of Data Structures and Algorithms in solving real-world engineering and infrastructure management problems.

⸻

GitHub Repository

Repository Name:

coolcity-dsa-simulation

Repository Link:

https://github.com/2025yatharthm-gif/coolcity-dsa-simulation