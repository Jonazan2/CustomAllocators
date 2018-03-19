#pragma once

class LinearAllocator
{
	using byte = unsigned char;

	public:
		LinearAllocator( size_t size );
		~LinearAllocator();

		void* Allocate( size_t bytes, const char * fileName, int line );
		void Free( void *ptr );
		void Reset();

		size_t GetAvailableSpace() const;
		size_t GetAllocatedSpace() const;

	private:
		size_t size;
		byte *start;
		byte *current;
		byte *end;
};