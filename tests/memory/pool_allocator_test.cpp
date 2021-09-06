#include "memory/pool_allocator.h"

#include <gtest/gtest.h>

TEST(PoolAllocatorTest, Complex)
{
	const size_t size = sizeof(int);
	const int value1 = 10;
	const int value2 = 20;
	const int value3 = 30;
	int *ptr1, *ptr2, *ptr3;

	scythe::PoolAllocator alloc(4);

	// Add first
	ptr1 = reinterpret_cast<int*>(alloc.Allocate(size));
	*ptr1 = value1;
	EXPECT_EQ(*ptr1, value1);

	// Add second
	ptr2 = reinterpret_cast<int*>(alloc.Allocate(size));
	*ptr2 = value2;
	EXPECT_EQ(*ptr2, value2);
	EXPECT_EQ(*ptr1, value1);

	// Remove second
	alloc.Free(ptr2);
	EXPECT_EQ(*ptr1, value1);

	// Add second and third
	ptr2 = reinterpret_cast<int*>(alloc.Allocate(size));
	*ptr2 = value2;
	ptr3 = reinterpret_cast<int*>(alloc.Allocate(size));
	*ptr3 = value3;
	EXPECT_EQ(*ptr1, value1);
	EXPECT_EQ(*ptr2, value2);
	EXPECT_EQ(*ptr3, value3);

	// Remove second
	alloc.Free(ptr2);
	EXPECT_EQ(*ptr1, value1);
	EXPECT_EQ(*ptr3, value3);
}