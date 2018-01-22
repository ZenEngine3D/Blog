#include <iostream>
#include "SignalEmitter.h"
#include <chrono>
#include <assert.h>
#include <array>

#if defined(_MSC_VER)
	#define NoInline __declspec(noinline)
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
	#define NoInline __attribute__((noinline)) 
#else
	#define NoInline
#endif

const unsigned int kLoopCount = 1000000;	// Number of loop to do per test
const unsigned int kIteration = 10;			// Number of time to re-do test, averaging result to improve reliability

NoInline void FunctionSumCallback(int InValue, int& InSumResult)
{
	InSumResult += InValue;	
}

struct ListFunctionPtr : public eastl:: intrusive_list_node
{
	void (*mpFonctionCallback)(int,int&) = FunctionSumCallback;
};

struct ListFunctor : public eastl::intrusive_list_node
{
	std::function<void(int, int&)> mFunctorCallback = FunctionSumCallback;
};

inline __int64 GetElapsedTimeUs(const std::chrono::high_resolution_clock::time_point& InStartTime)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - InStartTime).count();
}

void SamplePerformances()
{
	printf("\n");
	printf("\n============================================================");
	printf("\n Performances evaluation");
	printf("\n (%i Iterations, averaged over %i times)", kLoopCount, kIteration);
	printf("\n============================================================");
	
	__int64 ElapsedDirect(0), ElapsedPointer(0), ElapsedFunctorFunction(0), ElapsedFunctorLambda(0);
	__int64 ElapsedListPointer(0), ElapsedListFunctor(0), ElapsedEmitter(0), ElapsedEmitterLambda(0);

	for(int idx(0); idx<kIteration; ++idx)
	{
		// Direct Function (made sure no inlining)
		{
			auto TimeStart		= std::chrono::high_resolution_clock::now();
			int Sum				= 0;
			for( int i(0); i<kLoopCount; ++i )
			{
				FunctionSumCallback(1, Sum);
			}
			ElapsedDirect		+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);		
		}
		// Function Pointer
		{
			void (*pCallback)(int,int&)	= FunctionSumCallback;
			auto TimeStart	= std::chrono::high_resolution_clock::now();
			int Sum			= 0;
			for( int i(0); i<kLoopCount; ++i )
			{
				pCallback(1, Sum);
			}
			ElapsedPointer		+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);		
		}
		// Functor with function call
		{
			std::function<void(int, int&)> FunctorCallback = FunctionSumCallback;
			auto TimeStart	= std::chrono::high_resolution_clock::now();
			int Sum			= 0;
			for( int i(0); i<kLoopCount; ++i )
			{
				FunctorCallback(1, Sum);
			}
			ElapsedFunctorFunction	+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);		
		}
		// Functor with Lambda
		{
			std::function<void(int, int&)> FunctorCallback = [](int InValue,int& InSumResult){ InSumResult += InValue;};
			auto TimeStart	= std::chrono::high_resolution_clock::now();
			int Sum			= 0;
			for( int i(0); i<kLoopCount; ++i )
			{
				FunctorCallback(1, Sum);
			}
			ElapsedFunctorLambda += GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);		
		}


		// List of Function Pointer
		{
			eastl::intrusive_list<ListFunctionPtr>	ListCallback;
			std::array<ListFunctionPtr, 10>			ArrayCallback;
			for( auto& item : ArrayCallback)
				ListCallback.push_back( item );

			auto TimeStart	= std::chrono::high_resolution_clock::now();
			int Sum			= 0;
			for( int i(0); i<kLoopCount; i+= (int)ArrayCallback.size())
			{
				auto it = ListCallback.begin();
				while( it != ListCallback.end() )
				{
					it->mpFonctionCallback(1,Sum);
					++it;
				}
			}
			ElapsedListPointer	+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);		
		}
		// List of Functor Function
		{
			eastl::intrusive_list<ListFunctor>	ListCallback;
			std::array<ListFunctor, 10>			ArrayCallback;
			for( auto& item : ArrayCallback)
				ListCallback.push_back( item );

			auto TimeStart		= std::chrono::high_resolution_clock::now();
			int Sum				= 0;
			for( int i(0); i<kLoopCount; i+= (int)ArrayCallback.size())
			{
				auto it = ListCallback.begin();
				while( it != ListCallback.end() )
				{
					it->mFunctorCallback(1,Sum);
					++it;
				}
			}
			ElapsedListFunctor	+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);
		
		}
		// Signal/Slot Emitter with Function
		{
			zEmitter<int, int&>						Emitter;
			std::array<decltype(Emitter)::Slot, 10>	ArraySlot;
			for( auto& SlotItem : ArraySlot)
				SlotItem.Connect(Emitter, FunctionSumCallback);

			auto TimeStart				= std::chrono::high_resolution_clock::now();
			int Sum						= 0;
			for( int i(0); i<kLoopCount; i+= (int)ArraySlot.size())
				Emitter.Signal(1, Sum);
			
			ElapsedEmitter				+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);
		}
		// Signal/Slot Emitter with Lambda
		{
			zEmitter<int, int&>						Emitter;
			std::array<decltype(Emitter)::Slot, 10>	ArraySlot;
			for( auto& SlotItem : ArraySlot)
				SlotItem.Connect( Emitter, [](int InValue,int& InSumResult){ InSumResult += InValue;} );

			auto TimeStart				= std::chrono::high_resolution_clock::now();
			int Sum						= 0;
			for( int i(0); i<kLoopCount; i+= (int)ArraySlot.size())
				Emitter.Signal(1, Sum);

			ElapsedEmitterLambda		+= GetElapsedTimeUs(TimeStart);
			assert(Sum == kLoopCount);
		}
	}
	printf("\n Direct Function              : Time %05.02fms", ElapsedDirect/kIteration/1000.f);
	printf("\n Function Pointer             : Time %05.02fms", ElapsedPointer/kIteration/1000.f);
	printf("\n std::Functor (Function)      : Time %05.02fms", ElapsedFunctorFunction/kIteration/1000.f);
	printf("\n std::Functor (Lambda)        : Time %05.02fms", ElapsedFunctorLambda/kIteration/1000.f);
	printf("\n List Function Pointer        : Time %05.02fms", ElapsedListPointer/kIteration/1000.f);
	printf("\n List std::Functor (function) : Time %05.02fms", ElapsedListFunctor/kIteration/1000.f);
	printf("\n Signal (Function)            : Time %05.02fms", ElapsedEmitter/kIteration/1000.f);
	printf("\n Signal (Lambda)              : Time %05.02fms", ElapsedEmitterLambda/kIteration/1000.f);
}