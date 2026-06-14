/*
=====================================================================
 CoolCity: Sub-Street Chilled-Water Utility Simulation
=====================================================================
 A menu-driven C++ DSA mini project simulating an underground
 district cooling network for a city.

 DSA CONCEPTS USED (summary):
   1. unordered_map  -> Pipe Register   (hashing for O(1) lookup)
   2. stack          -> Pressure Log    (LIFO rollback)
   3. queue          -> Startup Queue   (FIFO fairness)
   4. vector         -> Component Finder (Linear & Binary Search)
   5. vector + sort  -> Usage Sorter    (std::sort)
   6. Graph (Adjacency List) -> Utility Map
   7. Dijkstra's Algorithm + Priority Queue -> Energy Optimizer
   8. priority_queue (Max-Heap) -> Power Allocator
=====================================================================
*/

#include <iostream>
#include <unordered_map>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <climits>
#include <string>
#include <limits>

using namespace std;

/* ===================================================================
   MODULE 1: PIPE REGISTER
   -------------------------------------------------------------------
   DSA Concept: unordered_map<int, Pipe>
   An unordered_map is implemented internally using a HASH TABLE.
   The key (serialNumber) is passed through a hash function which
   computes an index into a bucket array. This gives average-case
   O(1) time complexity for insertion, deletion, and lookup, because
   we do not need to scan through every element like we would in a
   vector or list. Worst case (many collisions) is O(n), but with a
   good hash function this is rare in practice.
=================================================================== */
class Pipe {
public:
    int serialNumber;
    string material;
    double diameter;     // in inches
    double length;       // in meters
    double pressureRating; // in PSI

    Pipe() : serialNumber(0), diameter(0), length(0), pressureRating(0) {}

    Pipe(int sn, string mat, double dia, double len, double pr)
        : serialNumber(sn), material(mat), diameter(dia), length(len), pressureRating(pr) {}

    void display() const {
        cout << "Serial No: " << serialNumber
             << " | Material: " << material
             << " | Diameter: " << diameter << " in"
             << " | Length: " << length << " m"
             << " | Pressure Rating: " << pressureRating << " PSI" << endl;
    }
};

class PipeRegister {
private:
    // Hash table mapping serial number -> Pipe object
    unordered_map<int, Pipe> pipes;

public:
    // Insertion: O(1) average case due to hashing
    void addPipe(int sn, const string &mat, double dia, double len, double pr) {
        if (pipes.find(sn) != pipes.end()) {
            cout << "Error: Pipe with serial number " << sn << " already exists.\n";
            return;
        }
        pipes[sn] = Pipe(sn, mat, dia, len, pr);
        cout << "Pipe " << sn << " added successfully.\n";
    }

    // Deletion: O(1) average case
    void deletePipe(int sn) {
        auto it = pipes.find(sn);
        if (it == pipes.end()) {
            cout << "Error: Pipe " << sn << " not found.\n";
            return;
        }
        pipes.erase(it);
        cout << "Pipe " << sn << " deleted successfully.\n";
    }

    // Update: O(1) average case lookup, then field modification
    void updatePipe(int sn, const string &mat, double dia, double len, double pr) {
        auto it = pipes.find(sn);
        if (it == pipes.end()) {
            cout << "Error: Pipe " << sn << " not found.\n";
            return;
        }
        it->second.material = mat;
        it->second.diameter = dia;
        it->second.length = len;
        it->second.pressureRating = pr;
        cout << "Pipe " << sn << " updated successfully.\n";
    }

    // Search: O(1) average case - the key advantage of hashing
    void searchPipe(int sn) const {
        auto it = pipes.find(sn);
        if (it == pipes.end()) {
            cout << "Pipe " << sn << " not found in registry.\n";
            return;
        }
        cout << "Pipe found:\n";
        it->second.display();
    }

    // Display all: O(n) - must visit every element regardless of structure
    void displayAll() const {
        if (pipes.empty()) {
            cout << "No pipes registered yet.\n";
            return;
        }
        cout << "----- All Registered Pipes (" << pipes.size() << ") -----\n";
        for (const auto &entry : pipes) {
            entry.second.display();
        }
    }

    bool exists(int sn) const {
        return pipes.find(sn) != pipes.end();
    }

    int size() const { return (int)pipes.size(); }
};


/* ===================================================================
   MODULE 2: PRESSURE LOG
   -------------------------------------------------------------------
   DSA Concept: stack<double>
   A stack follows LIFO (Last In, First Out) ordering. Pressure
   readings are PUSHed onto the stack as they are recorded. If a
   dangerous spike occurs, the most recent reading can be POPped
   off (rolled back) in O(1) time, restoring the previous safe
   pressure value. This models real-world rollback/undo systems
   where only the most recent change needs to be reversed first.
=================================================================== */
class PressureLog {
private:
    stack<double> pressureStack;

public:
    // Push: O(1)
    void recordPressure(double value) {
        pressureStack.push(value);
        cout << "Pressure reading " << value << " PSI recorded.\n";
    }

    // Top: O(1)
    void viewCurrentPressure() const {
        if (pressureStack.empty()) {
            cout << "No pressure readings recorded yet.\n";
            return;
        }
        cout << "Current pressure: " << pressureStack.top() << " PSI\n";
    }

    // Pop: O(1) - rollback the most recent (dangerous) reading
    void undoLastChange() {
        if (pressureStack.empty()) {
            cout << "No pressure readings to undo.\n";
            return;
        }
        double removed = pressureStack.top();
        pressureStack.pop();
        cout << "Rolled back pressure reading: " << removed << " PSI\n";
        if (!pressureStack.empty())
            cout << "Pressure restored to: " << pressureStack.top() << " PSI\n";
        else
            cout << "Pressure history is now empty.\n";
    }

    // Display history: O(n) - must temporarily empty stack to traverse
    // (stack has no iterator by design, since it enforces LIFO access)
    void displayHistory() const {
        if (pressureStack.empty()) {
            cout << "No pressure history available.\n";
            return;
        }
        stack<double> temp = pressureStack; // copy to preserve original
        cout << "----- Pressure History (most recent first) -----\n";
        while (!temp.empty()) {
            cout << temp.top() << " PSI\n";
            temp.pop();
        }
    }
    bool empty() const { return pressureStack.empty(); }
};


/* ===================================================================
   MODULE 3: STARTUP QUEUE
   -------------------------------------------------------------------
   DSA Concept: queue<string>
   A queue follows FIFO (First In, First Out) ordering. Buildings
   requesting a cooling restart are ENQUEUEd in the order they
   request, and the operator DEQUEUEs (processes) them in the same
   order they arrived. This guarantees fairness - the building that
   requested first is served first, just like a real-world ticketing
   or queueing system.
=================================================================== */
class StartupQueue {
private:
    queue<string> requestQueue;

public:
    // Enqueue: O(1)
    void addRequest(const string &buildingName) {
        requestQueue.push(buildingName);
        cout << "Restart request added for: " << buildingName << endl;
    }

    // Dequeue: O(1)
    void processNextRequest() {
        if (requestQueue.empty()) {
            cout << "No pending restart requests.\n";
            return;
        }
        string building = requestQueue.front();
        requestQueue.pop();
        cout << "Processing restart for: " << building
             << " ... Cooling restored.\n";
    }

    // Front + size traversal: O(n) (queue has no iterator,
    // so we copy to view all pending requests)
    void viewPendingRequests() const {
        if (requestQueue.empty()) {
            cout << "No pending restart requests.\n";
            return;
        }
        queue<string> temp = requestQueue;
        cout << "----- Pending Restart Requests (front to back) -----\n";
        int position = 1;
        while (!temp.empty()) {
            cout << position++ << ". " << temp.front() << endl;
            temp.pop();
        }
    }

    bool empty() const { return requestQueue.empty(); }
};


/* ===================================================================
   MODULE 4: COMPONENT FINDER
   -------------------------------------------------------------------
   DSA Concept: vector<Component> + Linear Search + Binary Search
   Components (pipes, valves, pumps) are stored in a vector indexed
   by serial number.

   - Linear Search: scans every element one by one until a match is
     found. Time Complexity = O(n). Works on unsorted data.

   - Binary Search: repeatedly divides the search interval in half.
     Requires the vector to be SORTED by serialNumber first.
     Time Complexity = O(log n), which is dramatically faster for
     large datasets.
=================================================================== */
class Component {
public:
    int serialNumber;
    string type; // "Pipe", "Valve", "Pump"

    Component() : serialNumber(0), type("") {}
    Component(int sn, string t) : serialNumber(sn), type(t) {}
};

class ComponentFinder {
private:
    vector<Component> components;

public:
    // Insertion at end: O(1) amortized
    void addComponent(int sn, const string &type) {
        components.push_back(Component(sn, type));
        cout << "Component " << sn << " (" << type << ") added.\n";
    }

    // Linear Search: O(n)
    // Checks each element sequentially - works regardless of order.
    void linearSearch(int sn) const {
        for (size_t i = 0; i < components.size(); i++) {
            if (components[i].serialNumber == sn) {
                cout << "[Linear Search] Found at index " << i
                     << " -> Serial: " << components[i].serialNumber
                     << ", Type: " << components[i].type << endl;
                cout << "Time Complexity: O(n)\n";
                return;
            }
        }
        cout << "[Linear Search] Component " << sn << " not found. (O(n))\n";
    }

    // Binary Search: O(log n)
    // Requires components to be sorted by serialNumber first.
    void binarySearch(int sn) {
        // Sort first (O(n log n)) so binary search precondition holds
        sort(components.begin(), components.end(),
             [](const Component &a, const Component &b) {
                 return a.serialNumber < b.serialNumber;
             });

        int low = 0, high = (int)components.size() - 1;
        int comparisons = 0;

        while (low <= high) {
            comparisons++;
            int mid = low + (high - low) / 2;
            if (components[mid].serialNumber == sn) {
                cout << "[Binary Search] Found at index " << mid
                     << " -> Serial: " << components[mid].serialNumber
                     << ", Type: " << components[mid].type << endl;
                cout << "Comparisons made: " << comparisons << endl;
                cout << "Time Complexity: O(log n)\n";
                return;
            } else if (components[mid].serialNumber < sn) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        cout << "[Binary Search] Component " << sn << " not found. (O(log n))\n";
        cout << "Comparisons made: " << comparisons << endl;
    }

    // Display: O(n)
    void displayComponents() const {
        if (components.empty()) {
            cout << "No components registered.\n";
            return;
        }
        cout << "----- Components (" << components.size() << ") -----\n";
        for (const auto &c : components) {
            cout << "Serial: " << c.serialNumber << " | Type: " << c.type << endl;
        }
    }

    int size() const { return (int)components.size(); }
};


/* ===================================================================
   MODULE 5: USAGE SORTER
   -------------------------------------------------------------------
   DSA Concept: vector<Block> + std::sort()
   City blocks are stored in a vector along with their cooling usage.
   std::sort() in C++ STL uses an INTROSORT algorithm (a hybrid of
   QuickSort, HeapSort, and InsertionSort), which provides an average
   and worst-case time complexity of O(n log n). This is far more
   efficient than simple sorts like Bubble Sort (O(n^2)) for large
   datasets, making it ideal for sorting many city blocks by their
   cooling consumption.
=================================================================== */
class Block {
public:
    string blockName;
    double coolingUsage; // in kWh

    Block() : blockName(""), coolingUsage(0) {}
    Block(string name, double usage) : blockName(name), coolingUsage(usage) {}
};

class UsageSorter {
private:
    vector<Block> blocks;

public:
    // Insertion: O(1) amortized
    void addBlock(const string &name, double usage) {
        blocks.push_back(Block(name, usage));
        cout << "Block '" << name << "' added with usage " << usage << " kWh.\n";
    }

    // std::sort: O(n log n) average and worst case (Introsort)
    void sortAscending() {
        sort(blocks.begin(), blocks.end(),
             [](const Block &a, const Block &b) {
                 return a.coolingUsage < b.coolingUsage;
             });
        cout << "Blocks sorted in ASCENDING order of cooling usage. (O(n log n))\n";
    }

    void sortDescending() {
        sort(blocks.begin(), blocks.end(),
             [](const Block &a, const Block &b) {
                 return a.coolingUsage > b.coolingUsage;
             });
        cout << "Blocks sorted in DESCENDING order of cooling usage. (O(n log n))\n";
    }

    // Display: O(n)
    void displayBlocks() const {
        if (blocks.empty()) {
            cout << "No blocks added yet.\n";
            return;
        }
        cout << "----- City Blocks (" << blocks.size() << ") -----\n";
        for (const auto &b : blocks) {
            cout << "Block: " << b.blockName
                 << " | Cooling Usage: " << b.coolingUsage << " kWh" << endl;
        }
    }

    int size() const { return (int)blocks.size(); }
};


/* ===================================================================
   MODULE 6 & 7: UTILITY MAP + ENERGY OPTIMIZER
   -------------------------------------------------------------------
   DSA Concept: Graph using Adjacency List + Dijkstra's Algorithm
                with a Priority Queue (Min-Heap)

   The underground cooling network is modeled as a weighted, 
   undirected GRAPH:
     - Nodes  = locations (Cooling Plant, Building A, B, C, ...)
     - Edges  = pipe connections, weighted by ENERGY COST to pump
                chilled water along that pipe.

   ADJACENCY LIST representation: each node stores a list of
   (neighbor, weight) pairs. This uses O(V + E) space, which is far
   more memory-efficient than an adjacency MATRIX (O(V^2)) for sparse
   networks like real city pipe layouts.

   DIJKSTRA'S ALGORITHM finds the shortest (minimum energy) path from
   a source node to all other nodes. It uses a MIN-HEAP (priority_queue
   with greater<> comparator) to always expand the node with the
   currently smallest known distance first.
   Time Complexity: O((V + E) log V) using a binary heap.
=================================================================== */
class UtilityMap {
private:
    // Adjacency list: nodeName -> list of (neighborName, energyCost)
    unordered_map<string, vector<pair<string, int>>> adjList;

public:
    // Add a node (location) to the graph: O(1)
    void addNode(const string &name) {
        if (adjList.find(name) == adjList.end()) {
            adjList[name] = vector<pair<string, int>>();
            cout << "Node '" << name << "' added to the network.\n";
        } else {
            cout << "Node '" << name << "' already exists.\n";
        }
    }

    // Add an undirected edge with energy cost: O(1)
    void addConnection(const string &from, const string &to, int energyCost) {
        if (adjList.find(from) == adjList.end()) addNode(from);
        if (adjList.find(to) == adjList.end()) addNode(to);

        adjList[from].push_back({to, energyCost});
        adjList[to].push_back({from, energyCost}); // undirected
        cout << "Connection added: " << from << " <-> " << to
             << " (Energy Cost: " << energyCost << ")\n";
    }

    // Display the adjacency list representation: O(V + E)
    void displayNetwork() const {
        if (adjList.empty()) {
            cout << "Network is empty. Add nodes first.\n";
            return;
        }
        cout << "----- Underground Cooling Network (Adjacency List) -----\n";
        for (const auto &node : adjList) {
            cout << node.first << " -> ";
            if (node.second.empty()) {
                cout << "(no connections)";
            } else {
                for (const auto &edge : node.second) {
                    cout << "[" << edge.first << ", cost:" << edge.second << "] ";
                }
            }
            cout << endl;
        }
    }

    bool hasNode(const string &name) const {
        return adjList.find(name) != adjList.end();
    }

    // Dijkstra's Algorithm: O((V + E) log V)
    // Returns true if a path exists, fills distance and path-tracking maps.
    bool dijkstra(const string &source, const string &destination,
                   unordered_map<string, int> &dist,
                   unordered_map<string, string> &parent) const {

        if (adjList.find(source) == adjList.end() ||
            adjList.find(destination) == adjList.end()) {
            return false;
        }

        // Initialize distances to "infinity"
        for (const auto &node : adjList) {
            dist[node.first] = INT_MAX;
        }
        dist[source] = 0;

        // Min-Heap: pair<distance, nodeName>, smallest distance on top
        priority_queue<pair<int, string>,
                       vector<pair<int, string>>,
                       greater<pair<int, string>>> pq;

        pq.push({0, source});

        while (!pq.empty()) {
            int currentDist = pq.top().first;
            string currentNode = pq.top().second;
            pq.pop();

            // Skip stale entries
            if (currentDist > dist[currentNode]) continue;

            // Relax all neighboring edges
            for (const auto &edge : adjList.at(currentNode)) {
                const string &neighbor = edge.first;
                int weight = edge.second;

                if (dist[currentNode] != INT_MAX &&
                    dist[currentNode] + weight < dist[neighbor]) {
                    dist[neighbor] = dist[currentNode] + weight;
                    parent[neighbor] = currentNode;
                    pq.push({dist[neighbor], neighbor});
                }
            }
        }

        return dist[destination] != INT_MAX;
    }
};

// Wrapper class for the Energy Optimizer menu options.
class EnergyOptimizer {
private:
    const UtilityMap &graph;

public:
    EnergyOptimizer(const UtilityMap &g) : graph(g) {}

    // Find and display shortest path + total cost: O((V+E) log V)
    void findShortestPath(const string &source, const string &destination) const {
        unordered_map<string, int> dist;
        unordered_map<string, string> parent;

        if (!graph.hasNode(source) || !graph.hasNode(destination)) {
            cout << "Error: One or both nodes do not exist in the network.\n";
            return;
        }

        bool pathExists = graph.dijkstra(source, destination, dist, parent);

        if (!pathExists) {
            cout << "No path exists between " << source << " and " << destination << ".\n";
            return;
        }

        // Reconstruct path by walking backward through parent map
        vector<string> path;
        string current = destination;
        path.push_back(current);
        while (current != source) {
            current = parent[current];
            path.push_back(current);
        }
        reverse(path.begin(), path.end());

        cout << "----- Shortest Path (Minimum Energy Route) -----\n";
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); i++) {
            cout << path[i];
            if (i != path.size() - 1) cout << " -> ";
        }
        cout << endl;
        cout << "Total Energy Cost: " << dist[destination] << endl;
        cout << "Algorithm Complexity: O((V + E) log V) using a Min-Heap Priority Queue\n";
    }
};


/* ===================================================================
   MODULE 8: POWER ALLOCATOR
   -------------------------------------------------------------------
   DSA Concept: priority_queue<Fan> (Max-Heap)
   Cooling fans are stored in a MAX-HEAP based on their power demand.
   A max-heap always keeps the element with the LARGEST key at the
   root (top), giving O(1) access to the highest-priority item.
   Insertion and removal (push/pop) both take O(log n) time because
   the heap must be re-balanced ("sift up"/"sift down") to maintain
   the heap property.

   This models real-world power allocation: fans with the HIGHEST
   power demand are served first, ensuring critical cooling zones
   receive power before less urgent ones.
=================================================================== */
class Fan {
public:
    string fanID;
    int powerDemand; // in Watts

    Fan() : fanID(""), powerDemand(0) {}
    Fan(string id, int demand) : fanID(id), powerDemand(demand) {}

    // Comparator: by default std::priority_queue is a MAX-HEAP based
    // on operator<, so the fan with the LARGEST powerDemand will be
    // at the top.
    bool operator<(const Fan &other) const {
        return powerDemand < other.powerDemand;
    }
};

class PowerAllocator {
private:
    priority_queue<Fan> fanQueue; // Max-Heap by powerDemand

public:
    // Push: O(log n)
    void addFanDemand(const string &id, int demand) {
        fanQueue.push(Fan(id, demand));
        cout << "Fan '" << id << "' added with power demand " << demand << " W.\n";
    }

    // Pop: O(log n) - allocate power to highest-demand fan first
    void allocatePower() {
        if (fanQueue.empty()) {
            cout << "No fans in the allocation queue.\n";
            return;
        }
        Fan top = fanQueue.top();
        fanQueue.pop();
        cout << "Allocating power to Fan '" << top.fanID
             << "' (Demand: " << top.powerDemand << " W). Power granted!\n";
    }

    // Display allocation order: O(n log n) since we must pop all
    // elements (each pop is O(log n)) to view heap order, then restore.
    void displayAllocationOrder() const {
        if (fanQueue.empty()) {
            cout << "No fans in the allocation queue.\n";
            return;
        }
        priority_queue<Fan> temp = fanQueue; // copy to preserve original
        cout << "----- Power Allocation Order (Highest Demand First) -----\n";
        int rank = 1;
        while (!temp.empty()) {
            Fan f = temp.top();
            temp.pop();
            cout << rank++ << ". Fan '" << f.fanID
                 << "' -> Demand: " << f.powerDemand << " W\n";
        }
    }

    bool empty() const { return fanQueue.empty(); }
};


/* ===================================================================
   MAIN SYSTEM: CoolCitySystem
   -------------------------------------------------------------------
   Integrates all 8 modules and provides a menu-driven interface.
=================================================================== */
class CoolCitySystem {
private:
    PipeRegister pipeRegister;
    PressureLog pressureLog;
    StartupQueue startupQueue;
    ComponentFinder componentFinder;
    UsageSorter usageSorter;
    UtilityMap utilityMap;

    // Helper to clear bad input from cin
    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // -------------------- MODULE 1 MENU --------------------
    void pipeRegisterMenu() {
        int choice;
        do {
            cout << "\n--- PIPE REGISTER (unordered_map / Hashing) ---\n";
            cout << "1. Add Pipe\n2. Delete Pipe\n3. Update Pipe\n";
            cout << "4. Search Pipe\n5. Display All Pipes\n6. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            int sn; string mat; double dia, len, pr;
            switch (choice) {
                case 1:
                    cout << "Enter Serial Number: "; cin >> sn;
                    cout << "Enter Material: "; cin >> mat;
                    cout << "Enter Diameter (in): "; cin >> dia;
                    cout << "Enter Length (m): "; cin >> len;
                    cout << "Enter Pressure Rating (PSI): "; cin >> pr;
                    pipeRegister.addPipe(sn, mat, dia, len, pr);
                    break;
                case 2:
                    cout << "Enter Serial Number to delete: "; cin >> sn;
                    pipeRegister.deletePipe(sn);
                    break;
                case 3:
                    cout << "Enter Serial Number to update: "; cin >> sn;
                    cout << "Enter new Material: "; cin >> mat;
                    cout << "Enter new Diameter (in): "; cin >> dia;
                    cout << "Enter new Length (m): "; cin >> len;
                    cout << "Enter new Pressure Rating (PSI): "; cin >> pr;
                    pipeRegister.updatePipe(sn, mat, dia, len, pr);
                    break;
                case 4:
                    cout << "Enter Serial Number to search: "; cin >> sn;
                    pipeRegister.searchPipe(sn);
                    break;
                case 5:
                    pipeRegister.displayAll();
                    break;
                case 6:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 6);
    }

    // -------------------- MODULE 2 MENU --------------------
    void pressureLogMenu() {
        int choice;
        do {
            cout << "\n--- PRESSURE LOG (Stack - LIFO) ---\n";
            cout << "1. Record Pressure\n2. View Current Pressure\n";
            cout << "3. Undo Last Pressure Change\n4. Display Pressure History\n";
            cout << "5. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            double value;
            switch (choice) {
                case 1:
                    cout << "Enter pressure value (PSI): "; cin >> value;
                    pressureLog.recordPressure(value);
                    break;
                case 2:
                    pressureLog.viewCurrentPressure();
                    break;
                case 3:
                    pressureLog.undoLastChange();
                    break;
                case 4:
                    pressureLog.displayHistory();
                    break;
                case 5:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 5);
    }

    // -------------------- MODULE 3 MENU --------------------
    void startupQueueMenu() {
        int choice;
        do {
            cout << "\n--- STARTUP QUEUE (Queue - FIFO) ---\n";
            cout << "1. Add Restart Request\n2. Process Next Request\n";
            cout << "3. View Pending Requests\n4. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            string name;
            switch (choice) {
                case 1:
                    cout << "Enter Building Name: "; cin >> name;
                    startupQueue.addRequest(name);
                    break;
                case 2:
                    startupQueue.processNextRequest();
                    break;
                case 3:
                    startupQueue.viewPendingRequests();
                    break;
                case 4:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 4);
    }

    // -------------------- MODULE 4 MENU --------------------
    void componentFinderMenu() {
        int choice;
        do {
            cout << "\n--- COMPONENT FINDER (Vector - Linear/Binary Search) ---\n";
            cout << "1. Add Component\n2. Linear Search\n3. Binary Search\n";
            cout << "4. Display Components\n5. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            int sn; string type;
            switch (choice) {
                case 1:
                    cout << "Enter Serial Number: "; cin >> sn;
                    cout << "Enter Type (Pipe/Valve/Pump): "; cin >> type;
                    componentFinder.addComponent(sn, type);
                    break;
                case 2:
                    cout << "Enter Serial Number to search: "; cin >> sn;
                    componentFinder.linearSearch(sn);
                    break;
                case 3:
                    cout << "Enter Serial Number to search: "; cin >> sn;
                    componentFinder.binarySearch(sn);
                    break;
                case 4:
                    componentFinder.displayComponents();
                    break;
                case 5:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 5);
    }

    // -------------------- MODULE 5 MENU --------------------
    void usageSorterMenu() {
        int choice;
        do {
            cout << "\n--- USAGE SORTER (Vector + std::sort, O(n log n)) ---\n";
            cout << "1. Add Block\n2. Sort Ascending\n3. Sort Descending\n";
            cout << "4. Display Blocks\n5. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            string name; double usage;
            switch (choice) {
                case 1:
                    cout << "Enter Block Name: "; cin >> name;
                    cout << "Enter Cooling Usage (kWh): "; cin >> usage;
                    usageSorter.addBlock(name, usage);
                    break;
                case 2:
                    usageSorter.sortAscending();
                    break;
                case 3:
                    usageSorter.sortDescending();
                    break;
                case 4:
                    usageSorter.displayBlocks();
                    break;
                case 5:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 5);
    }

    // -------------------- MODULE 6 MENU --------------------
    void utilityMapMenu() {
        int choice;
        do {
            cout << "\n--- UTILITY MAP (Graph - Adjacency List) ---\n";
            cout << "1. Add Node\n2. Add Connection\n3. Display Network\n";
            cout << "4. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            string a, b; int cost;
            switch (choice) {
                case 1:
                    cout << "Enter Node Name: "; cin >> a;
                    utilityMap.addNode(a);
                    break;
                case 2:
                    cout << "Enter first Node Name: "; cin >> a;
                    cout << "Enter second Node Name: "; cin >> b;
                    cout << "Enter Energy Cost: "; cin >> cost;
                    utilityMap.addConnection(a, b, cost);
                    break;
                case 3:
                    utilityMap.displayNetwork();
                    break;
                case 4:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 4);
    }

    // -------------------- MODULE 7 MENU --------------------
    void energyOptimizerMenu() {
        EnergyOptimizer optimizer(utilityMap);
        int choice;
        do {
            cout << "\n--- ENERGY OPTIMIZER (Dijkstra's Algorithm) ---\n";
            cout << "1. Find Shortest Path & Total Energy Cost\n";
            cout << "2. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            string src, dest;
            switch (choice) {
                case 1:
                    cout << "Enter Source Node (e.g., Cooling Plant): "; cin >> src;
                    cout << "Enter Destination Node (e.g., Building A): "; cin >> dest;
                    optimizer.findShortestPath(src, dest);
                    break;
                case 2:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 2);
    }

    // -------------------- MODULE 8 MENU --------------------
    void powerAllocatorMenu(PowerAllocator &powerAllocator) {
        int choice;
        do {
            cout << "\n--- POWER ALLOCATOR (Priority Queue - Max Heap) ---\n";
            cout << "1. Add Fan Demand\n2. Allocate Power\n";
            cout << "3. Display Allocation Order\n4. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) { clearInput(); continue; }

            string id; int demand;
            switch (choice) {
                case 1:
                    cout << "Enter Fan ID: "; cin >> id;
                    cout << "Enter Power Demand (W): "; cin >> demand;
                    powerAllocator.addFanDemand(id, demand);
                    break;
                case 2:
                    powerAllocator.allocatePower();
                    break;
                case 3:
                    powerAllocator.displayAllocationOrder();
                    break;
                case 4:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 4);
    }

    // -------------------- DEMO SIMULATION --------------------
    void demoSimulation(PowerAllocator &powerAllocator) {
        cout << "\n=====================================================\n";
        cout << "          RUNNING DEMO SIMULATION\n";
        cout << "=====================================================\n";

        // 1. Pipe Registration
        cout << "\n[1] Pipe Registration (unordered_map - Hashing)\n";
        pipeRegister.addPipe(101, "Steel", 12.5, 250.0, 150.0);
        pipeRegister.addPipe(102, "Copper", 8.0, 180.0, 200.0);
        pipeRegister.addPipe(103, "PVC", 6.0, 120.0, 100.0);
        cout << "Searching for pipe 102 (O(1) average via hashing):\n";
        pipeRegister.searchPipe(102);

        // 2. Pressure rollback
        cout << "\n[2] Pressure Log & Rollback (Stack - LIFO)\n";
        pressureLog.recordPressure(120.0);
        pressureLog.recordPressure(135.0);
        pressureLog.recordPressure(210.0); // dangerous spike
        cout << "A dangerous spike of 210 PSI was detected!\n";
        pressureLog.viewCurrentPressure();
        pressureLog.undoLastChange();

        // 3. Building restart queue
        cout << "\n[3] Startup Queue (Queue - FIFO)\n";
        startupQueue.addRequest("BuildingA");
        startupQueue.addRequest("BuildingB");
        startupQueue.addRequest("BuildingC");
        cout << "Processing restarts in arrival order:\n";
        startupQueue.processNextRequest();
        startupQueue.processNextRequest();

        // 4. Component search
        cout << "\n[4] Component Finder (Vector - Linear/Binary Search)\n";
        componentFinder.addComponent(301, "Valve");
        componentFinder.addComponent(305, "Pump");
        componentFinder.addComponent(302, "Pipe");
        componentFinder.addComponent(310, "Valve");
        cout << "Linear Search for component 302:\n";
        componentFinder.linearSearch(302);
        cout << "Binary Search for component 310:\n";
        componentFinder.binarySearch(310);

        // 5. Usage sorting
        cout << "\n[5] Usage Sorter (Vector + std::sort, O(n log n))\n";
        usageSorter.addBlock("Block-A", 450.5);
        usageSorter.addBlock("Block-B", 120.0);
        usageSorter.addBlock("Block-C", 320.8);
        usageSorter.sortDescending();
        usageSorter.displayBlocks();

        // 6. Graph creation
        cout << "\n[6] Utility Map (Graph - Adjacency List)\n";
        utilityMap.addNode("Cooling Plant");
        utilityMap.addNode("Building A");
        utilityMap.addNode("Building B");
        utilityMap.addNode("Building C");
        utilityMap.addConnection("Cooling Plant", "Building A", 4);
        utilityMap.addConnection("Cooling Plant", "Building B", 8);
        utilityMap.addConnection("Building A", "Building B", 2);
        utilityMap.addConnection("Building A", "Building C", 7);
        utilityMap.addConnection("Building B", "Building C", 3);
        utilityMap.displayNetwork();

        // 7. Dijkstra shortest path
        cout << "\n[7] Energy Optimizer (Dijkstra's Algorithm)\n";
        EnergyOptimizer optimizer(utilityMap);
        cout << "Finding minimum-energy route from Cooling Plant to Building C:\n";
        optimizer.findShortestPath("Cooling Plant", "Building C");

        // 8. Power allocation
        cout << "\n[8] Power Allocator (Priority Queue - Max Heap)\n";
        powerAllocator.addFanDemand("Fan-1", 500);
        powerAllocator.addFanDemand("Fan-2", 1200);
        powerAllocator.addFanDemand("Fan-3", 800);
        powerAllocator.displayAllocationOrder();
        cout << "Allocating power (highest demand first):\n";
        powerAllocator.allocatePower();
        powerAllocator.allocatePower();

        cout << "\n=====================================================\n";
        cout << "          DEMO SIMULATION COMPLETE\n";
        cout << "=====================================================\n";
    }

public:
    // Run the entire menu-driven application
    void run() {
        PowerAllocator powerAllocator; // shared so demo + menu use same data
        int choice;

        cout << "=====================================================\n";
        cout << "   CoolCity: Sub-Street Chilled-Water Utility Simulation\n";
        cout << "=====================================================\n";

        do {
            cout << "\n================= MAIN MENU =================\n";
            cout << "1.  Pipe Registry\n";
            cout << "2.  Pressure Log\n";
            cout << "3.  Startup Queue\n";
            cout << "4.  Component Finder\n";
            cout << "5.  Usage Sorter\n";
            cout << "6.  Utility Map\n";
            cout << "7.  Energy Optimizer\n";
            cout << "8.  Power Allocator\n";
            cout << "9.  Demo Simulation\n";
            cout << "10. Exit\n";
            cout << "==============================================\n";
            cout << "Enter choice: ";

            if (!(cin >> choice)) { clearInput(); continue; }

            switch (choice) {
                case 1: pipeRegisterMenu(); break;
                case 2: pressureLogMenu(); break;
                case 3: startupQueueMenu(); break;
                case 4: componentFinderMenu(); break;
                case 5: usageSorterMenu(); break;
                case 6: utilityMapMenu(); break;
                case 7: energyOptimizerMenu(); break;
                case 8: powerAllocatorMenu(powerAllocator); break;
                case 9: demoSimulation(powerAllocator); break;
                case 10: cout << "Exiting CoolCity Simulation. Goodbye!\n"; break;
                default: cout << "Invalid choice. Please enter 1-10.\n";
            }
        } while (choice != 10);
    }
};


/* ===================================================================
   MAIN FUNCTION
=================================================================== */
int main() {
    CoolCitySystem system;
    system.run();
    return 0;
}
