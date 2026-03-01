#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include "../sources/vector_utils.h"

using namespace std;

TEST(InputTest, HandleInvalidInput)
{
    istringstream input("10 abc 20 30 40");
    vector<int> vec(4);

    for(int i = 0; i<4; i++)
        while(!tryReadValue(input, vec[i])) {}

    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
    EXPECT_EQ(vec[3], 40);
}

TEST(CommandTest, FiltersNonPrintable)
{
    string input = "na\x01me";
    string result = filterPrintable(input);

    EXPECT_EQ(result, "name");
}

TEST(VectorValidationTest, RejectsZeroW)
{
    vector<int> vec = {1, 2, 3, 0};
    EXPECT_FALSE(validateWComponent(vec));
}

TEST(VectorValidationTest, AcceptsNonZeroW)
{
    vector<int> vec = {1, 2, 3, 5};
    EXPECT_TRUE(validateWComponent(vec));
}

TEST(VectorValidationTest, FloatNearZero)
{
    vector<float> vec = {1.0, 2.0, 3.0, 1e-12};
    EXPECT_FALSE(validateWComponent(vec));
}