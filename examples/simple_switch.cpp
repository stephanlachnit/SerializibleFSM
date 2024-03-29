// Copyright © 2024 Stephan Lachnit <stephanlachnit@debian.org>
// SPDX-License-Identifier: MIT

#include <iostream>

#include <magic_enum.hpp>
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

// Optional typedef for prettier code
class Switch;
using BaseFSM = SerializableFSM::FSM<Switch, State, Event>;

class Switch : public BaseFSM {
private:
    // Allow base class access to private methods
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
    // Constructor: we need to pass the "this" pointer so that the base FSM can access the transition function
    explicit Switch(State initial_state) : BaseFSM(this, initial_state, BaseFSM::StateTransitionMap({
        // All transitions from the On state go here
        {State::On, {{Event::SwitchOff, &Switch::switchOff}}},
        // All transitions from the Off state go here
        {State::Off, {{Event::SwitchOn, &Switch::switchOn}}},
    })) {}
};

int main() {
    // Define FSM with starting in Off state
    Switch switch_fsm {State::Off};

    std::cout << "Possible events: SwitchOn, SwitchOff" << std::endl;

    // REPL loop
    while (true) {
        // Print current state with magic_enum
        std::cout << "State: " << magic_enum::enum_name(switch_fsm.getState()) << std::endl;

        // User input of new event
        std::string event_name;
        std::cout << "Event: ";
        std::cin >> event_name;

        // Convert string to enum with magic_enum
        auto event = magic_enum::enum_cast<Event>(event_name);

        // React to event if valid user input
        if (event.has_value()) {
            try {
                switch_fsm.react(event.value());
            }
            catch(const SerializableFSM::Exception& error) {
                std::cerr << "Error: " << error.what() << std::endl;
            }
        }
        else {
            std::cerr << "Error: " << event_name << " is not a valid event name" << std::endl;
        }
    }

    return 0;
}
