#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <set>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Airport
{
    string airportName;
    long double latitude;
    long double longitude;
    string municipality;
    string state;
    int sizeAirport;                      // 1 large, 2 medium
    vector<pair<long, Airport *>> nearby; // small and medium airports here
};

class Graph
{
private:
    map<string, pair<Airport *, vector<Airport *>>> graph;

public: // functions
    void Insert(string airportName, long double latitude, long double longitude, string municipality, string state, int sizeAirport);
    // inserts a large airport into the graph. They map key is the name of the large airport.
    // creates an airport object. then pushes it to insertEdge to be put in the graph correctly.
    void InsertEdge(Airport *obj);                      // inserts an edge between two large airports based on distance
    void bfs(Airport *, string userAirport);            // breadth first search
    void dfs(Airport *src, string userAirport);         // depth first search
    long double distance(Airport *obj1, Airport *obj2); // calculates the distance between two airports based on lat/long
    long double toRadians(const long double degree);    // helper function used in distance function
    vector<string> nameLoop(string stateName);          // helper function to transfer names from airport object to vector of strings
    Airport *returnSource(string airportName);          // getter to retrieve the set source for the search functions
};

void Graph::Insert(string airportName, long double latitude, long double longitude, string municipality, string state, int sizeAirport)
{

    // creates Airport object
    Airport *airport = new Airport;
    airport->airportName = airportName;
    airport->latitude = latitude;
    airport->longitude = longitude;
    airport->municipality = municipality;
    airport->state = state;
    airport->sizeAirport = sizeAirport;

    // if its a large airport put it in the graph
    if (airport->sizeAirport == 1)
    {                                       //it's large
        graph[airportName].first = airport; // initializes the graph key and it's first value
        // call insert edge to check if it's a certain distance from another large airport
        InsertEdge(airport);
    }
    else
    { // its a small or medium airport so loop through and see which large airports are a certain distance away
        // push the small or medium airports to the vector of the large airport.
        for (auto &it : graph)
        {
            long double dist = distance(it.second.first, airport);
            if (dist <= 60)
            { //inserts small & medium airports into small airports vector
                it.second.first->nearby.push_back(make_pair(dist, airport));
            }
        }
    }
}

void Graph::InsertEdge(Airport *obj)
{
    for (auto &it : graph)
    {
        //checks distance from other large airports && != 0 (can't be added to itself)
        if ((distance(it.second.first, obj) <= 75) && (distance(it.second.first, obj) != 0))
        {
            it.second.second.push_back(obj);
        }
    }
}

long double Graph::toRadians(const long double degree)
{
    long double oneDeg = (M_PI) / 180;
    return (oneDeg * degree);
}

long double Graph::distance(Airport *obj1, Airport *obj2)
{
    long double R = 6371; // For KM
    long double lat1 = toRadians(obj1->latitude);
    long double long1 = toRadians(obj1->longitude);
    long double lat2 = toRadians(obj2->latitude);
    long double long2 = toRadians(obj2->longitude);

    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);
    ans = 2 * asin(sqrt(ans));
    ans *= R;

    return ans;
}

void Graph::dfs(Airport *src, string userAirport)
{
    int count = 1;
    set<Airport *> visitedVertices;
    stack<Airport *> graphStack;

    visitedVertices.insert(src);
    graphStack.push(src);

    while (!graphStack.empty())
    {
        Airport *u = graphStack.top();
        if (u->airportName.compare(userAirport) == 0)
        {
            // Print the medium and small airport stuff
            sort(u->nearby.begin(), u->nearby.end());
            cout << "Top 5 closest regional airports: " << endl;
            for (int i = 1; i <= 5; i++)
            {
                cout << count << ". " << u->nearby[i].second->airportName << " (" << u->nearby[i].first << "km)" << endl;
                count++;
            }
            break;
        }
        graphStack.pop();

        for (auto iter = graph.begin(); iter != graph.end(); iter++)
        {
            if (visitedVertices.count(iter->second.first) == false)
            {
                visitedVertices.insert(iter->second.first);
                graphStack.push(iter->second.first);
            }
        }
    }
}

void Graph::bfs(Airport *src, string userAirport)
{
    int count = 1;
    set<Airport *> visitedVertices;
    queue<Airport *> graphQueue;

    visitedVertices.insert(src);
    graphQueue.push(src);
    visitedVertices.count(src);
    while (!graphQueue.empty())
    {
        Airport *u = graphQueue.front();
        if (u->airportName.compare(userAirport) == 0)
        {
            // print the medium and small airport stuff
            sort(u->nearby.begin(), u->nearby.end());
            cout << "Top 5 closest regional airports: " << endl;
            for (int i = 1; i <= 5; i++)
            {
                cout << count << ". " << u->nearby[i].second->airportName << " (" << u->nearby[i].first << "km)" << endl;
                count++;
            }
            break;
        }
        graphQueue.pop();

        for (auto iter = graph.begin(); iter != graph.end(); iter++)
        {
            if (visitedVertices.count(iter->second.first) == false)
            {
                visitedVertices.insert(iter->second.first);
                graphQueue.push(iter->second.first);
            }
        }
    }
}

vector<string> Graph::nameLoop(string stateName)
{
    vector<string> largeAirport;
    for (auto iter = graph.begin(); iter != graph.end(); iter++)
    {
        if (iter->second.first->state == stateName)
        {
            largeAirport.push_back(iter->second.first->airportName);
        }
    }

    return largeAirport;
}
Airport *Graph::returnSource(string airportName)
{ //Getter
    return graph[airportName].first;
}

int main()
{
    // read in the csv as a text file with commas separating each value
    ifstream file("./airports.txt"); //relative path to the txt file in cmake folder
    string line;
    string airportSize;
    string airportName;
    string lat;
    string longit;
    string municipality;
    string state;
    long double latitude;
    long double longitude;
    int sizeAirport;
    char delimiter = ','; // delimiter to read the txt file
    // create graph object
    Graph graph;
    while (getline(file, line))
    {
        // now break up the string separate by comma
        string::size_type pos = line.find(delimiter);
        airportSize = line.substr(0, pos);
        line.erase(0, pos + 1); // erases up to after the first comma

        pos = line.find(delimiter);
        airportName = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(delimiter);
        lat = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(delimiter);
        longit = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(delimiter);
        state = line.substr(0, pos);
        line.erase(0, pos + 1);

        municipality = line; // the rest is the municipality

        char *pEnd;
        latitude = strtold(lat.c_str(), &pEnd); // converts string to long double
        longitude = strtold(longit.c_str(), &pEnd);
        if (airportSize == "large_airport")
        {
            sizeAirport = 1;
        }
        else
        {
            sizeAirport = 2; // small or medium
        }

        graph.Insert(airportName, latitude, longitude, municipality, state, sizeAirport);
    }

    Airport *airport, *src;
    string startAirport = "Birmingham-Shuttlesworth International Airport";
    src = graph.returnSource(startAirport);
    string stateName, userAirport, userSearch;
    vector<string> largeAirport;
    int numLarge;
    int count = 1;

    cout << "Enter state initials (i.e. 'FL'): " << endl;
    getline(cin, stateName);
    cout << endl;

    cout << "Enter name for desired large airport as provided: " << endl;
    largeAirport = graph.nameLoop(stateName);

    for (int i = 0; i < largeAirport.size(); i++)
    {
        cout << count << ". " << largeAirport[i] << endl;
        count++;
    }
    getline(cin, userAirport);

    cout << endl;
    cout << "Enter name for desired search method as provided: " << endl;
    cout << "1. Breadth First Search" << endl;
    cout << "2. Depth First Search" << endl;
    getline(cin, userSearch);
    cout << endl;

    if (userSearch.compare("Breadth First Search") == 0)
    {
        auto start = high_resolution_clock::now();
        graph.bfs(src, userAirport);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Breadth First Search took " << duration.count() << " milliseconds" << endl;
    }
    else if (userSearch.compare("Depth First Search") == 0)
    {
        auto start = high_resolution_clock::now();
        graph.dfs(src, userAirport);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Depth First Search took " << duration.count() << " milliseconds" << endl;
    }

    return 0;
}