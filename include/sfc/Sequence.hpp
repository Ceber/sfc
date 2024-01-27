#pragma once

#include "sfc/ctpl_stl.h"
#include "sfc/step/Macro.hpp"
#include "sfc/step/Step.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

/**
 * @brief RAII hack to unlock steps waiting for previous ones.
 * This enables to synchronize the call the 'run' method.
 */
class StepActivation;

/**
 * @brief A sequence:
 * - Is represented by a chart where steps and transitions are alternatively following.
 * - Checks its basic consistency like graph-cycling and zero/one checks.
 * - Cannot magically checks all possible logic consistencies,
 *   if you do shitty logic, it will run this shitty logic.
 * - Is handled asynchronously, by spawning detached threads (Strict usage avoid deadlocks...for now).
 *
 * @todo This implementation is very naive and simplist ! But:
 * - First make it work...
 * - Then make it right...
 * - Then make it fast !
 */
class Sequence {
  friend class StepActivation;

private:
  /**
   * @brief To protect access to 'm_steps_required_call_count' and 'm_steps_current_call_count'.
   */
  std::mutex map_mutex;
  /**
   * @brief To sync callbacks triggerring.
   */
  std::mutex seq_cb_mutex;
  /**
   * @brief To sync callbacks triggerring.
   */
  std::mutex step_cb_mutex;
  /**
   * @brief Thread pool size (threads count).
   */
  uint32_t m_thread_pool_size = 0;
  /**
   * @brief Sequence thead pool responsible for running all steps.
   * If the pool is lacking idling threads, we considere the sequence as fucked (Crazy-looping).
   */
  std::unique_ptr<ctpl::thread_pool> m_thread_pool;

  /**
   * @brief Wait delay between each transition polling validity check.
   * The unit of this value is 'microsecond
   */
  unsigned int m_transition_polling_delay = 100;

  /**
   * @brief Running state.
   */
  std::atomic_bool m_running;
  /**
   * @brief Running state.
   */
  std::atomic_int32_t m_stop_code = 0;
  /**
   * @brief Currently running steps count.
   */
  std::atomic_uint32_t m_running_steps;

  /**
   * @brief To sync steps maps access.
   */
  mutable std::mutex steps_mutex;
  /**
   * @brief Initial sequence steps. The one that is represented by a doubled squared.
   * We can choose with wich initial step to start the sequence.
   * It only means that the sequence can have several starting points,
   * not that we will run several process loops.
   */
  std::unordered_map<unsigned int, std::shared_ptr<Step>> m_initial_steps;
  /**
   * @brief All steps.
   */
  std::unordered_map<unsigned int, std::shared_ptr<Step>> m_steps;
  /**
   * @brief Required call count per step-id, before effectively launching the step.
   */
  std::unordered_map<unsigned int, unsigned int> m_steps_required_call_count;
  /**
   * @brief Current call count per step-id, resetted when launching the step.
   */
  std::unordered_map<unsigned int, unsigned int> m_steps_current_call_count;
  /**
   * @brief Callbacks to trigger when the sequence state changed (running or not).
   */
  std::vector<std::function<void(bool)>> m_sequence_changed_callbacks;
  /**
   * @brief Callbacks to trigger when the current step changes.
   */
  std::vector<std::function<void(unsigned int, bool)>> m_step_changed_callbacks;
  /**
   * @brief Callbacks to trigger when the current step changes.
   */
  std::unordered_map<unsigned int, unsigned int> m_macro_deactivations;

  /**
   * @brief Run 'Sequential function chart' from start.
   * @throw std::invalid_argument if step_id is not in 'm_initial_steps'.
   */
  void run();
  /**
   * @brief Run 'Sequential function chart' from step 'step_id'.
   * @throw std::invalid_argument if step_id is not in 'm_initial_steps'.
   */
  void run(unsigned int step_id, std::shared_ptr<Step> previous_step = nullptr,
           std::shared_ptr<std::condition_variable> cond_var = nullptr);

  /**
   * @brief Trigger all callbacks of 'm_sequence_changed_callbacks'
   * @param state
   */
  void fireSequenceChanged(bool state);
  /**
   * @brief Trigger all callbacks of 'm_step_changed_callbacks'
   * @param id
   * @param state
   */
  void fireStepChanged(unsigned int id, bool state);

public:
  static constexpr uint32_t NORMAL_STOP = 0;
  static constexpr uint32_t CRAZY_LOOPING_STOP = 666;
  static constexpr uint32_t CRAZY_PARALLELISM_STOP = 667;

  /**
   * @brief Default constructor.
   */
  Sequence(uint32_t thread_pool_size = std::thread::hardware_concurrency());
  /**
   * @brief Copy Constructor.
   * @param toCopy
   */
  Sequence(const Sequence &toCopy);
  /**
   * @brief Destroy the Sequence object.
   */
  ~Sequence();

  /**
   * @brief Get the Transition Polling Delay.
   * @return unsigned int
   */
  unsigned int getTransitionPollingDelay() const;
  /**
   * @brief Set the Transition Polling Delay.
   * @param delay
   */
  void setTransitionPollingDelay(unsigned int delay);
  /**
   * @brief Add Step to Sequence.
   * @param step
   */
  void addStep(std::shared_ptr<Step> step);

  /**
   * @brief Check the consistency of the sequence:
   * - Every path must loop though an init-step.
   * - Every step must have at least one previous and one next transition (can have several in some cases).
   *
   * @return true
   * @return false
   */
  bool isValid() const;
  /**
   * @brief Sequence Running State.
   * @return true
   * @return false
   */
  bool isRunning() const;

  /**
   * @brief Get the Stop Code.
   * @return uint32_t
   */
  uint32_t getStopCode() const;

  /**
   * @brief Add new callback to 'm_sequence_changed_callbacks'
   * @param cb
   */
  void addSequenceChangedCallback(std::function<void(bool)> cb);
  /**
   * @brief Clear all of 'm_sequence_changed_callbacks'
   */
  void clearSequenceChangedCallback();

  /**
   * @brief Add new callback to 'm_step_changed_callbacks'
   * @param cb
   */
  void addStepChangedCallback(std::function<void(int, bool)> cb);
  /**
   * @brief Clear all of 'm_sequence_changed_callbacks'
   */
  void clearStepChangedCallback();

  /**
   * @brief Return true if step_id is in sequence.
   * @param id
   * @return true
   * @return false
   */
  bool containsStep(unsigned int id);

  /**
   * @brief Return true if transition is in sequence.
   * @param transition
   * @return true
   * @return false
   */
  bool containsTransition(std::shared_ptr<Transition> transition);

  /**
   * @brief Get the Step By Id.
   * @param id
   * @return std::shared_ptr<Step>
   */
  std::shared_ptr<Step> getStepById(unsigned int id);
  /**
   * @brief Get the Activated Steps.
   * @return std::vector<unsigned int>
   */
  const std::vector<std::shared_ptr<Step>> getActivatedSteps() const;

  /**
   * @brief Stop 'Sequential function chart'.
   */
  void start(unsigned int init_step_id = 0);

  /**
   * @brief Stop 'Sequential function chart'.
   */
  void stop(bool lock = true);
};
