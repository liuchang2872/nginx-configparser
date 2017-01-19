#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

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

TEST_F(NginxConfigParseFixtureTest, nestedBracketConfig3 ){
  std::string config_string = "a {b {c;d {e;}}} f {g;}";
  std::stringstream config_stream(config_string);
  EXPECT_TRUE(parser.Parse(&config_stream, &output_config));
}

// A testcase that combines nested and mismatched brackets
TEST_F(NginxConfigParseFixtureTest, nestedMismatchedBracketConfig ){
  std::string config_string = "a {b {c;d {e;}} f {g;}";
  std::stringstream config_stream(config_string);
  EXPECT_FALSE(parser.Parse(&config_stream, &output_config));
}





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
