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

TEST(BasicTest, MultipleOptionalArgument)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--input_path")
      .nargs(1)
      .type(arser::DataType::STR)
      .help("input path of this program.")
      .required();
  arser.add_argument("--output_path")
      .nargs(1)
      .type(arser::DataType::STR)
      .help("output path of this program.")
      .required(true);
  arser.add_argument("--training_data")
      .nargs(5)
      .type(arser::DataType::INT32_VEC)
      .help("give traning data to this program.")
      .required();

  Prompt prompt(
      "./ml --input_path /I/am/in.put --output_path I/am/out.put "
      "--training_data 2 43 234 3 334");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--input_path"]);
  EXPECT_EQ("/I/am/in.put", arser.get<std::string>("--input_path"));
  EXPECT_TRUE(arser["--output_path"]);
  EXPECT_EQ("I/am/out.put", arser.get<std::string>("--output_path"));
  EXPECT_TRUE(arser["--training_data"]);
  std::vector<int32_t> data =
      arser.get<std::vector<int32_t>>("--training_data");
  EXPECT_EQ(2, data.at(0));
  EXPECT_EQ(43, data.at(1));
  EXPECT_EQ(234, data.at(2));
  EXPECT_EQ(3, data.at(3));
  EXPECT_EQ(334, data.at(4));
}

TEST(BasicTest, MultipleFloatValue)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--add_float")
      .nargs(2)
      .type(arser::DataType::FLOAT_VEC)
      .help("Add two float numbers.");

  Prompt prompt("./calculator --add_float 3.2 5.4");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--add_float"]);
  std::vector<float> values = arser.get<std::vector<float>>("--add_float");
  EXPECT_FLOAT_EQ(3.2, values.at(0));
  EXPECT_FLOAT_EQ(5.4, values.at(1));

  EXPECT_THROW(arser.get<std::vector<int>>("--add_float"), std::runtime_error);
}

TEST(BasicTest, MultipleStringValue)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--three_color")
      .nargs(3)
      .type(arser::DataType::STR_VEC)
      .help("insert your three favorite color");

  Prompt prompt("./color_factory --three_color red blue yellow");
  /* act */
  arser.parse(prompt.argc(), prompt.argv());
  /* assert */
  EXPECT_TRUE(arser["--three_color"]);
  std::vector<std::string> values =
      arser.get<std::vector<std::string>>("--three_color");
  EXPECT_EQ("red", values.at(0));
  EXPECT_EQ("blue", values.at(1));
  EXPECT_EQ("yellow", values.at(2));

  EXPECT_THROW(arser.get<std::vector<std::string>>("--color"),
               std::runtime_error);
}

void printBiography(void) { std::cerr << "When I was young.." << std::endl; }

TEST(BasicTest, ExitWithFunctionCall)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--history")
      .help("Show history and exit")
      .exit_with(printBiography);

  arser.add_argument("--name")
      .nargs(1)
      .type(arser::DataType::STR)
      .help("Name your hero");

  Prompt prompt("./hero --history");
  /* act */ /* assert */
  EXPECT_EXIT(arser.parse(prompt.argc(), prompt.argv()),
              testing::ExitedWithCode(0), "When I was young..");
}

void printVersion(std::string version)
{
  std::cerr << "arser version : " << version << std::endl;
}

TEST(BasicTest, ExitWithFunctionCallWithBind)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--version")
      .help("Show version and exit")
      .exit_with(std::bind(printVersion, "1.2.0"));

  Prompt prompt("./arser --version");
  /* act */ /* assert */
  EXPECT_EXIT(arser.parse(prompt.argc(), prompt.argv()),
              testing::ExitedWithCode(0), "arser version : 1.2.0");
}

TEST(BasicTest, ExitWithFunctionCallWithLamda)
{
  /* arrange */
  Arser arser;

  arser.add_argument("--shutdown")
      .help("Shut down your computer")
      .exit_with([](void) { std::cerr << "Good bye.." << std::endl; });

  arser.add_argument("OS")
      .nargs(1)
      .type(arser::DataType::STR)
      .help("The OS you want to boot");

  Prompt prompt("./computer --shutdown");
  /* act */ /* assert */
  EXPECT_EXIT(arser.parse(prompt.argc(), prompt.argv()),
              testing::ExitedWithCode(0), "Good bye..");
}
