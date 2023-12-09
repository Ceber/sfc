# Sequential Function Chart Library – C++ 2nd party library: based on standard library

![Workflow](https://github.com/Ceber/sfc/actions/workflows/cmake-single-platform.yml/badge.svg)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/ceber68)

## Library to handle sequence running composed by steps and transitions. Each step can trigger several actions.
The main purpose is to interpret "grafcet" sequences. It is not forseen for "production use" (even if it could be enough).
It is used for sequences developpment, in collaboration with another library that enable to generate C++ code to "hard-code" sequences, that give more determinism and enable "soft real-time".

## We distinguish three kinds of sequences, and all possible combinations that respect the chart constraints:
- Unique Sequence
- Simultaneous Sequence
- Exclusive Sequence

 ## Availibilities:
- Run "interpreted sequences", Grafcet like.
- Macro: Reusable sequence subset.
- Check if sequences are crazy: Looping / Too Much Parallelism
- Configurable threading (thread_pool sizing).
- Sequence consistency checks.

## About:
- This library is not meant to be able to externally wait on step activation (We get an event notification when a step is activated/deactivated)
- This implementation use a thread_pool whose default threads count is the current ‘hardware thread contexts’ count of the machine (16 on my device)
- Each step run in its own thread (So only 16 steps can run simultaneously on my machine).

## Known Issues:
- Thread Sanitizer is crying blood. I think it is mainly because of the way we wait on steps via "sleeps" inside the unit-tests...

## TODO
It is probably far to be exhaustive. Feel free to help.
