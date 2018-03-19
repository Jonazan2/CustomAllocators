#pragma once

class DebugAllocator
{
	public:

		enum class Mode
		{
			VERBOSE,
			SILENT
		};

		DebugAllocator( Mode verbose );

		void* Allocate( size_t bytes, const char * fileName, int line );
		void Free( void *ptr );
		int GetAllocationCount() const;

	private:
		Mode mode;
		int allocations;
};