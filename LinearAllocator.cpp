#include "LinearAllocator.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>

LinearAllocator::LinearAllocator( size_t size )
	: size ( size )
{
	start = static_cast< byte* > ( malloc( size ) );
	memset( start, 0x00, size );

	current = start;
	end = start;
	end = start + size;
}

LinearAllocator::~LinearAllocator()
{
	free( start );
}

void* LinearAllocator::Allocate( size_t bytes, const char * fileName, int line )
{
	void* userPtr = current;
	current += bytes;

	if ( current > end )
	{
		return nullptr;
	}

	return userPtr;
}

void LinearAllocator::Free( void *ptr )
{
	// This allocator frees all its memory at once in its destructor
}

void LinearAllocator::Reset()
{
	current = start;
}

size_t LinearAllocator::GetAvailableSpace() const
{
	return ( end - current ) * sizeof( byte );
}

size_t LinearAllocator::GetAllocatedSpace() const
{
	return ( current - start ) * sizeof( byte );
}