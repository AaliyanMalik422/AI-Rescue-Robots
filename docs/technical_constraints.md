# Technical Constraints and Approval Register

## Approved (team decision)
- C++17, GCC. Headers: `<iostream>`, `<fstream>`, `<sstream>`, `<string>`, `<cmath>`.
- Studied and allowed: classes, basic templates, pointers, `new`/`delete[]`,
  constructors/destructors, copy constructor / copy assignment, operator overloading
  (instructor-confirmed), references, `const`, `#define` for the small test macros.
- Named integer constants instead of enums.

## Not allowed
- Any STL container (`vector`, `list`, `queue`, `stack`, `deque`, `map`, `set`,
  `priority_queue`, ...), `<algorithm>`, `std::sort`, smart pointers, lambdas, `auto`,
  `std::move`/move semantics, namespaces for helpers, `static` class members, `explicit`,
  `<climits>`, `<utility>`, third-party libraries.
- External ML, pathfinding, scheduling or simulation libraries (instructor approval needed).

## Audit of this skeleton (Nov 14)
- Only `iostream`, `fstream`, `sstream`, and `string` are included anywhere.
- Removed from the earlier starter: `<climits>`, `<utility>`, `std::move` tests,
  anonymous namespaces, `explicit`, `static` Logger/Pathfinder/Parser members.

## Still conditional (ask the instructor)
- `<ctime>` / `<chrono>`: not needed; performance is shown with work counters.
- Any other standard header.
