#pragma once

#include <atomic>
#include <memory>
#include <vector>

class StepAction;
class Transition;

/**
 * @brief Like the square (or doubled square) from Grafcet norm.
 */
class Step {

public:
  enum StepType : uint8_t { INIT_STEP, DEFAULT_STEP, END_STEP, MACRO_STEP };

protected:
  /**
   * @brief Step ID.
   */
  unsigned int m_step_id;
  /**
   * @brief True if this is an initial step.
   */
  StepType m_step_type;
  /**
   * @brief True if is activated (Currently running, all steps actions triggered).
   */
  std::atomic_bool m_activated;
  /**
   * @brief Step Actions.
   */
  std::vector<std::shared_ptr<StepAction>> m_actions;
  /**
   * @brief Next steps after crossing transition.
   * @note Transitions are exclusives each other.
   * If there are several transitions, they shall all reach the same step,
   * and the transitions before this step must have only one next step !
   */
  std::vector<std::shared_ptr<Transition>> m_next_transitions;

public:
  /**
   * @brief Construct a new Step.
   * @param step_id ID.
   * @param init Is it an init-step ?
   * @param m_actions Step actions !
   */
  Step(unsigned int step_id, StepType step_type = DEFAULT_STEP, std::vector<std::shared_ptr<StepAction>> m_actions = {});
  virtual ~Step() = default;
  /**
   * @brief Get the Step Id/
   * @return unsigned int
   */
  unsigned int getStepId() const;
  /**
   * @brief To know if it's an init-step.
   * @return true
   * @return false
   */
  bool isInitialStep() const;
  /**
   * @brief To know if it's a macro-step.
   * @return true
   * @return false
   */
  bool isMacroStep() const;
  /**
   * @brief To know if step is activated.
   * @return true
   * @return false
   */
  virtual bool isActivated() const;
  /**
   * @brief Set the Step Activated.
   * @param activated
   */
  void setActivated(bool activated);
  /**
   * @brief Get StepType.
   * @return StepType
   */
  StepType type() const;
  /**
   * @brief Add action to step.
   * @param a  Action to add.
   */
  void addStepAction(std::shared_ptr<StepAction> a);
  /**
   * @brief Get the Actions.
   * @return const std::vector<std::unique_ptr<StepAction>>&
   */
  const std::vector<std::shared_ptr<StepAction>> &getActions() const;
  /**
   * @brief Add transition to step. Can have one or several.
   * If there are several transitions, they are exclusive, the first one having it's receptivity "true",
   * will we the choosen one to cross (so std::vector order matter if the formulas (the different paths)
   * are not exclusives themselves.
   * @param t Transition to add.
   */
  virtual void addTransition(std::shared_ptr<Transition> t);
  /**
   * @brief Get the Next Transitions.
   * @return const std::vector<std::shared_ptr<Transition>>&
   */
  const std::vector<std::shared_ptr<Transition>> &getNextTransitions() const;
};
