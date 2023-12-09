#include "sfc/transition/Receptivity.hpp"

Receptivity::Receptivity() : m_state(false) {}

bool Receptivity::state() { return m_state.load(); }

void Receptivity::setState(bool state) { m_state.store(state); }