#include "../include/scenario_parser.hpp"
#include <iostream>

bool ScenarioParser::parse(const std::string& file_path, 
                           Environment& /*env*/, 
                           DynamicArray<Robot>& /*robots*/, 
                           DynamicArray<RescueRequest>& /*requests*/, 
                           DynamicArray<Event>& /*events*/, 
                           SimulationSettings& /*settings*/) {
    // TODO: Pending actual scenario format. 
    // Must return false until parsing logic is properly implemented.
    std::cerr << "Parser not fully implemented. Missing scenario file format for: " << file_path << "\n";
    return false;
}
