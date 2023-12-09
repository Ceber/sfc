#pragma once

#include <sfc/step/action/StepAction.hpp>

class DummyStepAction : public StepAction {
private:
  int counter = 0;

public:
  DummyStepAction() : StepAction([=]() { counter++; }) {}
  ~DummyStepAction() = default;
};
