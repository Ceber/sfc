#pragma once

#include <atomic>

class Receptivity {
private:
  std::atomic_bool m_state;

public:
  Receptivity();
  virtual ~Receptivity() = default;

  /**
   * @brief Get the receptivity state.
   * 'true' Means that the transition receptivity conditions are met. Can cross transition if previous steps are validated.
   * @return true
   * @return false
   */
  bool state();
  /**
   * @brief Set the State.
   * Validate the associated transition if 'state' and 'validate_transition' are true.
   * @param state
   */
  void setState(bool state);
};