/*
 * main.cpp
 *
 *  Created on: 10 oct. 2021
 *      Author: ceber
 */

#include "sfc/SfcTests.h"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
