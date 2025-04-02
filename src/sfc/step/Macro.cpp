#include "sfc/step/Macro.hpp"

#include <algorithm>
#include <stdexcept>

Macro::Macro(unsigned int step_id, std::vector<std::shared_ptr<StepAction>> m_actions) : Step(step_id, MACRO_STEP, m_actions) {}

void Macro::addStep(std::shared_ptr<Step> step) {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  if (step && m_macro_steps.empty()) {
    m_first = step;
  }
  if (step) {
    m_macro_steps[step->getStepId()] = step;
    m_last = step;
  } else {
    throw std::invalid_argument("Trying to add a nullptr Step !");
  }
}

bool Macro::containsStep(unsigned int id) const {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  return m_macro_steps.count(id);
}

std::shared_ptr<Step> Macro::first() { return m_first; }

std::shared_ptr<Step> Macro::last() { return m_last; }

const std::unordered_map<unsigned int, std::shared_ptr<Step>> &Macro::steps() const { return m_macro_steps; }

bool Macro::isActivated() const {
  return std::any_of(m_macro_steps.begin(), m_macro_steps.end(), [](auto s) { return s.second->isActivated(); });
}

void Macro::addTransition(std::shared_ptr<Transition> t) {
  Step::addTransition(t);
  m_last->addTransition(t);
}
