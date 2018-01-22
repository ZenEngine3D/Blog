#pragma once

#include <functional>
#include <EASTL/intrusive_list.h>

//==================================================================================================
//! @Class Signal/Slots systems for any type of callbacks
//! @details	This can can be used to implement events/listeners system. 
//!				It is simple to use and fast, but no multi threading support for this version. 
//!				A derived class can extend support for more complex behaviors, such as async messages
//! @Example	Look at 'SampleSignal.cpp' for usage
//==================================================================================================
template<typename... TParameters>
class zEmitter
{
public:
	//----------------------------------------------------------------------------------------------
	//! @Class	Class connecting a Listener to an event Emitter
	//! @detail Wrap the callback and list node into one, and allow auto removal from emitter, 
	//!			as soon as an object is out of scope (when Slot a a member of deleted object), 
	//!			thus preventing invoking callback on freed memory.
	//----------------------------------------------------------------------------------------------
	class Slot : public eastl::intrusive_list_node
	{	
	public:									
		typedef std::function<void(TParameters...)>	Callback;										//!< Callback function signature
		typedef zEmitter<TParameters...>			Emitter;										//!< Useful to get emitter type that works with this slot type

	public:
								Slot();
								~Slot();															//!< Remove this slot from list kept in emitter, when slot is destroyed
		inline void				Connect(zEmitter& _Emitter,const typename Callback& _Callback);		//!< Bind signal to a function to invoke when received
		inline void				Disconnect();														//!< Remove this Slot from Emitter Listeners
		inline const Callback&	GetCallback()const;
	protected:
		Callback				mCallback;															//!< Functions emitter should call when signaling
	};

	//----------------------------------------------------------------------------------------------
	// Main content of the class
	//----------------------------------------------------------------------------------------------
public:	
	inline void					Signal(TParameters..._Values)const;									//!< Signal all slots connected to this emitter
	inline void					DisconnectAll();													//!< Remove all slots connected to this emitter

protected:
	typedef eastl::intrusive_list<Slot> SlotList;
	friend class Slot;																				//!< Allow access to mlstSlots when calling 'Connect'	
	SlotList					mlstSlots;															//!< Linked list of all slots waiting for Signals	
};

#include "SignalEmitter.inl"

