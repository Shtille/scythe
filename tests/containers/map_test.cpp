#include "containers/map.h"
#include "memory/test_allocator.h"

#include <gtest/gtest.h>

class MapTest : public testing::Test {
	typedef scythe::Map<int, int, scythe::TestAllocator> Map;
protected:
	typedef std::pair<int, int> Pair;

	void SetUp() override
	{
		allocator = new scythe::TestAllocator();
		map = new Map(allocator);
		initial_allocated = allocator->allocated();
	}
	void TearDown() override
	{
		delete map;
		delete allocator;
	}
	scythe::TestAllocator * allocator;
	Map * map;
	int initial_allocated;
};

TEST_F(MapTest, Creation)
{
	EXPECT_EQ(map->empty(), true);
	EXPECT_EQ(map->begin(), map->end());
}

TEST_F(MapTest, Insert)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		Pair value_pair(value, value);
		auto pair = map->insert(value_pair);
		EXPECT_EQ(*pair.first, value_pair);
		EXPECT_EQ(pair.second, true);
		EXPECT_EQ(map->size(), static_cast<size_t>(i+1));
		EXPECT_EQ(allocator->allocated(), initial_allocated + i + 1);
	}
	// Then check the values
	i = 0;
	for (auto it = map->begin(); it != map->end(); ++it)
	{
		int value = values[i];
		Pair value_pair(value, value);
		EXPECT_EQ(*it, value_pair);
		++i;
	}
	// Try to insert existing value
	{
		int existing_value = 1;
		Pair value_pair(existing_value, existing_value);
		auto pair = map->insert(value_pair);
		EXPECT_EQ(*pair.first, value_pair);
		EXPECT_EQ(pair.second, false);
		EXPECT_EQ(map->size(), static_cast<size_t>(count));
		EXPECT_EQ(allocator->allocated(), initial_allocated + count);
	}
}

TEST_F(MapTest, Find)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		Pair value_pair(value, value);
		(void)map->insert(value_pair);
	}
	// Then try to find an existing value
	{
		int existing_value = 2;
		Pair existing_value_pair(existing_value, existing_value);
		auto it = map->find(existing_value);
		EXPECT_NE(it, map->end());
		EXPECT_EQ(*it, existing_value_pair);
	}
	// Then try to find a non existing value
	{
		int non_existing_value = 5;
		auto it = map->find(non_existing_value);
		EXPECT_EQ(it, map->end());
	}
}

TEST_F(MapTest, Erase)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		Pair value_pair(value, value);
		(void)map->trusted_insert(value_pair);
	}
	// Then erase an existing value
	{
		int existing_value = 2;
		auto it = map->find(existing_value);
		map->erase(it);
		EXPECT_EQ(map->size(), static_cast<size_t>(count - 1));
		EXPECT_EQ(allocator->allocated(), initial_allocated + count - 1);
	}
	// Then check the rest values
	{
		auto it = map->begin();
		EXPECT_EQ((*it).first, values[0]);
		++it;
		EXPECT_EQ((*it).first, values[2]);
	}
}

TEST_F(MapTest, Clear)
{
	int i;
	int values[] = {1,2,3};
	const int count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		Pair value_pair(value, value);
		(void)map->trusted_insert(value_pair);
	}
	// Then clear the map
	{
		map->clear();
		EXPECT_EQ(map->empty(), true);
		EXPECT_EQ(map->begin(), map->end());
		EXPECT_EQ(allocator->allocated(), initial_allocated);
	}
}