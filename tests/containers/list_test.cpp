#include "containers/list.h"
#include "memory/pool_allocator.h"

#include <gtest/gtest.h>

class ListWithDefaultAllocatorTest : public testing::Test {
	typedef scythe::List<int> List;
protected:
	void SetUp() override
	{
		list = new List();
	}
	void TearDown() override
	{
		delete list;
	}
	List * list;
};

TEST_F(ListWithDefaultAllocatorTest, Creation)
{
	EXPECT_EQ(list->empty(), true);
}

TEST_F(ListWithDefaultAllocatorTest, PushFront)
{
	int value1 = 1;
	list->push_front(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_front(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value1);
}

TEST_F(ListWithDefaultAllocatorTest, PushBack)
{
	int value1 = 1;
	list->push_back(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_back(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value2);
}

TEST_F(ListWithDefaultAllocatorTest, PopFront)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 0U);
}

TEST_F(ListWithDefaultAllocatorTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_back();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	list->pop_back();
	EXPECT_EQ(list->size(), 0U);
}


/**
 * The same test, but with pool allocator
 */
class ListWithPoolAllocatorTest : public testing::Test {
	typedef scythe::List<int, scythe::PoolAllocator> List;
protected:
	void SetUp() override
	{
		allocator = new scythe::PoolAllocator(4);
		list = new List(allocator);
	}
	void TearDown() override
	{
		delete list;
		delete allocator;
	}
	scythe::PoolAllocator * allocator;
	List * list;
};

TEST_F(ListWithPoolAllocatorTest, Creation)
{
	EXPECT_EQ(list->empty(), true);
}

TEST_F(ListWithPoolAllocatorTest, PushFront)
{
	int value1 = 1;
	list->push_front(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_front(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value1);
}

TEST_F(ListWithPoolAllocatorTest, PushBack)
{
	int value1 = 1;
	list->push_back(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_back(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value2);
}

TEST_F(ListWithPoolAllocatorTest, PopFront)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 0U);
}

TEST_F(ListWithPoolAllocatorTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_back();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	list->pop_back();
	EXPECT_EQ(list->size(), 0U);
}