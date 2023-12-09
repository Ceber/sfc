#include "sfc/step/action/StepAction.hpp"
#include <stdexcept>

StepAction::StepAction(StepActionCallback action_callback) : m_action_callback(action_callback) {}

void StepAction::exec() const {
  if (m_action_callback) {
    m_action_callback();
  } else {
    throw std::runtime_error("StepAction callback is undefined !");
  }
}

void StepAction::operator()() const { exec(); }