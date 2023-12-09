#pragma once

#include "../SfcTest.h"
#include <sfc/Sequence.hpp>
#include <sfc/step/action/StepAction.hpp>
#include <sfc/transition/Receptivity.hpp>
#include <sfc/transition/Transition.hpp>

TEST_F(SfcTest, Create_Delete_Sequence) { Sequence seq; }

/// @brief See 'doc/Unique_Sequence.png'
TEST_F(SfcTest, Unique_Sequence_Validity) {
  Sequence seq;
  std::shared_ptr<Step> useless_step = std::make_shared<Step>(1000, Step::INIT_STEP);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  EXPECT_THROW(seq.addStep(nullptr), std::invalid_argument);
  seq.addStep(init_step);
  EXPECT_THROW(seq.addStep(init_step), std::invalid_argument);
  seq.addStep(first_step);
  seq.addStep(second_step);
  EXPECT_TRUE(seq.containsStep(init_step->getStepId()));
  EXPECT_TRUE(seq.containsStep(first_step->getStepId()));
  EXPECT_TRUE(seq.containsStep(second_step->getStepId()));
  EXPECT_FALSE(seq.containsStep(useless_step->getStepId()));
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  EXPECT_TRUE(seq.containsTransition(t1));
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  EXPECT_TRUE(seq.containsTransition(t2));
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  EXPECT_FALSE(seq.isValid());
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.containsTransition(t3));
  EXPECT_TRUE(seq.isValid());
}

TEST_F(SfcTest, Unique_Sequence_Two_Init_Validity) {
  Sequence seq;
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> degraded_init_step = std::make_shared<Step>(10, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(degraded_init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  EXPECT_TRUE(seq.containsStep(init_step->getStepId()));
  EXPECT_TRUE(seq.containsStep(degraded_init_step->getStepId()));
  EXPECT_TRUE(seq.containsStep(first_step->getStepId()));
  EXPECT_TRUE(seq.containsStep(second_step->getStepId()));
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  EXPECT_TRUE(seq.containsTransition(t1));
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  EXPECT_TRUE(seq.containsTransition(t2));
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.containsTransition(t3));
  EXPECT_FALSE(seq.isValid());
  std::shared_ptr<Transition> t10 = Transition::mk_sp_transition({second_step}, {degraded_init_step});
  degraded_init_step->addTransition(t10);
  EXPECT_TRUE(seq.isValid());
}

/// @brief See 'doc/Unique_Valid_Sequence.png'
TEST_F(SfcTest, Unique_Sequence_With_Orphan_Transition) {
  Sequence seq;
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  EXPECT_TRUE(seq.containsTransition(t1));
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  EXPECT_TRUE(seq.containsTransition(t2));
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({}, {second_step});
  EXPECT_FALSE(seq.isValid());
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.containsTransition(t3));
  EXPECT_FALSE(seq.isValid());
}

TEST_F(SfcTest, Unique_Sequence_Transition_Without_Validation) {
  Sequence seq;
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  EXPECT_TRUE(seq.containsTransition(t1));
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  EXPECT_TRUE(seq.containsTransition(t2));
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {});
  EXPECT_FALSE(seq.isValid());
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.containsTransition(t3));
  EXPECT_FALSE(seq.isValid());
}

TEST_F(SfcTest, Unique_Sequence_Only_With_Init) {
  Sequence seq;
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  seq.addStep(init_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({init_step}, {init_step});
  init_step->addTransition(t1);

  EXPECT_FALSE(seq.isValid());
}

using namespace std::chrono_literals;
bool callback_called = false;
StepActionCallback action_callback = []() { callback_called = true; };
std::atomic<int32_t> callback_called_count(0);
StepActionCallback count_action_callback = []() { callback_called_count++; };

void stepChanged(unsigned int step_id, bool state) {
#ifdef DEBUG_MODE
  std::cout << "Step " << (state ? "activated" : "deactivated") << " => #" << step_id << std::endl;
#endif
}

void seqChanged(bool state) {
#ifdef DEBUG_MODE
  if (state) {
    std::cout << "Sequence activated" << std::endl;
  }
#endif
}

/// @warning Waiting on a step is not a normal-use case.
/// We can have unfixable deadlock with std::condition_variable
/// and segfaults with current implementation...if polling to fast...
/// Need to be improved...

const unsigned int default_transition_poll_delay = Sequence().getTransitionPollingDelay();

void waitForStep(Step &step) {
#ifdef DEBUG_MODE
  std::cout << "Waiting for step ! (#" << step.getStepId() << ")" << std::endl;
#endif
  while (!step.isActivated()) {
    std::this_thread::sleep_for(
        std::chrono::microseconds(default_transition_poll_delay * 3)); // be nice with all those async stuff...
  }
}

void waitForStep(Step &step, Transition &t) {
#ifdef DEBUG_MODE
  std::cout << "Waiting for step ! (#" << step.getStepId() << ")" << std::endl;
#endif
  t.setReceptivityState(true); // Set the needed transition state to true.
  while (!step.isActivated()) {
    std::this_thread::sleep_for(
        std::chrono::microseconds(default_transition_poll_delay * 3)); // be nice with all those async stuff...
  }
}

void waitForSteps(std::vector<std::shared_ptr<Step>> steps, std::vector<std::shared_ptr<Transition>> trans) {
  bool ret = true;
  for (auto &t : trans) {
    t->setReceptivityState(true); // Set the needed transition state to true.
  }
  for (auto &&step : steps) {
    ret &= step->isActivated();
  }
  while (!ret) {
    ret = true;
    for (auto &&step : steps) {
      ret &= step->isActivated();
    }
  }
}

TEST_F(SfcTest, Copy_Sequence) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  Sequence seq_copy(seq);
  EXPECT_TRUE(seq_copy.containsStep(0));
  EXPECT_TRUE(seq_copy.containsStep(1));
  EXPECT_TRUE(seq_copy.containsStep(2));
}

TEST_F(SfcTest, Delete_Running_Sequence) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  t1->setReceptivityState(false);
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  std::thread t([&seq]() { seq.start(); });
  t.detach();
  std::this_thread::sleep_for(std::chrono::microseconds(default_transition_poll_delay * 2)); // Wait for thread launch.
}

TEST_F(SfcTest, Run_Invalid_Step_Id) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  EXPECT_THROW(seq.start(9999), std::invalid_argument);
}

TEST_F(SfcTest, Run_Unique_Sequence_With_One_Action) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);       // Wait for step activation.
  waitForStep(*first_step, *t1); // Wait for step activation.
  t1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t3); // Wait for step activation.
  t3->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  callback_called = false;
}

TEST_F(SfcTest, Run_Unique_Sequence_With_Two_Actions) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(init_step->getActions().size(), 2);
  EXPECT_EQ(first_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);       // Wait for step activation.
  waitForStep(*first_step, *t1); // Wait for step activation.
  t1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t3); // Wait for step activation.
  t3->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 3);
  callback_called = false;
  callback_called_count = 0;
}

TEST_F(SfcTest, Run_Unique_Sequence_With_One_Action_Two_Loops) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);       // Wait for step activation.
  waitForStep(*first_step, *t1); // Wait for step activation.
  t1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t3); // Wait for step activation.
  t3->setReceptivityState(false);
  EXPECT_TRUE(callback_called);
  callback_called = false;
  waitForStep(*first_step, *t1); // Wait for step activation.
  t1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t3); // Wait for step activation.
  t3->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  callback_called = false;
}

TEST_F(SfcTest, Run_Unique_Sequence_With_Action_At_Each_Step) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  EXPECT_FALSE(seq.isValid());
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);       // Wait for step activation.
  waitForStep(*first_step, *t1); // Wait for step activation.
  t1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t3); // Wait for step activation.
  t3->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 2);
  callback_called = false;
  callback_called_count = 0;
}

/// @brief See 'doc/Simultaneous_Sequence_Two_Branch.png'
TEST_F(SfcTest, Run_Simultaneous_Sequence_Two_Branch) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step, second_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({init_step}, {first_step, second_step});
  first_step->addTransition(t2);
  second_step->addTransition(t2);
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step); // Wait for step activation (+1)
  waitForSteps({first_step, second_step}, {t1});
  t1->setReceptivityState(false);
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation
  t2->setReceptivityState(false);
  waitForSteps({first_step, second_step}, {t1});
  t1->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation
  t2->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 7);
  callback_called = false;
  callback_called_count = 0;
}

/// @todo See 'doc/Simultaneous_Sequence_Two_Branch_Looping_On_NonInitStep.png'
TEST_F(SfcTest, Run_Simultaneous_Sequence_Two_Branch_Looping_On_NonInitStep) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> after_first_step = std::make_shared<Step>(11, Step::DEFAULT_STEP);
  std::shared_ptr<Step> join_step = std::make_shared<Step>(12, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(after_first_step);
  seq.addStep(join_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step, second_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t11 = Transition::mk_sp_transition({after_first_step}, {first_step});
  init_step->addTransition(t11);
  std::shared_ptr<Transition> t111 = Transition::mk_sp_transition({first_step}, {after_first_step});
  init_step->addTransition(t111);
  std::shared_ptr<Transition> t112 = Transition::mk_sp_transition({join_step}, {after_first_step});
  init_step->addTransition(t112);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({init_step}, {join_step, second_step});
  first_step->addTransition(t11);
  after_first_step->addTransition(t111);
  after_first_step->addTransition(t112);
  join_step->addTransition(t2);
  second_step->addTransition(t2);
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  /// make a loop to run two times the first_step
  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step); // Wait for step activation (+1)
  waitForSteps({first_step, second_step}, {t1});
  t1->setReceptivityState(false);
  t2->setReceptivityState(false);
  waitForStep(*after_first_step, *t11); // Wait for step activation
  t11->setReceptivityState(false);
  waitForStep(*first_step, *t111); // Wait for step activation
  t111->setReceptivityState(false);
  waitForStep(*after_first_step, *t11); // Wait for step activation
  t11->setReceptivityState(false);
  waitForStep(*join_step, *t112); // Wait for step activation
  t112->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation
  t2->setReceptivityState(false);
  waitForSteps({first_step, second_step}, {t1});
  t1->setReceptivityState(false);
  waitForStep(*after_first_step, *t11); // Wait for step activation
  t11->setReceptivityState(false);
  waitForStep(*first_step, *t111); // Wait for step activation
  t111->setReceptivityState(false);
  waitForStep(*after_first_step, *t11); // Wait for step activation
  t11->setReceptivityState(false);
  waitForStep(*join_step, *t112); // Wait for step activation
  t112->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation
  t2->setReceptivityState(false);

  seq.stop();
  t.join();

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 9);
  callback_called = false;
  callback_called_count = 0;
}

/// @brief See 'doc/Exclusive_Sequence.png'
TEST_F(SfcTest, Run_Exclusive_Sequence_Two_Branches) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);

  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {init_step});

  std::shared_ptr<Transition> t11 = Transition::mk_sp_transition({init_step}, {first_step});
  std::shared_ptr<Transition> t22 = Transition::mk_sp_transition({init_step}, {second_step});

  init_step->addTransition(t1);
  init_step->addTransition(t2);
  first_step->addTransition(t11);
  second_step->addTransition(t22);

  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step); // Wait for step activation (+1)
  /// Go left branch. Check count.
  waitForStep(*first_step, *t1); // Wait for step activation. Step 1 only. (+1)
  EXPECT_TRUE(first_step->isActivated());
  EXPECT_FALSE(second_step->isActivated());
  t1->setReceptivityState(false);
  waitForStep(*init_step, *t11); // Wait for step activation. Back to Init Step. (+1)
  t11->setReceptivityState(false);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 3);

  callback_called = false;

  /// Go right branch. Check count.
  waitForStep(*second_step, *t2); // Wait for step activation. Step 2 only. (+1)
  EXPECT_FALSE(first_step->isActivated());
  EXPECT_TRUE(second_step->isActivated());
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t22); // Wait for step activation. Back to Init Step. (+1)
  t22->setReceptivityState(false);

  seq.stop();
  t.join();

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 5);
}

TEST_F(SfcTest, Run_Simultaneous_Sequence_Three_Branches) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  std::shared_ptr<Step> third_step = std::make_shared<Step>(3, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  seq.addStep(third_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step, second_step, third_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({init_step}, {first_step, second_step, third_step});
  first_step->addTransition(t2);
  second_step->addTransition(t2);
  third_step->addTransition(t2);
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  third_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(third_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);                                   // Wait for step activation (+1)
  waitForSteps({first_step, second_step, third_step}, {t1}); // Step 1&2&3 simultaneously. (+3)
  t1->setReceptivityState(false);
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation. Go back to Init Step (+1)
  t2->setReceptivityState(false);
  waitForSteps({first_step, second_step, third_step}, {t1});
  t1->setReceptivityState(false);
  waitForStep(*init_step, *t2); // Wait for step activation. Back to Init (+1)
  t2->setReceptivityState(false);

  seq.stop();
  t.join();

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 9);
  callback_called = false;
  callback_called_count = 0;
}

TEST_F(SfcTest, Run_Exclusive_Sequence_Three_Branches) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  std::shared_ptr<Step> third_step = std::make_shared<Step>(3, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  seq.addStep(third_step);

  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {init_step});
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({third_step}, {init_step});

  std::shared_ptr<Transition> t11 = Transition::mk_sp_transition({init_step}, {first_step});
  std::shared_ptr<Transition> t22 = Transition::mk_sp_transition({init_step}, {second_step});
  std::shared_ptr<Transition> t33 = Transition::mk_sp_transition({init_step}, {third_step});

  init_step->addTransition(t1);
  init_step->addTransition(t2);
  init_step->addTransition(t3);
  first_step->addTransition(t11);
  second_step->addTransition(t22);
  third_step->addTransition(t33);

  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  init_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(first_step->getActions().size(), 1);
  second_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(second_step->getActions().size(), 1);
  third_step->addStepAction(std::make_unique<StepAction>(count_action_callback));
  EXPECT_EQ(third_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  callback_called_count = 0;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step); // Wait for step activation (+1)
  /// Go left branch. Check count.
  waitForStep(*first_step, *t1); // Wait for step activation. Step 1 only. (+1)
  EXPECT_TRUE(first_step->isActivated());
  EXPECT_FALSE(second_step->isActivated());
  EXPECT_FALSE(third_step->isActivated());
  t1->setReceptivityState(false);
  waitForStep(*init_step, *t11); // Wait for step activation. Back to Init Step. (+1)
  t11->setReceptivityState(false);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 3);

  callback_called = false;

  /// Go right branch. Check count.
  waitForStep(*second_step, *t2); // Wait for step activation. Step 2 only. (+1)
  EXPECT_FALSE(first_step->isActivated());
  EXPECT_TRUE(second_step->isActivated());
  EXPECT_FALSE(third_step->isActivated());
  t2->setReceptivityState(false);
  waitForStep(*init_step, *t22); // Wait for step activation. Back to Init Step. (+1)
  t22->setReceptivityState(false);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 5);

  callback_called = false;

  /// Go last branch. Check count.
  waitForStep(*third_step, *t3); // Wait for step activation. Step 3 only. (+1)
  EXPECT_FALSE(first_step->isActivated());
  EXPECT_FALSE(second_step->isActivated());
  EXPECT_TRUE(third_step->isActivated());
  t3->setReceptivityState(false);
  waitForStep(*init_step, *t33); // Wait for step activation. Back to Init Step. (+1)
  t33->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_EQ(seq.getStopCode(), Sequence::NORMAL_STOP);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(callback_called_count, 7);
}

TEST_F(SfcTest, Detect_And_Stop_Cray_Parallelism) {

  Sequence seq(2);
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  std::shared_ptr<Step> third_step = std::make_shared<Step>(3, Step::DEFAULT_STEP);
  std::shared_ptr<Step> fourth_step = std::make_shared<Step>(4, Step::DEFAULT_STEP);
  std::shared_ptr<Step> fifth_step = std::make_shared<Step>(5, Step::DEFAULT_STEP);
  std::shared_ptr<Step> sixth_step = std::make_shared<Step>(6, Step::DEFAULT_STEP);
  std::shared_ptr<Step> seventh_step = std::make_shared<Step>(7, Step::DEFAULT_STEP);
  std::shared_ptr<Step> eighth_step = std::make_shared<Step>(8, Step::DEFAULT_STEP);
  std::shared_ptr<Step> nineth_step = std::make_shared<Step>(9, Step::DEFAULT_STEP);
  std::shared_ptr<Step> tenth_step = std::make_shared<Step>(10, Step::DEFAULT_STEP);
  std::shared_ptr<Step> eleventh_step = std::make_shared<Step>(11, Step::DEFAULT_STEP);
  std::shared_ptr<Step> twelfth_step = std::make_shared<Step>(12, Step::DEFAULT_STEP);
  std::shared_ptr<Step> thirteenth_step = std::make_shared<Step>(13, Step::DEFAULT_STEP);
  std::shared_ptr<Step> fourteenth_step = std::make_shared<Step>(14, Step::DEFAULT_STEP);
  std::shared_ptr<Step> fifteenth_step = std::make_shared<Step>(15, Step::DEFAULT_STEP);
  std::shared_ptr<Step> sixteenth_step = std::make_shared<Step>(16, Step::DEFAULT_STEP);
  std::shared_ptr<Step> seventeenth_step = std::make_shared<Step>(17, Step::DEFAULT_STEP);
  std::shared_ptr<Step> eigthteenth_step = std::make_shared<Step>(18, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_first_step = std::make_shared<Step>(111, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_second_step = std::make_shared<Step>(112, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_third_step = std::make_shared<Step>(113, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_fourth_step = std::make_shared<Step>(114, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_fifth_step = std::make_shared<Step>(115, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_sixth_step = std::make_shared<Step>(116, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_seventh_step = std::make_shared<Step>(117, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_eighth_step = std::make_shared<Step>(118, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_nineth_step = std::make_shared<Step>(119, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_tenth_step = std::make_shared<Step>(1110, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_eleventh_step = std::make_shared<Step>(1111, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_twelfth_step = std::make_shared<Step>(1112, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_thirteenth_step = std::make_shared<Step>(1113, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_fourteenth_step = std::make_shared<Step>(1114, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_fifteenth_step = std::make_shared<Step>(1115, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_sixteenth_step = std::make_shared<Step>(1116, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_seventeenth_step = std::make_shared<Step>(1117, Step::DEFAULT_STEP);
  std::shared_ptr<Step> bis_eigthteenth_step = std::make_shared<Step>(1118, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  seq.addStep(third_step);
  seq.addStep(fourth_step);
  seq.addStep(fifth_step);
  seq.addStep(sixth_step);
  seq.addStep(seventh_step);
  seq.addStep(eighth_step);
  seq.addStep(nineth_step);
  seq.addStep(tenth_step);
  seq.addStep(eleventh_step);
  seq.addStep(twelfth_step);
  seq.addStep(thirteenth_step);
  seq.addStep(fourteenth_step);
  seq.addStep(fifteenth_step);
  seq.addStep(sixteenth_step);
  seq.addStep(seventeenth_step);
  seq.addStep(eigthteenth_step);
  seq.addStep(bis_first_step);
  seq.addStep(bis_second_step);
  seq.addStep(bis_third_step);
  seq.addStep(bis_fourth_step);
  seq.addStep(bis_fifth_step);
  seq.addStep(bis_sixth_step);
  seq.addStep(bis_seventh_step);
  seq.addStep(bis_eighth_step);
  seq.addStep(bis_nineth_step);
  seq.addStep(bis_tenth_step);
  seq.addStep(bis_eleventh_step);
  seq.addStep(bis_twelfth_step);
  seq.addStep(bis_thirteenth_step);
  seq.addStep(bis_fourteenth_step);
  seq.addStep(bis_fifteenth_step);
  seq.addStep(bis_sixteenth_step);
  seq.addStep(bis_seventeenth_step);
  seq.addStep(bis_eigthteenth_step);

  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition(
      {first_step,         third_step,         fourth_step,          fifth_step,          sixth_step,
       seventh_step,       eighth_step,        nineth_step,          tenth_step,          eleventh_step,
       twelfth_step,       thirteenth_step,    fourteenth_step,      fifteenth_step,      sixteenth_step,
       seventeenth_step,   eigthteenth_step,   bis_first_step,       bis_third_step,      bis_fourth_step,
       bis_fifth_step,     bis_sixth_step,     bis_seventh_step,     bis_eighth_step,     bis_nineth_step,
       bis_tenth_step,     bis_eleventh_step,  bis_twelfth_step,     bis_thirteenth_step, bis_fourteenth_step,
       bis_fifteenth_step, bis_sixteenth_step, bis_seventeenth_step, bis_eigthteenth_step},
      {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition(
      {second_step}, {first_step,         third_step,         fourth_step,          fifth_step,          sixth_step,
                      seventh_step,       eighth_step,        nineth_step,          tenth_step,          eleventh_step,
                      twelfth_step,       thirteenth_step,    fourteenth_step,      fifteenth_step,      sixteenth_step,
                      seventeenth_step,   eigthteenth_step,   bis_first_step,       bis_third_step,      bis_fourth_step,
                      bis_fifth_step,     bis_sixth_step,     bis_seventh_step,     bis_eighth_step,     bis_nineth_step,
                      bis_tenth_step,     bis_eleventh_step,  bis_twelfth_step,     bis_thirteenth_step, bis_fourteenth_step,
                      bis_fifteenth_step, bis_sixteenth_step, bis_seventeenth_step, bis_eigthteenth_step});
  first_step->addTransition(t2);
  third_step->addTransition(t2);
  fourth_step->addTransition(t2);
  fifth_step->addTransition(t2);
  sixth_step->addTransition(t2);
  seventh_step->addTransition(t2);
  eighth_step->addTransition(t2);
  nineth_step->addTransition(t2);
  tenth_step->addTransition(t2);
  eleventh_step->addTransition(t2);
  twelfth_step->addTransition(t2);
  thirteenth_step->addTransition(t2);
  fourteenth_step->addTransition(t2);
  fifteenth_step->addTransition(t2);
  sixteenth_step->addTransition(t2);
  seventeenth_step->addTransition(t2);
  eigthteenth_step->addTransition(t2);
  bis_first_step->addTransition(t2);
  bis_third_step->addTransition(t2);
  bis_fourth_step->addTransition(t2);
  bis_fifth_step->addTransition(t2);
  bis_sixth_step->addTransition(t2);
  bis_seventh_step->addTransition(t2);
  bis_eighth_step->addTransition(t2);
  bis_nineth_step->addTransition(t2);
  bis_tenth_step->addTransition(t2);
  bis_eleventh_step->addTransition(t2);
  bis_twelfth_step->addTransition(t2);
  bis_thirteenth_step->addTransition(t2);
  bis_fourteenth_step->addTransition(t2);
  bis_fifteenth_step->addTransition(t2);
  bis_sixteenth_step->addTransition(t2);
  bis_seventeenth_step->addTransition(t2);
  bis_eigthteenth_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);

  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  t1->setReceptivityState(true);
  t2->setReceptivityState(true);
  t3->setReceptivityState(true);
  EXPECT_THROW(seq.start(), std::logic_error);
  seq.stop();

  t1->setReceptivityState(false);
  bool exception_called = false;
  std::thread t([&seq, &exception_called]() {
    try {
      seq.start();
    } catch (const std::exception &e) {
      EXPECT_EQ(std::string(e.what()),
                "Not enough threads available to run sequence. Too big parallelism detection -> Sequence stopped !");
      exception_called = true;
    }
  });

  waitForStep(*init_step);
  t1->setReceptivityState(true);
  uint32_t watchdog_counter = 0;
  while (seq.isRunning() && watchdog_counter++ < 100) {
    std::this_thread::sleep_for(10ms); // Wait for the Crazy-parallelism detection.
  }
  EXPECT_FALSE(seq.isRunning());
  if (!seq.isRunning()) {
    t.join();
  } else {
    seq.stop();
    t.join();
  }
  EXPECT_TRUE(exception_called);
  EXPECT_EQ(seq.getStopCode(), Sequence::CRAZY_PARALLELISM_STOP);
}

TEST_F(SfcTest, Detect_And_Stop_Crazy_Looping_Sequence) {
  Sequence seq(2);
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  seq.addStep(init_step);
  seq.addStep(first_step);
  seq.addStep(second_step);
  std::shared_ptr<Transition> t1 = Transition::mk_sp_transition({first_step}, {init_step});
  init_step->addTransition(t1);
  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  std::shared_ptr<Transition> t3 = Transition::mk_sp_transition({init_step}, {second_step});
  second_step->addTransition(t3);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  first_step->addStepAction(std::make_unique<StepAction>(action_callback));
  second_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  t1->setReceptivityState(true);
  t2->setReceptivityState(true);
  t3->setReceptivityState(true);
  EXPECT_THROW(seq.start(), std::logic_error);
  seq.stop();

  t1->setReceptivityState(false);
  std::thread t([&seq]() {
    try {
      seq.start();
    } catch (const std::exception &e) {
      EXPECT_EQ(std::string(e.what()), "No more thread available to run sequence. Crazy-Looping detection -> Sequence stopped !");
    }
  });

  waitForStep(*init_step);
  EXPECT_TRUE(seq.isRunning());

  t1->setReceptivityState(true);

  uint32_t watchdog_counter = 0;
  while (seq.isRunning() && watchdog_counter++ < 1000) {
    std::this_thread::sleep_for(1ms); // Wait for the Crazy-looping detection.
  }
  EXPECT_FALSE(seq.isRunning());
  if (!seq.isRunning()) {
    t.join();
  } else {
    seq.stop();
    t.join();
  }

  EXPECT_EQ(seq.getStopCode(), Sequence::CRAZY_LOOPING_STOP);
}

TEST_F(SfcTest, Run_Unique_Sequence_With_One_Macro) {
  Sequence seq;
  seq.setTransitionPollingDelay(1);
  seq.addStepChangedCallback(&stepChanged);
  seq.addSequenceChangedCallback(&seqChanged);
  std::shared_ptr<Step> init_step = std::make_shared<Step>(0, Step::INIT_STEP);
  std::shared_ptr<Macro> macro_step = std::make_shared<Macro>(12);
  std::shared_ptr<Step> first_step = std::make_shared<Step>(1, Step::DEFAULT_STEP);
  std::shared_ptr<Step> second_step = std::make_shared<Step>(2, Step::DEFAULT_STEP);
  macro_step->addStep(first_step);
  macro_step->addStep(second_step);
  seq.addStep(init_step);
  seq.addStep(macro_step);

  std::shared_ptr<Transition> mt1 = Transition::mk_sp_transition({macro_step}, {init_step});
  init_step->addTransition(mt1);
  std::shared_ptr<Transition> mt2 = Transition::mk_sp_transition({init_step}, {macro_step});
  macro_step->addTransition(mt2);

  std::shared_ptr<Transition> t2 = Transition::mk_sp_transition({second_step}, {first_step});
  first_step->addTransition(t2);
  EXPECT_TRUE(seq.isValid());
  init_step->addStepAction(std::make_unique<StepAction>(action_callback));
  EXPECT_EQ(init_step->getActions().size(), 1);
  EXPECT_TRUE(seq.isValid());

  callback_called = false;
  std::thread t([&seq]() { seq.start(); });

  waitForStep(*init_step);        // Wait for step activation.
  waitForStep(*first_step, *mt1); // Wait for step activation.
  mt1->setReceptivityState(false);
  waitForStep(*second_step, *t2); // Wait for step activation.
  t2->setReceptivityState(false);
  waitForStep(*init_step, *mt2); // Wait for step activation.
  mt2->setReceptivityState(false);
  seq.stop();
  t.join();
  EXPECT_TRUE(callback_called);
  callback_called = false;
}

/// @todo Forbid to use "in parallel" the same macro in the same sequence ?
// TEST_F(SfcTest, Run_Unique_Sequence_With_Two_Times_The_Same_Macro) { FAIL(); }