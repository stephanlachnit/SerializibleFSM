<!--
Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
SPDX-License-Identifier: MIT
-->

# SerializibleFSM

SerializibleFSM is a templated header-only final state machine C++17 library designed to be easily
serializable for network transport. It achieves this by being templated by two enums: one for the
possible states of the FSM, and one for the possible events the FSM may encounter.

## Simple example

```c++
#include <iostream>
#include "SerializableFSM.hpp"

// Possible states
enum class State {
    On,
    Off,
};

// Possible events
enum class Event {
    OnEvent,
    OffEvent,
};

// Optional type forwarding
using EventReactionMap = SerializibleFSM::EventReactionMap<State, Event>;
using StateMap = SerializibleFSM::StateMap<State, Event>;
using FSM = SerializibleFSM::FSM<State, Event>;

// Switch is being switched on
State switchOn() {
    return State::On;
}

// Switch is being switched off
State switchOff() {
    return State::Off;
}

int main() {
    // Definition of state map
    StateMap state_map {
        {State::On, EventReactionMap {
            {Event::OnEvent, switchOn},
            {Event::OffEvent, switchOff},
        }},
        {State::Off, EventReactionMap {
            {Event::OnEvent, switchOn},
            {Event::OffEvent, switchOff},
        }},
    };

    // Define FSM with starting in Off state
    FSM fsm {state_map, State::Off};

    // REPL loop
    while (true) {
        // Print current state with magic_enum
        std::cout << "State: " << magic_enum::enum_name(fsm.GetCurrentState()) << std::endl;

        // User input of new event
        std::string event_name;
        std::cout << "Event: ";
        std::cin >> event_name;

        // Convert string to enum with magic_enum
        auto event = magic_enum::enum_cast<Event>(event_name);

        // React to event if valid user input
        if (event.has_value()) {
            fsm.React(event.value());
        }
    }

    return 0;
}
```

For more examples, take a look at the [examples](examples/) directory.

## Build examples

You can build the examples with [Meson](https://mesonbuild.com/):

```shell
meson setup builddir -Dbuild_examples=true
meson compile -C builddir
```

The examples are then located in the `builddir/` directory.

## License
Licensed under MIT.
