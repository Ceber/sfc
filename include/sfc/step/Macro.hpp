#pragma once

#include <mutex>
#include <unordered_map>

#include "Step.hpp"

class Macro : public Step {
private:
  /**
   * @brief All macro's steps.
   */
  std::unordered_map<unsigned int, std::shared_ptr<Step>> m_macro_steps;
  /**
   * @brief First added step.
   */
  std::shared_ptr<Step> m_first = nullptr;
  /**
   * @brief Last added step.
   */
  std::shared_ptr<Step> m_last = nullptr;
  /**
   * @brief To sync steps maps access.
   */
  mutable std::mutex steps_mutex;

public:
  /**
   * @brief Construct a new Macro.
   * @param step_id
   * @param step_type
   * @param m_actions
   */
  Macro(unsigned int step_id, std::vector<std::shared_ptr<StepAction>> m_actions = {});

  /**
   * @brief Add a step to the macro.
   * The firs added step is considered as the "first step".
   * @param step
   */
  void addStep(std::shared_ptr<Step> step);
  /**
   * @brief Return true if step_id is in sequence.
   * @param id
   * @return true
   * @return false
   */
  bool containsStep(unsigned int id) const;
  /**
   * @brief Get first macro's step.
   * @return Step&
   * @throw if no steps in 'm_macro_steps'.
   */
  std::shared_ptr<Step> first();
  /**
   * @brief Get last macro's step.
   * @return Step&
   * @throw if no steps in 'm_macro_steps'.
   */
  std::shared_ptr<Step> last();

  /**
   * @brief Get all macro's steps.
   * @return std::unordered_map<unsigned int, std::shared_ptr<Step>>
   */
  const std::unordered_map<unsigned int, std::shared_ptr<Step>> &steps() const;

  /**
   * @brief To know if step is activated.
   * @return true
   * @return false
   */
  bool isActivated() const override;
  /**
   * @brief Add transition to step. Can have one or several.
   * If there are several transitions, they are exclusive, the first one having it's receptivity "true",
   * will we the choosen one to cross (so std::vector order matter if the formulas (the different paths)
   * are not exclusives themselves.
   * @param t Transition to add.
   */
  void addTransition(std::shared_ptr<Transition> t) override;
};