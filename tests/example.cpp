/*
 * Copyright (c) 2020 seongwoo chae <mhs4670go@naver.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "arser.h"

using namespace arser;

class Prompt {
 public:
  Prompt(const std::string &command)
  {
    std::istringstream iss(command);
    std::vector<std::string> token(std::istream_iterator<std::string>{iss},
                                   std::istream_iterator<std::string>());
    _arg = std::move(token);
    _argv.reserve(_arg.size());
    for (const auto &t : _arg) {
      _argv.push_back(const_cast<char *>(t.data()));
    }
  }
  int argc(void) const { return _argv.size(); }
  char **argv(void) { return _argv.data(); }

 private:
  std::vector<char *> _argv;
  std::vector<std::string> _arg;
};

TEST(BasicTest, option)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--verbose")
      .nargs(0)
      .help(
          "It provides additional details as to what the executable is doing");

  Prompt prompt("./executable --verbose");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--verbose"]);
  EXPECT_TRUE(arser.get<bool>("--verbose"));
}

TEST(BasicTest, OptionalArgument)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--volume")
      .nargs(1)
      .type(arser::DataType::INT32)
      .help("Set a volume as you provided.");

  Prompt prompt("./radio --volume 5");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--volume"]);
  EXPECT_EQ(5, arser.get<int>("--volume"));

  EXPECT_FALSE(arser["--price"]);
  EXPECT_THROW(arser.get<bool>("--volume"), std::runtime_error);
}

TEST(BasicTest, NonRequiredOptionalArgument)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--weight")
      .nargs(1)
      .type(arser::DataType::INT32)
      .help("Set a volume as you provided.");

  Prompt prompt("./radio");  // empty argument
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_FALSE(arser["--volume"]);
  EXPECT_THROW(arser.get<int>("--weight"), std::runtime_error);
}

TEST(BasicTest, RequiredOptionalArgument)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--volume")
      .nargs(1)
      .type(arser::DataType::INT32)
      .required()
      .help("Set a volume as you provided.");

  Prompt prompt("./radio");
  /* act */ /* assert */
  EXPECT_THROW(arser.parse(prompt.argc(), prompt.argv()), std::runtime_error);
}

TEST(BasicTest, OptionalMultipleArgument)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--add")
      .nargs(2)
      .type(arser::DataType::INT32_VEC)
      .help("Add two numbers.");

  Prompt prompt("./calculator --add 3 5");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--add"]);
  std::vector<int> values = arser.get<std::vector<int>>("--add");
  EXPECT_EQ(3, values.at(0));
  EXPECT_EQ(5, values.at(1));

  EXPECT_THROW(arser.get<std::vector<float>>("--add"), std::runtime_error);
}
