#include "catch.hpp"
#include "nlohmann/json.hpp"
#include "inja.hpp"



TEST_CASE("Basic search in string") {
	std::string input = "lorem ipsum dolor it";
	inja::Regex regex("i(.*)m");

	SECTION("Basic search from start") {
		inja::Match match = inja::search(input, regex, 0);
		CHECK( match.found() == true );
		CHECK( match.position() == 6 );
		CHECK( match.length() == 5 );
		CHECK( match.end_position() == 11 );
		CHECK( match.str() == "ipsum" );
		CHECK( match.str(1) == "psu" );
	}

	SECTION("Basic search from position") {
		inja::Match match = inja::search(input, regex, 8);
		CHECK( match.found() == false );
		CHECK( match.length() == 0 );
	}
}

TEST_CASE("Search in string with multiple possible regexes") {
	std::string input = "lorem ipsum dolor amit estas tronum.";

	SECTION("Basic 1") {
		std::vector<inja::Regex> regex_patterns = { inja::Regex("tras"), inja::Regex("do(\\w*)or"), inja::Regex("es(\\w*)as"), inja::Regex("ip(\\w*)um") };
		inja::Match match = inja::search(input, regex_patterns, 0);
		CHECK( match.regex_number() == 3 );
		CHECK( match.str() == "ipsum" );
		CHECK( match.str(1) == "s" );
	}

	SECTION("Basic 2") {
		std::vector<inja::Regex> regex_patterns = { inja::Regex("tras"), inja::Regex("ip(\\w*)um"), inja::Regex("do(\\w*)or"), inja::Regex("es(\\w*)as") };
		inja::Match match = inja::search(input, regex_patterns, 0);
		CHECK( match.regex_number() == 1 );
		CHECK( match.str() == "ipsum" );
		CHECK( match.str(1) == "s" );
	}
}

TEST_CASE("Search on level") {
	std::string input = "(% up %)(% up %)Test(% N1 %)(% down %)...(% up %)(% N2 %)(% up %)(% N3 %)(% down %)(% N4 %)(% down %)(% N5 %)(% down %)";

	inja::Regex regex_statement("\\(\\% (.*?) \\%\\)");
	inja::Regex regex_level_up("up");
	inja::Regex regex_level_down("down");
	inja::Regex regex_search("N(\\d+)");

	SECTION("First instance") {
		inja::Match open_match = inja::search(input, regex_statement, 0);
		CHECK( open_match.position() == 0 );
		CHECK( open_match.end_position() == 8 );
		CHECK( open_match.str(1) == "up" );

		inja::MatchClosed match = inja::search_closed_on_level(input, regex_statement, regex_level_up, regex_level_down, regex_search, open_match);
		CHECK( match.position() == 0 );
		CHECK( match.end_position() == 109 );
	}

	SECTION("Second instance") {
		inja::Match open_match = inja::search(input, regex_statement, 4);

		CHECK( open_match.position() == 8 );
		CHECK( open_match.end_position() == 16 );
		CHECK( open_match.str(1) == "up" );

		inja::MatchClosed match = inja::search_closed_on_level(input, regex_statement, regex_level_up, regex_level_down, regex_search, open_match);

		CHECK( match.open_match.position() == 8 );
		CHECK( match.open_match.end_position() == 16 );
		CHECK( match.close_match.position() == 20 );
		CHECK( match.close_match.end_position() == 28 );
		CHECK( match.position() == 8 );
		CHECK( match.end_position() == 28 );
		CHECK( match.outer() == "(% up %)Test(% N1 %)" );
		CHECK( match.inner() == "Test" );
	}
}
