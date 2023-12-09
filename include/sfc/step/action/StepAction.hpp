#pragma once

#include <functional>

using StepActionCallback = std::function<void()>;

class StepAction {
private:
  /* data */
  StepActionCallback m_action_callback = nullptr;

public:
  StepAction(StepActionCallback action_callback = nullptr);
  ~StepAction() = default;

  /**
   * @brief Execute Step Action !
   */
  void exec() const;
  /**
   * @brief Functor. Call 'exec' method.
   */
  void operator()() const;
};
