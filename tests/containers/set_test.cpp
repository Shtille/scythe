#include "containers/set.h"
#include "memory/test_allocator.h"

#include <gtest/gtest.h>

class SetTest : public testing::Test {
	typedef scythe::Set<int, scythe::TestAllocator> Set;
protected:
	void SetUp() override
	{
		allocator = new scythe::TestAllocator();
		set = new Set(allocator);
		initial_allocated = allocator->allocated();
	}
	void TearDown() override
	{
		delete set;
		delete allocator;
	}
	scythe::TestAllocator * allocator;
	Set * set;
	int initial_allocated;
};

TEST_F(SetTest, Creation)
{
	EXPECT_EQ(set->empty(), true);
	EXPECT_EQ(set->begin(), set->end());
}

TEST_F(SetTest, Insert)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		auto pair = set->insert(value);
		EXPECT_EQ(*pair.first, value);
		EXPECT_EQ(pair.second, true);
		EXPECT_EQ(set->size(), static_cast<size_t>(i+1));
		EXPECT_EQ(allocator->allocated(), initial_allocated + i + 1);
	}
	// Then check the values
	i = 0;
	for (auto it = set->begin(); it != set->end(); ++it)
	{
		int value = values[i];
		EXPECT_EQ(*it, value);
		++i;
	}
	// Try to insert existing value
	{
		int existing_value = 1;
		auto pair = set->insert(existing_value);
		EXPECT_EQ(*pair.first, existing_value);
		EXPECT_EQ(pair.second, false);
		EXPECT_EQ(set->size(), static_cast<size_t>(count));
		EXPECT_EQ(allocator->allocated(), initial_allocated + count);
	}
}

TEST_F(SetTest, Find)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->insert(value);
	}
	// Then try to find an existing value
	{
		int existing_value = 2;
		auto it = set->find(existing_value);
		EXPECT_NE(it, set->end());
		EXPECT_EQ(*it, existing_value);
	}
	// Then try to find a non existing value
	{
		int non_existing_value = 5;
		auto it = set->find(non_existing_value);
		EXPECT_EQ(it, set->end());
	}
}

TEST_F(SetTest, Erase)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->trusted_insert(value);
	}
	// Then erase an existing value
	{
		int existing_value = 2;
		auto it = set->find(existing_value);
		set->erase(it);
		EXPECT_EQ(set->size(), static_cast<size_t>(count - 1));
		EXPECT_EQ(allocator->allocated(), initial_allocated + count - 1);
	}
	// Then check the rest values
	{
		auto it = set->begin();
		EXPECT_EQ(*it, values[0]);
		++it;
		EXPECT_EQ(*it, values[2]);
	}
}

TEST_F(SetTest, Clear)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->trusted_insert(value);
	}
	// Then clear the set
	{
		set->clear();
		EXPECT_EQ(set->empty(), true);
		EXPECT_EQ(set->begin(), set->end());
		EXPECT_EQ(allocator->allocated(), initial_allocated);
	}
}