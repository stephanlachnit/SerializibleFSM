// Copyright © 2024 Stephan Lachnit <stephanlachnit@debian.org>
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <iostream>

#include <magic_enum.hpp>
#include <SerializableFSM.hpp>

enum class State : std::uint8_t {
    Level0 = 0,
    Level1 = 1,
    Level2 = 2,
};

enum class Event {
    SwitchDown,
    SwitchUp,
};

class Switch;
using BaseFSM = SerializableFSM::FSM<Switch, State, Event>;

class Switch : public BaseFSM {
private:
    friend BaseFSM;

    // Go to higher level
    auto switchUp(std::any /* user_data */) -> State {
        if (getState() == State::Level0) {
            return State::Level1;
        }
        return State::Level2;
    }

    // Go to lower level
    auto switchDown(std::any /* user_data */) -> State {
        if (getState() == State::Level2) {
            return State::Level1;
        }
        return State::Level0;
    }

public:
    explicit Switch(State initial_state) : BaseFSM(this, initial_state, BaseFSM::StateTransitionMap({
        // Level0: only allow to switch up
        {State::Level0, {{Event::SwitchUp, &Switch::switchUp}}},
        // Level1: can go both up and down
        {State::Level1, {{Event::SwitchDown, &Switch::switchDown},
                         {Event::SwitchUp, &Switch::switchUp}}},
         // Level2: can only go down
        {State::Level2, {{Event::SwitchDown, &Switch::switchDown}}},
        })) {}
};

int main() {
    // Define FSM with starting in Off state
    Switch switch_fsm {State::Level0};

    std::cout << "Possible events: SwitchUp, SwitchDown" << std::endl;

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
