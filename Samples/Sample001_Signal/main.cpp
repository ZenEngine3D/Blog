// src\main.cpp
#include <iostream>
#include "SignalEmitter.h"

//==================================================================================================
//! @class	ClassWithSlot
//! @detail	Basic example of objects receiving signal and processing them
//==================================================================================================
class ClassWithSlot
{
public:
	ClassWithSlot(const char* _zName)
	: mzDisplayName(_zName)
	{
	}

	//----------------------------------------------------------------------------------------------
	//! @brief Connect a Lambda function to receive 'SignalA' callbacks, with 1 parameter
	//----------------------------------------------------------------------------------------------
	inline void ConnectSlotSignalA(zEmitter<int>& InEmitter)
	{		
		InEmitter.Connect(mSlotSignalA, [this](int inValue)
		{ 
			printf("\n %s  : SignalA triggered, Value=%i", mzDisplayName, inValue);
		} );		
	}

	//----------------------------------------------------------------------------------------------
	//! @brief Connect a method to receive 'SignalB' callbacks, with 2 parameters
	//----------------------------------------------------------------------------------------------
	inline void ConnectSlotSignalB(zEmitter<float,bool>& InEmitter)	
	{				
		InEmitter.Connect(mSlotSignalB, [this](float inValue, bool inDisconnect)
		{ 
			printf("\n %s  : SignalB triggered, Value=%03.1f Disconnect=%s", mzDisplayName, inValue, inDisconnect ? "True" : "False" );
			if( inDisconnect )
				mSlotSignalB.Disconnect();
		} );
	}

protected:
	const char*					mzDisplayName;
	zEmitter<int>::Slot			mSlotSignalA;	//!< Slot to receive signals from EmitterA
	zEmitter<float,bool>::Slot	mSlotSignalB;	//!< Slot to receive signals from EmitterB
};

//==================================================================================================
// Test connecting function to signal emitter
//==================================================================================================
void SlotCallbackSignalA(int inValue)
{
	printf("\n %s : SignalA triggered, Value=%i", "Function", inValue);
}

void SlotCallbackSignalB(float inValue, bool inDisconnect)
{
	(void)inDisconnect;
	printf("\n %s : SignalB triggered, Value=%03.1f Disconnect=%s", "Function", inValue, "Ignored" );
	// Disconnect is ignored. Unless we keep pointer to slot, we're unable to remove it. 
	// A Lambda can capture the slot ptr if needed, and disconnect the slot
}

//==================================================================================================
// Entry point
//==================================================================================================
int main(int, char**)
{
	printf("\n================================================================================");
	printf("\n Sample 01 : Signal and Emitters");
	printf("\n================================================================================");


	ClassWithSlot ObjectWithSlot1("Object1");
	ClassWithSlot ObjectWithSlot2("Object2");

	//----------------------------------------------------------------------------------------------
	// Basic Signal/Slot setup with Objects/Lambda/Functions. 
	// Signals have 1 int parameter
	//----------------------------------------------------------------------------------------------
	zEmitter<int>					EmitterSignalA;
	decltype(EmitterSignalA)::Slot	SlotSignalAFunction;
	decltype(EmitterSignalA)::Slot	SlotSignalALambda;
	ObjectWithSlot1.ConnectSlotSignalA(EmitterSignalA);					// Connect Object1 slot
	ObjectWithSlot2.ConnectSlotSignalA(EmitterSignalA);					// Connect Object2 slot
	EmitterSignalA.Connect(SlotSignalAFunction, SlotCallbackSignalA);	// Connect a function callback slot
	EmitterSignalA.Connect(SlotSignalALambda, [](int inValue)			// Connect a Lambda slot
	{
		printf("\n %s : SignalA triggered, Value=%i", "Lambda  ", inValue);
	});
	printf("\n\n======== Emitter A(1) ========");
	EmitterSignalA.Signal(1);
	printf("\n\n======== Emitter A(2) ========");
	EmitterSignalA.Signal(2);

	//----------------------------------------------------------------------------------------------
	// Basic Signal/Slot setup with Objects/Lambda/Functions. 
	// Signals have 1 int parameter
	//----------------------------------------------------------------------------------------------
	zEmitter<float,bool>			EmitterSignalB;
	decltype(EmitterSignalB)::Slot	SlotSignalBFunction;
	decltype(EmitterSignalB)::Slot	SlotSignalBLambda;
	ObjectWithSlot1.ConnectSlotSignalB(EmitterSignalB);													// Connect Object1 slot
	ObjectWithSlot2.ConnectSlotSignalB(EmitterSignalB);													// Connect Object2 slot
	EmitterSignalB.Connect(SlotSignalBFunction, SlotCallbackSignalB);									// Connect a function callback slot
	EmitterSignalB.Connect(SlotSignalBLambda, [&SlotSignalBLambda](float inValue, bool inDisconnect)	// Connect a Lambda slot
	{
		printf("\n %s : SignalB triggered, Value=%03.1f Disconnect=%s", "Lambda  ", inValue, inDisconnect ? "True" : "False" );
		if( inDisconnect )
			SlotSignalBLambda.Disconnect();
	});
	printf("\n\n======== Emitter B(1, Disconnect=1) ========");
	EmitterSignalB.Signal(1, true);
	//Only 1 slot will receive signal next, since we asked slots to disconnect 
	//but the 'SlotCallbackSignalB' ignore the request
	printf("\n\n======== Emitter B(2, Disconnect=0) ========");
	printf("\nNote: Only 1 slot should receive signal");
	EmitterSignalB.Signal(2, false); 

	printf("\n\n================================================================================");
	printf("\nPress 'Enter' to end");
	getchar();
}