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
	//! @brief Connect a Lambda function to receive 'SignalA' callbacks (1 parameter)
	//----------------------------------------------------------------------------------------------
	inline void ConnectSlotSignalA(zEmitter<int>& InEmitter)
	{		
		// Connect the Slot to a new Lambda having full access to this object. 
		// This way, 'ConnectSlotSignalA' cleanly encapsulate all needed code to process the signal
		mSlotSignalA.Connect(InEmitter, [this](int inValue)
		{ 
			printf("\n %s  : SignalA triggered, Value=%i", mzDisplayName, inValue);
		} );		
	}

	//----------------------------------------------------------------------------------------------
	//! @brief Connect a method to receive 'SignalB' callbacks (2 parameters)
	//----------------------------------------------------------------------------------------------
	inline void ConnectSlotSignalB(zEmitter<float,bool>& InEmitter)	
	{		
		// Here we connect the Slot to a method of this class. 
		// std:bind will create a functor that can invoke this object 'SlotSignalB' method
		mSlotSignalB.Connect(InEmitter, std::bind(&ClassWithSlot::SlotSignalB, this, std::placeholders::_1, std::placeholders::_2));
	}

protected:
	//----------------------------------------------------------------------------------------------
	//! @brief This method is invoked with 'SignalB', since it has been bound to the slot in 'ConnectSlotSignalB'
	//----------------------------------------------------------------------------------------------
	void SlotSignalB(float inValue, bool inDisconnect)
	{
		printf("\n %s  : SignalB triggered, Value=%03.1f Disconnect=%s", mzDisplayName, inValue, inDisconnect ? "True" : "False" );
		if( inDisconnect )
			mSlotSignalB.Disconnect();
	}

	const char*					mzDisplayName;
	zEmitter<int>::Slot			mSlotSignalA;	//!< Slot to receive signals from EmitterA
	zEmitter<float,bool>::Slot	mSlotSignalB;	//!< Slot to receive signals from EmitterB
};

//==================================================================================================
// Basic example of functions receiving signal and processing them
//==================================================================================================
void SlotCallbackSignalA(int inValue)
{
	printf("\n %s : SignalA triggered, Value=%i", "Function", inValue);
}

void SlotCallbackSignalB(float inValue, bool inDisconnect)
{
	// The 'Disconnect' custom parameter is ignored in this function.
	// Unless we keep pointer to associated slot, we're unable to remove it. 
	// A Lambda can capture the slot ptr if needed, and disconnect inside lambda code
	
	(void)inDisconnect;	//!< Prevents 'unused parameter' warning
	printf("\n %s : SignalB triggered, Value=%03.1f Disconnect=%s", "Function", inValue, "Ignored" );	
}


void SampleUseage()
{
	printf("\n");
	printf("\n============================================================");
	printf("\n Usage examples");
	printf("\n============================================================");

	ClassWithSlot ObjectWithSlot1("Object1");
	ClassWithSlot ObjectWithSlot2("Object2");

	//----------------------------------------------------------------------------------------------
	// Basic Signal/Slot setup with Objects/Lambda/Functions. 
	// Signals are declared with 1 parameter (int)
	//----------------------------------------------------------------------------------------------
	zEmitter<int>					EmitterSignalA;
	decltype(EmitterSignalA)::Slot	SlotSignalAFunction;
	decltype(EmitterSignalA)::Slot	SlotSignalALambda;
	ObjectWithSlot1.ConnectSlotSignalA(EmitterSignalA);					// Connect Object1 slot
	ObjectWithSlot2.ConnectSlotSignalA(EmitterSignalA);					// Connect Object2 slot
	SlotSignalAFunction.Connect(EmitterSignalA, SlotCallbackSignalA);	// Connect a function callback slot
	SlotSignalALambda.Connect(EmitterSignalA, [](int inValue)			// Connect a Lambda slot
	{
		printf("\n %s : SignalA triggered, Value=%i", "Lambda  ", inValue);
	});
	printf("\n\n-------- SignalA(1) --------");
	EmitterSignalA.Signal(1);
	printf("\n\n-------- SignalA(2) --------");
	EmitterSignalA.Signal(2);

	//----------------------------------------------------------------------------------------------
	// Basic Signal/Slot setup with Objects/Lambda/Functions. 
	// Signals are declared with 2 parameters (float,bool)
	//----------------------------------------------------------------------------------------------
	zEmitter<float,bool>			EmitterSignalB;
	decltype(EmitterSignalB)::Slot	SlotSignalBFunction;
	decltype(EmitterSignalB)::Slot	SlotSignalBLambda;
	ObjectWithSlot1.ConnectSlotSignalB(EmitterSignalB);													// Connect Object1 slot
	ObjectWithSlot2.ConnectSlotSignalB(EmitterSignalB);													// Connect Object2 slot
	SlotSignalBFunction.Connect(EmitterSignalB, SlotCallbackSignalB);									// Connect a function callback slot
	SlotSignalBLambda.Connect(EmitterSignalB, [&SlotSignalBLambda](float inValue, bool inDisconnect)	// Connect a Lambda slot
	{
		printf("\n %s : SignalB triggered, Value=%03.1f Disconnect=%s", "Lambda  ", inValue, inDisconnect ? "True" : "False" );
		if( inDisconnect )
			SlotSignalBLambda.Disconnect();
	});
	printf("\n\n-------- SignalB(1, true) --------");
	EmitterSignalB.Signal(1, true);
	//Only 1 slot will receive signal next, since we asked slots to disconnect 
	//but 'SlotCallbackSignalB' function ignore the request
	printf("\n\n-------- SignalB(2, false) --------");
	printf("\nNote: Only 1 slot should receive signal");
	EmitterSignalB.Signal(2, false); 

}