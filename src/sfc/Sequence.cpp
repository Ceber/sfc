#include "sfc/Sequence.hpp"
#include "sfc/step/action/StepAction.hpp"
#include "sfc/transition/Receptivity.hpp"
#include "sfc/transition/Transition.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

/**
 * @brief RAII hack to unlock steps waiting for previous ones.
 * This enable synchronize the call the 'run' method.
 */
class StepActivation {
private:
  Sequence &seq;
  Step &step;

  std::mutex notif_mutex;
  std::function<void()> notifications;

public:
  StepActivation(Sequence &seq, Step &step) : seq(seq), step(step) {
    step.setActivated(true);
    seq.fireStepChanged(step.getStepId(), step.isActivated());
  }
  ~StepActivation() { reset(); }

  void reset() {
    step.setActivated(false);
    seq.fireStepChanged(step.getStepId(), step.isActivated());
    std::lock_guard<std::mutex> _lock(notif_mutex);
    if (notifications) {
      notifications();
    }
  }

  void setNotifications(std::function<void()> notif) {
    std::lock_guard<std::mutex> _lock(notif_mutex);
    this->notifications = notif;
  }
};

Sequence::Sequence(uint32_t thread_pool_size)
    : m_thread_pool_size(thread_pool_size), m_thread_pool(nullptr), m_running(false), m_running_steps(0) {}

Sequence::Sequence(const Sequence &toCopy) : Sequence() {

  for (auto init_step : toCopy.m_initial_steps) {
    m_initial_steps[init_step.first] = init_step.second;
  }

  for (auto step : toCopy.m_steps) {
    m_steps[step.first] = step.second;
  }
}

Sequence::~Sequence() {
  stop();
  {
    m_sequence_changed_callbacks.clear();
    std::lock_guard<std::mutex> lock(seq_cb_mutex);
  }
  {
    std::lock_guard<std::mutex> lock(step_cb_mutex);
    m_step_changed_callbacks.clear();
  }
}

unsigned int Sequence::getTransitionPollingDelay() const { return m_transition_polling_delay; }

void Sequence::setTransitionPollingDelay(unsigned int delay) { this->m_transition_polling_delay = delay; }

using StepsMap = std::unordered_map<unsigned int, std::shared_ptr<Step>>;

void Sequence::addStep(std::shared_ptr<Step> step) {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  if (!step) {
    throw std::invalid_argument("Trying to add nullptr Step to sequence !");
  } else if (m_initial_steps.count(step->getStepId()) || m_steps.count(step->getStepId())) {
    throw std::invalid_argument("Step id already used in this sequence !");
  } else if (m_running) {
    throw std::runtime_error("Trying to a step while sequence is running ! That's forbidden !");
  }
  if (step->isInitialStep()) {
    m_initial_steps[step->getStepId()] = step;
  } else if (step->isMacroStep()) {
    /// @todo Copy the macro and all its steps ! Or not ?
    /// How do we do when a macro is used by several sequences ?
    auto macro = std::dynamic_pointer_cast<Macro>(step);
    m_steps.insert(macro->steps().begin(), macro->steps().end());
    m_steps[step->getStepId()] = step;
  } else {
    m_steps[step->getStepId()] = step;
  }
}

bool checkMacro(Macro &current_step) {
  /// A Macro must have a first step.
  bool ret = (current_step.first() != nullptr && current_step.steps().size() > 1);

  if (ret) {
    /// Each step must have a transition.
    for (auto &step : current_step.steps()) {
      if (ret) {
        ret &= (step.second->getNextTransitions().size() > 0);
      } else {
        break;
      }
    }
  }

  return ret;
}

/**
 * @brief Helper function that recursively check if we respect the chart constraints.
 *
 * @param current_step The currently checked step.
 * @param traversed_steps The already traversed steps.
 * @param check_traversed If true, not only check for init-step, also if looping on already traversed one.
 * @return true if OK
 * @return false if NOK
 */
bool loopCheck(Step &current_step, std::vector<unsigned int> &traversed_steps, bool check_traversed = false) {
  bool ret = current_step.getNextTransitions().size() > 0;

  if (current_step.isMacroStep()) {
    ret &= checkMacro(static_cast<Macro &>(current_step));
  }

  if (check_traversed &&
      std::find(traversed_steps.begin(), traversed_steps.end(), current_step.getStepId()) != traversed_steps.end()) {
    return true;
  }

  traversed_steps.push_back(current_step.getStepId());
  for (auto t : current_step.getNextTransitions()) {
    // Every transition must have at least one next step ! And consitent validation !
    if (t->nexts().size() > 0 && t->validations().size() > 0) {
      // One of the nexts has to reach an init-step,
      // the others have to loop somewhere else,
      // Surely to an already traversed step !
      /// @todo Check & fix this !
      for (auto next : t->nexts()) {
        if (next.lock()->isInitialStep()) {
          continue;
        }
        ret &= loopCheck(*next.lock(), traversed_steps, !current_step.isInitialStep());
      }
    } else {
      std::cerr << "Transition is missing 'nexts' or 'validations' ! " << std::endl;
      return false;
    }
  }
  return ret;
}

bool Sequence::isValid() const {
  /// @todo Consistency checks !
  std::lock_guard<std::mutex> _lock(steps_mutex);
  if (m_steps.size() == 0 || m_initial_steps.size() == 0) {
    std::cerr << "Are you fucking kidding me ?" << std::endl;
    return false;
  }
  bool ret = true;
  std::vector<unsigned int> traversed_steps;
  bool first = true;
  for (auto init_step_p : m_initial_steps) {
    const auto init_step = init_step_p.second;
    ret &= loopCheck(*init_step, traversed_steps, !first);
    first = false;
  }
  if (!ret) {
    std::cerr << "One of the initial_steps is not looping to another (or same) init-step." << std::endl;
  }
  return ret;
}

bool Sequence::isRunning() const { return m_running.load(); }

uint32_t Sequence::getStopCode() const { return m_stop_code; }

void Sequence::addSequenceChangedCallback(std::function<void(bool)> cb) {
  std::lock_guard<std::mutex> lock(seq_cb_mutex);
  m_sequence_changed_callbacks.push_back(cb);
}

void Sequence::clearSequenceChangedCallback() {
  std::lock_guard<std::mutex> lock(seq_cb_mutex);
  m_sequence_changed_callbacks.clear();
}

void Sequence::addStepChangedCallback(std::function<void(int, bool)> cb) {
  std::lock_guard<std::mutex> lock(step_cb_mutex);
  m_step_changed_callbacks.push_back(cb);
}

void Sequence::clearStepChangedCallback() {
  std::lock_guard<std::mutex> lock(step_cb_mutex);
  m_step_changed_callbacks.clear();
}

bool Sequence::containsStep(unsigned int id) {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  return m_initial_steps.count(id) || m_steps.count(id);
}

bool Sequence::containsTransition(std::shared_ptr<Transition> transition) {
  auto search = [&transition](StepsMap &map) -> bool {
    for (StepsMap::value_type &init_step : map) {
      auto &step = init_step.second;
      if (step) {
        auto &next_transitions = step->getNextTransitions();
        auto it = std::find(next_transitions.begin(), next_transitions.end(), transition);
        if (it != next_transitions.end()) {
          return true;
        }
      }
    }
    return false;
  };
  std::lock_guard<std::mutex> _lock(steps_mutex);
  return search(m_initial_steps) || search(m_steps);
}

std::shared_ptr<Step> Sequence::getStepById(unsigned int id) {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  if (m_initial_steps.count(id)) {
    return m_initial_steps.at(id);
  } else if (m_steps.count(id)) {
    return m_steps.at(id);
  }
  return nullptr;
}

const std::vector<std::shared_ptr<Step>> Sequence::getActivatedSteps() const {
  std::lock_guard<std::mutex> _lock(steps_mutex);
  std::vector<std::shared_ptr<Step>> to_vector;
  std::transform(m_steps.begin(), m_steps.end(), std::back_inserter(to_vector), [](auto &kv) { return kv.second; });
  std::transform(m_initial_steps.begin(), m_initial_steps.end(), std::back_inserter(to_vector),
                 [](auto &kv) { return kv.second; });
  to_vector.erase(
      std::remove_if(to_vector.begin(), to_vector.end(), [](const std::shared_ptr<Step> &s) { return s->isActivated(); }),
      to_vector.end());
  return to_vector;
}

void Sequence::run() { run(m_initial_steps[0]->getStepId()); }

void Sequence::run(unsigned int step_id, std::shared_ptr<Step> previous_step, std::shared_ptr<std::condition_variable> cond_var) {
  m_running_steps++;
  if (m_running) {
    steps_mutex.lock();
    auto it = m_steps.find(step_id);
    if (it == m_steps.end() && ((it = m_initial_steps.find(step_id)) == m_initial_steps.end())) {
      throw std::invalid_argument("Trying to run an invalid step (Id not found) !");
    }
    steps_mutex.unlock();

    Step &step_to_run = *it->second;
    if (!step_to_run.isMacroStep()) {
      /// Launch steps actions even if not yet activated ;)
      for (const auto &a : step_to_run.getActions()) {
        (*a)();
      }
    }

    /// To properly finish the last triggered steps.
    {
      std::mutex mumu;
      std::unique_lock<std::mutex> lock(mumu);
      while (previous_step && m_running && previous_step->isActivated()) {
        // Don't race with previous step !
        using namespace std::chrono_literals;
        cond_var->wait_for(lock, 100ms, [=, &previous_step]() { return !previous_step->isActivated() || !m_running; });
      }
    }
    StepActivation activation_guard(*this, step_to_run);
#ifdef DEBUG_MODE
    std::cout << "Running step #" << step_id << std::endl;
#endif
    bool done = false;
    std::atomic<uint32_t> waiting_steps(0);
    std::vector<std::weak_ptr<Step>> m_about_to_run_steps;
    typedef std::shared_ptr<std::condition_variable> SP_CondVar;
    SP_CondVar cond_var = std::make_shared<std::condition_variable>();
    /// Run receptivity(ies) detection(s).
    while (m_running && !done) {
      for (const auto &t : step_to_run.getNextTransitions()) {
        // Wait to be trigger and check the transition state and the bool reference.
        if (m_running && t->getReceptivityState()) {
          using namespace std::chrono_literals;
          done = true;
          // If several next steps. We need to find the next common transition.
          // The step(s) after this transition must only be launch once !
          // So we could count how many times 'run' is called with a given id ?
          // Also, if a branch has finished, we should not trigger already running steps !

          m_about_to_run_steps = t->nexts();
          if (m_about_to_run_steps.size() > m_thread_pool_size) {
            m_running = false;
            m_stop_code = CRAZY_PARALLELISM_STOP;
            activation_guard.reset();
            fireSequenceChanged(m_running);
            throw std::runtime_error(
                "Not enough threads available to run sequence. Too big parallelism detection -> Sequence stopped !");
          }
          for (auto &step : t->nexts()) {
            bool is_macro = step.lock()->isMacroStep();
            if (is_macro) {
              step.lock()->setActivated(true);
              m_macro_deactivations[std::dynamic_pointer_cast<Macro>(step.lock())->last()->getStepId()] =
                  step.lock()->getStepId();
            }
            Step &s = is_macro ? *std::dynamic_pointer_cast<Macro>(step.lock())->first() : *step.lock();
            if (m_running && !s.isActivated()) {
              SP_CondVar step_cond_var = std::make_shared<std::condition_variable>();
              int next_id = s.getStepId();
              {
                std::lock_guard<std::mutex> _lock(map_mutex);
                if (!m_steps_required_call_count.count(next_id)) {
                  m_steps_required_call_count[next_id] = t->validations().size();
                  m_steps_current_call_count[next_id] = 0;
                }
                m_steps_current_call_count[next_id] = m_steps_current_call_count[next_id] + 1;
                if (m_steps_required_call_count.count(next_id) &&
                    m_steps_current_call_count[next_id] == m_steps_required_call_count[next_id]) {
                  m_steps_current_call_count[next_id] = 0;
                  uint32_t available_threads_count = 0;
                  {
                    std::lock_guard<std::mutex> _lock(thread_pool_mutex);
                    available_threads_count = m_thread_pool->n_idle();
                  }
                  if (m_running && (available_threads_count == 0 || m_running_steps > m_thread_pool_size)) {
                    m_running = false;
                    m_stop_code = CRAZY_LOOPING_STOP;
                    activation_guard.reset();
                    fireSequenceChanged(m_running);
                    throw std::runtime_error(
                        "No more thread available to run sequence. Crazy-Looping detection -> Sequence stopped !");
                  } else if (m_running) {
#ifdef DEBUG_MODE
                    std::cout << "run step id:" << next_id << std::endl;
#endif
                    {
                      std::lock_guard<std::mutex> _lock(thread_pool_mutex);
                      m_thread_pool->push([=](int) { run(next_id, it->second, cond_var); });
                    }
                  }
                }
              }
            }
          }
          break;
        } else if (!m_running) {
          break;
        }
      }
      if (m_running) {
        // This delay is to avoid 100% CPU taken by while loop...
        std::this_thread::sleep_for(std::chrono::microseconds(m_transition_polling_delay));
      } else {
        break;
      }
    }

    // Wait for all steps to be activated.
    // Here, if we have several parallel steps...one of them can catch back the previous step...
    // So we use condition_variables to protect the run method, and it wait that the previous is gone before rushing.
    // The notification that enable next steps to continue is sent by 'StepActivation activation_guard' destructor.
    /// @warning What about when we have 2 times the same macro in the sequence ?
    /// I think each of them has to be copied, to avoid "crossed-notifications",
    /// and IDs of macro's steps have to be modified, to avoid conflicts.
    if (m_running) {
      activation_guard.setNotifications([=, &step_to_run]() {
        if (m_macro_deactivations.count(step_to_run.getStepId())) {
          auto macro = m_steps[m_macro_deactivations.at(step_to_run.getStepId())];
          macro->setActivated(false);
          fireStepChanged(macro->getStepId(), macro->isActivated());
          m_macro_deactivations.erase(step_to_run.getStepId());
        }
        for (auto it = m_about_to_run_steps.begin(); it != m_about_to_run_steps.end(); it++) {
          if (!it->expired()) {
            Step &s = *it->lock();
            do {
              cond_var->notify_all();
            } while (m_running && !(s.isActivated()));
          }
        }
      });
    }
    m_running_steps--;
#ifdef DEBUG_MODE
    std::cout << "done step id:" << step_id << std::endl;
#endif
  }
}

void Sequence::fireSequenceChanged(bool state) {
  std::lock_guard<std::mutex> lock(seq_cb_mutex);
  std::lock_guard<std::mutex> lock2(step_cb_mutex);
  for (auto &callback : m_sequence_changed_callbacks) {
    if (callback) {
      callback(state);
    }
  }
}

void Sequence::fireStepChanged(unsigned int id, bool state) {
  if (m_running) {
    std::lock_guard<std::mutex> lock(seq_cb_mutex);
    std::lock_guard<std::mutex> lock2(step_cb_mutex);
    for (auto &callback : m_step_changed_callbacks) {
      if (callback) {
        callback(id, state);
      }
    }
  }
}

void Sequence::start(unsigned int init_step_id) {
  steps_mutex.lock();
  bool all_transition_true = std::all_of(m_steps.begin(), m_steps.end(), [](const auto &p) {
    const Step &s = *p.second;
    return std::all_of(s.getNextTransitions().begin(), s.getNextTransitions().end(),
                       [](const auto &t) { return t->getReceptivityState(); });
  });
  all_transition_true &= std::all_of(m_initial_steps.begin(), m_initial_steps.end(), [](const auto &p) {
    const Step &s = *p.second;
    return std::all_of(s.getNextTransitions().begin(), s.getNextTransitions().end(),
                       [](const auto &t) { return t->getReceptivityState(); });
  });
  steps_mutex.unlock();
  if (all_transition_true) {
    throw std::logic_error("Trying to run a sequence with all transitions true at startup is not allowed...for the moment !");
  }
  if (!isValid()) {
    throw std::runtime_error("Trying to run an invalid sequence !");
  }
  m_running = true;
  fireSequenceChanged(m_running);
  {
    std::lock_guard<std::mutex> lock(thread_pool_mutex);
    m_thread_pool = std::make_unique<ctpl::thread_pool>(m_thread_pool_size);
  }
  run(init_step_id);
}

void Sequence::stop(bool lock) {
  m_running = false;
  m_stop_code = NORMAL_STOP;
  {
    if (lock) {
      std::lock_guard<std::mutex> _lock(thread_pool_mutex);
      if (m_thread_pool) {
        // Wait for steps termination.
        m_thread_pool->stop(true);
        m_thread_pool.reset(nullptr);
      }
    } else {
      if (m_thread_pool) {
        // Wait for steps termination.
        m_thread_pool->stop(true);
        m_thread_pool.reset(nullptr);
      }
    }

    fireSequenceChanged(m_running);
  }
}