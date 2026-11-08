#include <iostream>

extern int run_collections_tests();
extern int run_environment_tests();
extern int run_entities_tests();
extern int run_pathfinding_tests();
extern int run_parser_tests();
extern int run_simulation_tests();

int main() {
    std::cout << "=== Running AI Rescue Robots Tests ===" << std::endl;
    int failures = 0;

    failures += run_collections_tests();
    failures += run_environment_tests();
    failures += run_entities_tests();
    failures += run_pathfinding_tests();
    failures += run_parser_tests();
    failures += run_simulation_tests();

    if (failures > 0) {
        std::cerr << "=== " << failures << " TEST SUITE(S) FAILED ===" << std::endl;
        return 1;
    }

    std::cout << "=== All implemented tests passed; pending features are identified above ===" << std::endl;
    return 0;
}
