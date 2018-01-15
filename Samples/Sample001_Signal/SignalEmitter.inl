#pragma once

template<typename... TParameters>
zEmitter<TParameters...>::Slot::~Slot()
{
	Disconnect();
}

template<typename... TParameters>
void zEmitter<TParameters...>::Slot::Disconnect()
{
	List::remove(*this);
}

template<typename... TParameters>
void zEmitter<TParameters...>::Slot::SetCallback(const typename Slot::Callback& _Callback)
{
	mCallback = _Callback;
}

template<typename... TParameters>
const typename zEmitter<TParameters...>::Slot::Callback& zEmitter<TParameters...>::Slot::GetCallback()const
{
	return mCallback;
}

template<typename... TParameters>
void zEmitter<TParameters...>::Connect(Slot& _Slot, const typename Slot::Callback& _Callback)
{
	_Slot.SetCallback(_Callback);
	mlstSlots.push_back(_Slot);
}

template<typename... TParameters>
void zEmitter<TParameters...>::Signal(TParameters..._Values)const
{
	auto it = mlstSlots.begin();
	while( it != mlstSlots.end() )
	{
		const Slot* pSlot = &(*it);
		++it;	//Increment first, so if callback removes slot, won't affect iteration
		pSlot->GetCallback()(_Values...);
	}	
}

template<typename... TParameters>
void zEmitter<TParameters...>::DisconnectAll()
{
	mlstSlots.Empty();
}
