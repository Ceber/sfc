#pragma once

#include "sfc/transition/Receptivity.hpp"
#include <condition_variable>
#include <memory>
#include <vector>

class Step;

class Transition {

private:
  /**
   * @brief Next steps after crossing transition.
   * @note If several simultaneous steps, so they shall all reach the same transition.
   * And the last steps before this transition must only contains one transition !
   */
  std::vector<std::weak_ptr<Step>> m_next_steps;
  /**
   * @brief Steps taking part of the validation check.
   * 'validation_count' steps need to be validated before crossing transition.
   */
  std::vector<std::weak_ptr<Step>> m_validation_steps;
  /**
   * @brief Transition receptivity.
   */
  std::atomic_bool m_receptivity_state;

public:
  static auto mk_sp_transition(std::initializer_list<std::weak_ptr<Step>> nexts,
                               std::initializer_list<std::weak_ptr<Step>> validations) {
    return std::make_shared<Transition>(nexts, validations);
  }

  Transition(std::vector<std::weak_ptr<Step>> next_steps = {}, std::vector<std::weak_ptr<Step>> validation_steps = {});

  ~Transition() = default;

  /**
   * @brief Get all next steps.
   * @return const std::vector<std::weak_ptr<Step>>&
   */
  const std::vector<std::weak_ptr<Step>> &nexts() const;
  /**
   * @brief Get all validations steps.
   * @return const std::vector<std::weak_ptr<Step>>&
   */
  const std::vector<std::weak_ptr<Step>> &validations() const;
  /**
   * @brief Get the Receptivity State/
   * @return true
   * @return false
   */
  bool getReceptivityState() const;
  /**
   * @brief Set the associated receptivity state.
   */
  void setReceptivityState(bool state);
};