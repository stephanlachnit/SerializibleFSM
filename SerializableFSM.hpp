#pragma once

#include <map>
#include <string>
#include "magic_enum.hpp"

namespace SerializibleFSM {

    // Reaction to certain event, returns State, takes any amount of args
    template<typename State, typename... Args>
    using EventReaction = State(Args...);

    // Maps event to reaction function
    template<typename State, typename Event, typename... Args>
    using EventReactionMap = std::map<Event, EventReaction<State, Args...>*>;

    // Maps states to event reaction maps
    template<typename State, typename Event, typename... Args>
    using StateMap = std::map<State, EventReactionMap<State, Event, Args...>>;

    // Generic error class thrown when encountering an error during React()
    class ReactionError : public std::exception {
    public:
        const char* what() const noexcept override { return error_message_.c_str(); }
    protected:
        ReactionError() = default;
        std::string error_message_;
    };

    // Error thrown when state not in map
    template<typename State>
    class StateHasNoReactionsError : public ReactionError {
    public:
        explicit StateHasNoReactionsError(State state) {
            error_message_ = "State ";
            error_message_ += magic_enum::enum_name(state);
            error_message_ += " has no reactions";
        }
    };

    // Error thrown when event not in map
    template<typename State, typename Event>
    class StateHasNoReactionToEventError : public ReactionError {
    public:
        explicit StateHasNoReactionToEventError(State state, Event event) {
            error_message_ = "State ";
            error_message_ += magic_enum::enum_name(state);
            error_message_ += " has no reaction to event ";
            error_message_ += magic_enum::enum_name(event);
        }
    };

    // FSM class taking state and event enums
    template<typename State, typename Event, typename... Args>
    class FSM {
    public:
        FSM(StateMap<State, Event, Args...> state_map, State initial_state)
         :  state_map_(std::move(state_map)),
            current_state_(initial_state),
            initial_state_(initial_state)
        {};

        // Reacts to event as given in state_map
        void React(Event event, Args... args) {
            if (state_map_.find(current_state_) != state_map_.end()) {
                if (state_map_[current_state_].find(event) != state_map_[current_state_].end()) {
                    Transit(state_map_[current_state_][event](args...));
                } else {
                    throw StateHasNoReactionToEventError(current_state_, event);
                }
            } else {
                throw StateHasNoReactionsError(current_state_);
            }
        };

        // Returns the current state
        inline State GetCurrentState() const {
            return current_state_;
        };

        // Resets to initial state
        inline void Reset() {
            Transit(initial_state_);
        };

    private:
        // Switched the current state to new state (no reaction)
        inline void Transit(State new_state) {
            current_state_ = new_state;
        };

        // State Map of the FSM
        StateMap<State, Event> state_map_;

        // Current State of the FSM
        State current_state_;

        // Initial State of the FSM used in Reset()
        State initial_state_;
    };

}
