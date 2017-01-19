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




//Edit config_parser_test.cc in your repository.
// You will note the test file already has a passing example. At least one unit test should fail due to bugs in the code you are testing.
// Don't modify config_parser.cc until you identify the bugs -- it is ALREADY buggy.
// Also, there is probably no need to review the code you're testing -- just write unit tests until you find a failure.
// You may be able to find some Nginx config docs or web pages on the internet.


//Test fixture 
class NginxConfigParseFixtureTest : public ::testing::Test {
protected:
    bool parseString(const std::string config_string) {
        std::stringstream config_stream(config_string);
        return parser.Parse(&config_stream, &output_config);
    }

    NginxConfigParser parser;
    NginxConfig output_config;
};

TEST_F(NginxConfigParseFixtureTest, ValidString){
	EXPECT_TRUE(parseString("foo    bar;" )); //multiple spaces are interpreted as a single space
	EXPECT_TRUE(parseString("foo	bar;" )); // tabs are interpreted as a single white space
	EXPECT_TRUE(parseString("foo bar; #a comment")); //All lines preceded by a pound sign or hash (#) are comments. 
    output_config.statements_.clear();


	EXPECT_TRUE(parseString("foo  \n { foo bar ; \n \n foo bar;} "));
	EXPECT_EQ(output_config.statements_.size(), 1); 
	EXPECT_EQ(output_config.statements_[0]->tokens_[0], "foo");
//	std::cout<<output_config.statements_[0]->tokens_[1];
}

TEST_F(NginxConfigParseFixtureTest, Non_ValidString){
	EXPECT_FALSE(parseString("foo bar"));  //Settings without ";" at the end
	EXPECT_FALSE(parseString("foo \" bar \';"));//mismatch quotes
	EXPECT_FALSE(parseString("foo  \'bar\";")); //mismatch quotes
	EXPECT_FALSE(parseString("foo foo bar;}"));  // mismatch curly braces
	EXPECT_FALSE(parseString("foo {foo bar;"));  // mismatch curly braces
	EXPECT_FALSE(parseString("foo {{foo bar;}"));  // mismatch curly braces
	EXPECT_FALSE(parseString("foo {foo bar;}}"));  // mismatch curly braces
}
