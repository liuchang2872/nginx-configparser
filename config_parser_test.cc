#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}





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
