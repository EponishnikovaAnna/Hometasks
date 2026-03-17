#pragma once
#include <vector>
#include <string>

using namespace std;

class TestBase
{
public: 
	virtual bool test(const vector<string>& params) = 0;

	virtual ~TestBase() = default;

};