#include "containers/array.h"

#include <gtest/gtest.h>

class ArrayTest : public testing::Test {
	typedef scythe::Array<int> Array;
protected:
	void SetUp() override
	{
		array = new Array();
	}
	void TearDown() override
	{
		delete array;
	}
	Array * array;
};

TEST_F(ArrayTest, Creation)
{
	EXPECT_EQ(array->empty(), true);
}

TEST_F(ArrayTest, PushBack)
{
	int value1 = 1;
	array->push_back(value1);
	EXPECT_EQ(array->size(), 1U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value1);

	int value2 = 2;
	array->push_back(value2);
	EXPECT_EQ(array->size(), 2U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value2);
}

TEST_F(ArrayTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	array->push_back(value1);
	array->push_back(value2);

	array->pop_back();
	EXPECT_EQ(array->size(), 1U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value1);

	array->pop_back();
	EXPECT_EQ(array->size(), 0U);
}

TEST_F(ArrayTest, Reserve)
{
	size_t capacity = 4U;
	array->reserve(capacity);
	EXPECT_EQ(array->size(), 0U);
	EXPECT_EQ(array->capacity(), capacity);
}

TEST_F(ArrayTest, Resize)
{
	size_t size = 4U;
	array->resize(size);
	EXPECT_EQ(array->size(), size);
	EXPECT_GE(array->capacity(), size);
}