

<!-- Start of picture text -->
et aclence ant THrBRalapy<br>Foundation taroe<br><!-- End of picture text -->

**National University of Computer & Emerging Sciences (NUCES), Islamabad** 

# **Data Structures Lab Fall 2026 (DS, AI) Semester Project** 

## **AI Rescue Robots (Autonomous Disaster-Response Simulation) Group** 

**Size:** The course project will be completed in groups of three. 

## **Project Brief** 

A major urban area has suffered a large-scale disaster. Roads are blocked, infrastructure is damaged, communication is unreliable, and emergency requests are arriving continuously. Your team has been asked to develop a computational rescue-coordination system that manages a fleet of autonomous rescue robots and coordinates their activities inside the affected region. 

The system must represent the disaster environment, maintain information about robots and rescue requests, determine feasible movement, handle competing tasks, respond to changing conditions, and produce a complete and reproducible record of the operation. 

The project intentionally does not prescribe one fixed data structure or algorithm. Selecting, combining, implementing, and justifying appropriate data structures and algorithms is a core part of the assessment. 

## **1. Scenario** 

At the beginning of the simulation, a disaster-management center receives a description of an affected region. The region contains locations such as roads, buildings, hospitals, supply depots, blocked areas, charging stations, command centers, and other relevant facilities. 

A number of autonomous rescue robots are deployed throughout the region. Each robot has an identifier, a current location, capabilities, an operational state, and a resource level such as battery. During the simulation, rescue requests are generated at different locations. Each request contains a unique identifier, location, category, urgency, and report time. The coordinator must decide which available robot should respond while considering route availability, robot capabilities, current workload, and resource constraints. 

The simulation proceeds in discrete time steps. At each time step, requests, robot states, movement, environmental changes, resource consumption, and task outcomes are updated according to the team's documented rules. 

**National University of Computer & Emerging Sciences (NUCES), Islamabad** 

## **2. Problem Statement** 

Design and implement a rescue-coordination simulator in C++ that manages a dynamic disaster environment and a fleet of autonomous rescue robots. The system must process incoming rescue requests, maintain the state of the environment and robots, determine and execute valid actions, resolve competing demands, and record the outcome of the simulation. 

The solution must operate as one coherent computing system. It must not be a collection of independent demonstrations of data structures. 

## **3. Project Objectives** 

- Represent a disaster-affected environment so that locations, connections, and restrictions can be queried efficiently. 

- Manage multiple robots with different states, capabilities, locations, and resource constraints. • 

Accept and maintain rescue requests that may arrive throughout the simulation. • Determine valid routes between locations while respecting unavailable or blocked connections. • Coordinate robots when multiple robots compete for a request or one robot can serve multiple  requests. 

- Support at least one meaningful environmental change that affects later decisions. • Maintain a deterministic, chronological simulation process. 

- Generate evidence that demonstrates correctness, scalability, and efficiency. 

## **4. Functional Requirements** 

### **4.1 Environment** 

- The environment shall contain a finite set of locations connected by routes/connections. • Each connection shall have a defined travel cost, distance, or equivalent measure used  consistently by the simulation. 

- Connections may be directed or undirected; the team's choice must be documented. • The 

environment must contain at least one restricted or unavailable area/connection. • A blocked connection shall become unavailable for traversal until the defined condition for  restoration is met. 

- Different environments must be loadable through external input without changing the source code. 

### **4.2 Rescue Requests** 

- Each request shall contain at minimum: unique ID, location, category/type, urgency, and report time. 

- Requests may appear at different simulation time steps. 

- Each request shall have a clearly defined lifecycle, such as pending, assigned, in-progress, completed, failed, or an equivalent design. 

- **National University of Computer & Emerging Sciences (NUCES), Islamabad** 

- A request may be assigned to only one robot at a time. 

- The system must prevent the same request from being incorrectly completed by multiple robots. 

### **4.3 Robots** 

- Each robot shall have a unique ID and current location. 

- Each robot shall maintain an operational state, such as Available, Travelling, Performing, Returning, Recharging, or Unavailable. 

- Robots may have different capabilities and/or resource limits. 

- Each robot shall have a resource level such as battery. 

- Movement and/or rescue operations shall consume resources according to a documented rule. • A robot must not begin or perform an operation when doing so violates its defined capability,  route, state, or resource constraints. 

### **4.4 Movement and Route Selection** 

- A robot assigned to a request must determine a valid route from its current location to the request location. 

- The route must respect blocked or unavailable connections. 

- The team must document the criterion used to select a route, such as minimum distance, minimum cost, or another justified criterion. 

- If no valid route exists, the request must remain unresolved, be reassigned, or be marked failed according to a documented rule. 

- If a route becomes blocked while a robot is travelling, the system must detect the change and apply a documented rerouting or failure policy. 

### **4.5 Simulation Time** 

- The simulation shall proceed in discrete time steps. 

- The team must define what one time step represents. 

- At every time step, relevant events shall be processed, robot/request states updated, and 

resulting changes recorded. 

- The order in which simultaneous events are processed must be explicitly documented and 

   - consistently applied. 

- The simulation must terminate according to a documented termination condition. 

### **4.6 Coordination and Conflicts** 

- The system must handle cases where multiple robots are candidates for the same request. • The 

system must handle cases where one robot could serve multiple requests. • When several requests compete for limited robots/resources, the system must apply a  deterministic priority/selection rule. 

- The priority rule may use attributes such as urgency, waiting time, distance, request type, or another justified factor. 

- **National University of Computer & Emerging Sciences (NUCES), Islamabad** 

- For identical input and initial conditions, the same decision must be produced every time. • The system must prevent invalid state transitions and inconsistent assignments. 

### **4.7 Dynamic Conditions** 

- At least one environmental or operational condition must change during the simulation. • The 

change must affect at least one subsequent robot decision, route, request assignment, or  resource decision. 

- Examples include a road becoming blocked, a charging station becoming unavailable, a request priority changing, or a new operational restriction appearing. 

- The internal state of the system must be updated correctly after the change. 

## **5. Scale and Performance Requirements** 

The project is intended to be a data-structures problem rather than a small CRUD-style application. Therefore, the implementation must handle both a manageable complete scenario and a larger stress <u>scenario.</u> 

|**Component**|**Minimum Working**<br>**Scenario**|**Required Stress Scenario**|
|---|---|---|
|Locations|50+|100+|
|Connections|100+|200+|
|Robots|10+|20+|
|Rescue requests|50+|100+|



Simulation duration 

200+ time steps 

500+ time steps 

The minimum working scenario must demonstrate the complete functionality of the system. The stress scenario must be used to demonstrate scalability and performance. Students must design the  program so that increasing the input size does not require rewriting the source code. For stress testing, the larger scenario should contain at least twice the corresponding size of the minimum working scenario. Teams may use larger inputs where practical. 

## **6. Required System Capabilities** 

- Load a complete scenario from external input. 

- Validate input and report invalid or inconsistent data safely. 

- Display or report the current state of the environment when required. 

- Display or report robot states when required. 

- Display or report pending, assigned, completed, and failed requests. 

- Process the simulation one time step at a time. 

- Run the complete simulation automatically. 

- Record assignments, movements, failures, completions, resource changes, and environmental changes. 

- Produce final statistics describing the simulation outcome. **National University of Computer & Emerging Sciences (NUCES), Islamabad** 

## **7. Input Specification** 

Scenario information must be provided through external files. A reference scenario file, scenario_provided.txt, is supplied with this document (see Appendix A) as a guideline for the expected input and as a common scenario on which submissions may be evaluated. It satisfies the size requirements of the minimum working scenario in Section 5. Every submission must load and run this file without any change to the file or to the source code. Groups may extend the format for their own additional scenarios (including the required stress scenario), but any extension must remain compatible with the provided file and must be clearly documented in the README. 

- Environment locations and connections. 

- Connection costs/distances and initial availability. 

- Initial robot states, locations, capabilities, and resources. 

- Initial rescue requests, if any. 

- Events that occur during the simulation. 

- Simulation parameters and termination conditions. 

## **8. Output and Evidence Requirements** 

The system must generate a machine-readable or clearly structured simulation record. The record must allow an evaluator to reconstruct the major events of the simulation. 

- Simulation time step. 

- Robot state changes. 

- Request state changes. 

- Assignments and task completions. 

- Movement and route-related events. 

- Failures or rejected actions. 

- Environmental changes. 

- Final simulation statistics. 

Recommended final metrics include: 

- Number of requests completed, failed, and remaining. 

- Average and maximum response time. 

- Robot utilization. 

- Distance or movement cost. 

- Resource/battery consumption. 

- Number of conflicts and reassignment events. 

- Simulation execution time. 

## **9. Data Structure and Algorithmic Design** 

No fixed list of data structures is prescribed. Teams are expected to analyze the required operations and select suitable data structures and algorithms. 

The final report must explain: 

**National University of Computer & Emerging Sciences (NUCES), Islamabad** 

- The data representation selected for each major component. 

- Why each representation is appropriate for the required operations. 

- The important operations supported by each structure. 

- Expected time and space complexity. 

- At least two important alternative design choices considered and why the selected approach was preferred. 

- How the design behaves as input size increases. 

## **10. Implementation Constraints** 

- The project must be implemented in C++. 

- Core data structures central to the solution must be implemented by the students. • STL containers are not allowed. 

- External libraries for machine learning, external pathfinding, scheduling, or simulation are not permitted unless explicitly approved by the instructor. 

- The program must be modular and separated into logical components. • 

Dynamic memory must be handled safely. 

- The program must not depend on manual modification of source code for individual test cases. 

- Every group member must understand and be able to explain the submitted implementation during the demo. 

## **11. Project Submissions** 

**Submission 1: Design and Foundation Deadline: 26**<sup>**th**</sup> **October 2026** Purpose: establish a sound design before the complete system is developed. • Design report covering problem understanding and proposed architecture. • Selected data structures and initial justification. 

- Planned algorithms and expected complexity. 

- Input/scenario format specification. 

- Initial C++ implementation of the environment, robots, rescue requests, and basic simulation framework. 

- All working scenarios (at least 5 input files) demonstrating the foundation. Submission 1 is a progress milestone. The complete rescue-coordination system is not required at  this stage. 

**Submission 2: Final Project Deadline: 26**<sup>**th**</sup> **November 2026** Purpose: submit and defend the complete working system. 

- Complete, compliable C++ source code. **National University of Computer & Emerging Sciences (NUCES), Islamabad** 

- Complete project report containing design, implementation, algorithms, complexity, testing, performance, results, and limitations. 

- README containing compilation, execution, input format, output format, and usage instructions. 

- At least three scenarios of increasing difficulty. 

- Minimum working scenario and required stress scenario. 

- Simulation output (record and final statistics) produced by running the provided scenario file scenario_provided.txt. 

- Performance analysis using multiple input sizes. 

## **12. Final Project Report Structure** 

- Introduction and Problem Overview 

- Objectives and Scope 

- Data Structures and Data Representation 

- Algorithms and Route/Coordination Strategy 

- Design Decisions and Alternatives 

- Complexity Analysis 

- Implementation Details 

- Performance and Scalability Analysis 

- Conclusion 

## **13. Recommended Project Organization** 

The following organization is recommended, not mandatory: project/ 

- ├── src/ 

- │├── main.cpp 

- │├── environment.* 

│├── robot.* │├── rescue_request.* │├── simulation.* │└── other modules as required ├── data/ │├── scenario_provided.txt (provided) │├── scenario_1.* │├── scenario_2.* │└── scenario_3.* ├── tests/ ├── output/ ├── README.md └── report.pdf 

**National University of Computer & Emerging Sciences (NUCES), Islamabad** 

## **Appendix A: Provided Input File (scenario_provided.txt)** 

The file scenario_provided.txt describes a complete minimum working scenario: 56 locations, 117  connections (7 initially blocked), 12 robots, 60 rescue requests, 20 dynamic events, and a limit of  250 time steps. It is a plain text file; every group must be able to load it as it is. **File format** 

- Lines beginning with # and blank lines are comments and must be ignored. • Each section 

begins with a header line of the form KEYWORD count, followed by exactly  count records, one per line. A mismatch between the count and the records present must be  reported as invalid input. The file ends with the line END. 

- PARAM key value: simulation constants (max_steps, speed, battery_per_cost_unit, battery_per_work_step, recharge_per_step). Groups define the exact rules in which these are used and document them. 

- LOCATIONS: id name type, where type is JUNCTION, BUILDING, HOSPITAL, DEPOT, CHARGING, or COMMAND. 

- CONNECTIONS: id location_a location_b cost status, where status is OPEN or BLOCKED. Connections are undirected. 

- ROBOTS: id start_location capability max_battery initial_battery, where capability is MEDICAL, FIRE, SUPPLY, SEARCH, or GENERAL (a GENERAL robot can serve any category). 

- REQUESTS: id report_time location category urgency duration_steps, where category is 

MEDICAL, FIRE, SUPPLY, or SEARCH, and urgency is 1 (low) to 5 (critical). • EVENTS: time followed by one of BLOCK or UNBLOCK (connection id),  STATION_DOWN or STATION_UP (location id), SET_URGENCY (request id, new  urgency), ROBOT_OFFLINE or ROBOT_ONLINE (robot id). 

- Records that occur at the same time step are processed in the order in which they appear in the 

file unless the group documents a different consistent rule. 

### **Situations included in the scenario** 

• Several requests are reported at the same time step, so tie-breaking rules are exercised. • One location is cut off at the start because all of its connections are blocked; a request there  must wait or be handled according to the group's documented rule until a connection is  restored. 

- Roads are blocked and restored during the simulation, including while robots may be travelling. 

- Charging stations become unavailable and later return, and robots go offline and come back online. 

- One request has its urgency raised during the simulation. 

### **Excerpt** 

SCENARIO minimum_working_1 PARAM max_steps 250 

LOCATIONS 56 

**National University of Computer & Emerging Sciences (NUCES), Islamabad** 

1 Junction_1 JUNCTION 2 Supply_Depot_1 DEPOT 3 Building_1 BUILDING ... 

CONNECTIONS 117 1 1 2 8 OPEN 2 1 8 9 OPEN 3 2 3 8 OPEN ... 

ROBOTS 12 

1 13 MEDICAL 100 100 2 29 MEDICAL 100 100 ... 

REQUESTS 60 1 0 17 MEDICAL 1 4 2 0 31 SEARCH 5 7 3 0 23 SUPPLY 2 7 ... 

EVENTS 20 20 BLOCK 22 35 BLOCK 3 45 STATION_DOWN 19 

END 

Groups are encouraged to create additional scenarios in the same format, including the larger stress  scenario required in Section 5. 

