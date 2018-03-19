#include <iostream>

#include "DebugAllocator.h"
#include "LinearAllocator.h"

#define NEW( T, arena ) new ( arena.Allocate( sizeof( T ), __FILE__, __LINE__ ) ) T
#define DELETE( object, arena ) Delete( object, arena )
#define NEW_ARRAY( T, size, arena ) NewArray<T> ( size, arena, __FILE__, __LINE__ )
#define DELETE_ARRAY( object, arena ) DeleteArray( object, arena )

template < typename T, class Arena >
void Delete( T* object, Arena &arena )
{
	object->~T();
	arena.Free( object );
}

template < typename T, class Arena>
T* NewArray ( size_t count, Arena &arena, const char * fileName, int line  )
{
	// Calculate bytes to allocate and allocate them
	size_t necessaryBytes = ( sizeof( T ) * count );
	if ( !std::is_pod< T >::value )
	{
		necessaryBytes += sizeof( size_t );
	}

	if ( std::is_pod< T >::value )
	{
		void *ptr = arena.Allocate( necessaryBytes, fileName, line );
		return static_cast< T* >( ptr );
	}
	else
	{
		union 
		{
			void *ptr;
			size_t *size;
			T *elements;
		};

		ptr = arena.Allocate( necessaryBytes, fileName, line );

		// Set the first byte of the ptr with the amount of elements (will be used later to determinate how
		// many destructors need to be called by DELETE_ARRAY
		*size = count;
		size++;

		// Call constructors using placement new
		const T * const finalAddress = elements + count;
		while ( elements < finalAddress )
		{
			new ( elements ) T;
			elements++;
		}

		// Return the pointer to the allocated chunk + 1 since the first address contains the number of elements
		// and it's hidden to the caller
		return ( elements - count );
	}
}

template < typename T, class Arena >
void DeleteArray ( T *object, Arena &arena )
{
	if ( std::is_pod<T>::value )
	{
		arena.Free( object );
	}
	else
	{
		union
		{
			size_t *size;
			T *ptr;
		};

		ptr = object;
		
		// Check the size in the first address (which is one before the given to the user)
		const size_t elementCount = size[ -1 ];

		for ( size_t i = elementCount; i > 0; --i )
		{
			ptr[ i - 1 ].~T();
		}
		arena.Free( size - 1 );
	}
}

class A
{
	public:
		A() : a(1), b (1) {}
		A( int a, int b ) : a( a ), b ( b ) {}
		A( const A &rhs ) {}
		A( int value ) : a( value ), b( value ) {}
		~A() { std::cout << "Destroying A\n"; }

	private:
		int a;
		int b;
};

class B
{
	public:
		B() : a(1), b (1) {}
		B( int a, int b ) : a( a ), b ( b ), c ('s') {}
		B( const A &rhs ) {}
		B( int value ) : a( value ), b( value ), c ('s') {}
		~B() { std::cout << "Destroying B\n"; }

	private:
		short x;
		char c;
		int a;
		int b;
};


int main( int argc, char** argv )
{
	// There is no need to free this memory since it will be free when linear allocator destructor gets called
	LinearAllocator linearAllocator(64);
	A *a = NEW( A, linearAllocator )( 0xCDCDCDCD, 0xAAAAAAAA );
	std::cout<< "Linear allocator space in bytes: " << linearAllocator.GetAvailableSpace() << "\n";
	std::cout<< "Linear allocator allocated in bytes: " << linearAllocator.GetAllocatedSpace() << "\n";

	B *b = NEW( B, linearAllocator )( 0x11223344, 0x55667788 );
	std::cout << "Alignment of B: " << std::alignment_of< B >::value << "\n";
	std::cout << "Linear allocator space in bytes: " << linearAllocator.GetAvailableSpace() << "\n";
	std::cout << "Linear allocator allocated in bytes: " << linearAllocator.GetAllocatedSpace() << "\n";


	DebugAllocator allocator( DebugAllocator::Mode::SILENT );

	A *object = NEW( A, allocator )( 1 );
	DELETE( object, allocator );

	int *pods = NEW_ARRAY( int, 3, allocator );
	DELETE_ARRAY( pods, allocator );
	A *aes = NEW_ARRAY( A, 3, allocator );
	DELETE_ARRAY( aes, allocator );

	std::cout<< "Final allocation count: " << allocator.GetAllocationCount() << std::endl;
}