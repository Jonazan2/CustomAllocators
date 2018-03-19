#include "DebugAllocator.h"

#include <iostream>

DebugAllocator::DebugAllocator( Mode mode )
	: mode ( mode ), allocations ( 0 ) {}

void* DebugAllocator::Allocate( size_t bytes, const char * fileName, int line )
{
	void* chunck = malloc( bytes );
	allocations++;
	if ( mode == Mode::VERBOSE )
	{
		std::cout << "Allocating: " << bytes << " bytes with address: " << chunck << " in file: " << fileName << " at line: " << line << "\n";
	}
	return chunck;
}

void DebugAllocator::Free( void *ptr )
{
	if ( mode == Mode::VERBOSE )
	{
		std::cout << "Deallocating: " << ptr <<"\n";
	}
	free( ptr );
	allocations--;
}

int DebugAllocator::GetAllocationCount() const
{
	return allocations;
}