// Copyright © 2024 Stephan Lachnit <stephanlachnit@debian.org>
// SPDX-License-Identifier: MIT

#pragma once

#include <any>
#include <exception>
#include <format>
#include <map>
#include <string>
#include <type_traits>

#include <magic_enum.hpp>

namespace SerializableFSM {
    /** Base exception for all exceptions in this library */
    class Exception : public std::exception {
    public:
        [[nodiscard]] const char* what() const noexcept override { return error_message_.c_str(); }
    protected:
        Exception() = default;
        std::string error_message_;
    };

    template<class Class, typename State, typename Event>
    requires std::is_enum_v<State> && std::is_enum_v<Event>
    class FSM {
    public:
        /** Function pointer for a transition function: takes any object, returns new State */
        using TransitionFunction = State(Class::*)(std::any);

        /** Maps event enums to a transition function */
        using TransitionMap = std::map<Event, TransitionFunction>;

        /** Maps state to transition maps for that state */
        using StateTransitionMap = std::map<State, TransitionMap>;

        /** Exception if a state has no transitions at all */
        class StateHasNoTransitionsError : public Exception {
        public:
            explicit StateHasNoTransitionsError(State state) {
                error_message_ = std::format("State {} has no transitions", magic_enum::enum_name(state));
            }
        };

        /** Exception if a state has no transition for given event */
        class StateHasNoTransitionForEventError : public Exception {
        public:
            explicit StateHasNoTransitionForEventError(State state, Event event) {
                error_message_ = std::format("State {} has no transition for event {}",
                                             magic_enum::enum_name(state), magic_enum::enum_name(event));
            }
        };

    public:
        FSM(Class* fsm_class, State initial_state, StateTransitionMap state_transition_map)
            : fsm_class_(fsm_class), state_(initial_state), initial_state_(initial_state),
              state_transition_map_(std::move(state_transition_map)) {}

        [[nodiscard]] constexpr State getState() const { return state_; }

        constexpr void reset() { state_ = initial_state_; }

        void react(Event event, std::any user_data = {}) {
            // Check that current state has transitions
            const auto transition_map_it = state_transition_map_.find(state_);
            if(transition_map_it != state_transition_map_.end()) [[likely]] {
                // Check that transition is allowed
                const auto transition_map = transition_map_it->second;
                const auto transition_function_it = transition_map.find(event);
                if(transition_function_it != transition_map.end()) [[likely]] {
                    // Execute transition function
                    const auto transition_function = transition_function_it->second;
                    state_ = (fsm_class_->*transition_function)(std::move(user_data));
                } else {
                    throw StateHasNoTransitionForEventError(state_, event);
                }
            } else {
                throw StateHasNoTransitionsError(state_);
            }
        }

    private:
        Class* fsm_class_;
        State state_;
        const State initial_state_;
        const StateTransitionMap state_transition_map_;
    };

}
