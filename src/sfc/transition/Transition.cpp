#include "sfc/transition/Transition.hpp"
#include "sfc/step/Step.hpp"

#include <algorithm>
#include <iostream>

Transition::Transition(std::vector<std::weak_ptr<Step>> next_steps, std::vector<std::weak_ptr<Step>> validation_steps)
    : m_next_steps(next_steps), m_validation_steps(validation_steps), m_receptivity_state(false) {}

const std::vector<std::weak_ptr<Step>> &Transition::nexts() const { return m_next_steps; }

const std::vector<std::weak_ptr<Step>> &Transition::validations() const { return m_validation_steps; }

bool Transition::getReceptivityState() const { return m_receptivity_state.load(); }

void Transition::setReceptivityState(bool state) { m_receptivity_state = state; }
