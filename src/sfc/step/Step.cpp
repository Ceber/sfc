#include "sfc/step/Step.hpp"
#include "sfc/step/action/StepAction.hpp"

#include <iostream>

Step::Step(unsigned int step_id, StepType step_type, std::vector<std::shared_ptr<StepAction>> actions)
    : m_step_id(step_id), m_step_type(step_type), m_activated(false), m_actions(actions) {}

unsigned int Step::getStepId() const { return m_step_id; }

bool Step::isInitialStep() const { return m_step_type == Step::INIT_STEP; }

bool Step::isMacroStep() const { return m_step_type == Step::MACRO_STEP; }

bool Step::isActivated() const { return m_activated; }

void Step::setActivated(bool activated) {
  m_activated = activated;
#ifdef DEBUG_MODE
  if (activated) {
    std::cout << "Step " << m_step_id << " activated !" << std::endl;
  }
#endif
}

Step::StepType Step::type() const { return m_step_type; }

void Step::addStepAction(std::shared_ptr<StepAction> a) { m_actions.push_back(std::move(a)); }

const std::vector<std::shared_ptr<StepAction>> &Step::getActions() const { return m_actions; }

void Step::addTransition(std::shared_ptr<Transition> t) { m_next_transitions.push_back(t); }

const std::vector<std::shared_ptr<Transition>> &Step::getNextTransitions() const { return m_next_transitions; }