<!--
Copyright © 2024 Stephan Lachnit <stephanlachnit@debian.org>
SPDX-License-Identifier: MIT
-->

# SerializableFSM

SerializableFSM is a templated header-only final state machine C++20 library designed to be easily
serializable for network transport. It achieves this by being templated by two enums: one for the
possible states of the FSM, and one for the possible events the FSM may encounter.

## Simple example

```c++
#include <cassert>
#include <SerializableFSM.hpp>

// Possible switch states
enum class State {
    On,
    Off,
};

// Possible switch events
enum class Event {
    SwitchOn,
    SwitchOff,
};

// Typedefs for prettier code
class Switch;
using BaseFSM = SerializableFSM::FSM<Switch, State, Event>;

class Switch : public BaseFSM {
private:
    friend BaseFSM;

    // Transition to turn switch on
    auto switchOn(std::any /* user_data */) -> State {
        return State::On;
    }

    // Transition to turn switch off
    auto switchOff(std::any /* user_data */) -> State {
        return State::Off;
    }

public:
    explicit Switch(State initial_state) : BaseFSM(this, initial_state, BaseFSM::StateTransitionMap({
        // All transitions from the On state go here
        {State::On, {{Event::SwitchOff, &Switch::switchOff}}},
        // All transitions from the Off state go here
        {State::Off, {{Event::SwitchOn, &Switch::switchOn}}},
    })) {}
};

int main() {
    // Define FSM with starting in Off state
    Switch fsm {State::Off};

    // Turn switch on
    fsm.react(Event::SwitchOn);

    // Switch is now in On state
    assert(fsm.getState() == State::On);

    return 0;
}
```

For more examples, take a look at the [examples](examples/) directory.

## Build examples

You can build the examples with [Meson](https://mesonbuild.com/):

```shell
meson setup build -Dexamples=true
meson compile -C build
```

The examples are then located in `build/example`.

## License

Licensed under MIT.
