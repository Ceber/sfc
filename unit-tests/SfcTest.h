/*
 * SfcTest.h
 *
 *  Created on: 10 oct. 2021
 *      Author: ceber
 */

#ifndef SFC_UNIT_TESTS_SFCTEST_H_
#define SFC_UNIT_TESTS_SFCTEST_H_

#include <gtest/gtest.h>
#include <string>

#ifdef DEBUG_MODE
#define GTEST_COUT std::cout << green << "[   INFO   ] "
#define GTEST_ENDL reset << std::endl
#endif

class SfcTest : public ::testing::Test {
protected:
  // Sets up the stuff shared by all tests in this test suite.
  static void SetUpTestSuite() {}

  static void TearDownTestSuite() {}

  const std::string red = "\033[0;31m";
  const std::string green = "\033[0;32m";
  const std::string reset = "\033[0m";

  SfcTest() = default;
  virtual ~SfcTest() = default;

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:
  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

#endif /* SFC_UNIT_TESTS_SFCTEST_H_ */
