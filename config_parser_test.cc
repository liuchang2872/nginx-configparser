#include "gtest/gtest.h"
#include "config_parser.h"

// test NginxConfigStatement ToString
TEST(NginxCongigTest, ToString){
  NginxConfigStatement statement;
  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");
  EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

// Fixture
class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}


// Known bug
TEST_F(NginxConfigParserTest, missMatchBrakets){
  std::string config_string = "foo { bar; ";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, missingSemicolon){
  std::string config_string = "foo bar ";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, multiStatement){
  std::string config_string = "foo; bar;";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, noToken ){
  std::string config_string = ";";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, noTokenRecursive ){
  std::string config_string = "a {;}";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, noSemicolonRecursive ){
  std::string config_string = "a {b}";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, configAfterBracket ){
  std::string config_string = "a {b;} c;";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, manyStatementOneLine ){
  std::string config_string = "a {b; d; e; f;} c; a b c {d; t;}";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

// Should empty config be allowed in brackets?
TEST_F(NginxConfigParserTest, emptyConfig ){
  std::string config_string = "a {}";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}

// The next three test cases deal with nested brackets
// The original code is not working for nested brackets
TEST_F(NginxConfigParserTest, nestedBracketConfig1 ){
  std::string config_string = "a {b {c;}}";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, nestedBracketConfig2 ){
  std::string config_string = "a {b {c;d {e;}}}";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

TEST_F(NginxConfigParserTest, nestedBracketConfig3 ){
  std::string config_string = "a {b {c;d {e;}}} f {g;}";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &out_config));
}

// A testcase that combines nested and mismatched brackets
TEST_F(NginxConfigParserTest, nestedMismatchedBracketConfig ){
  std::string config_string = "a {b {c;d {e;}} f {g;}";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &out_config));
}
