template<typename... TParameters>
zEmitter<TParameters...>::Slot::Slot()
{
	mpNext = mpPrev = nullptr;	
}

template<typename... TParameters>
zEmitter<TParameters...>::Slot::~Slot()
{
	Disconnect();
}

template<typename... TParameters>
void zEmitter<TParameters...>::Slot::Disconnect()
{
	if( mpNext != nullptr )
	{
		zEmitter::SlotList::remove(*this);
		mpNext = nullptr;
		mpPrev = nullptr;
	}	
}

template<typename... TParameters>
void zEmitter<TParameters...>::Slot::Connect(zEmitter& _Emitter, const typename Callback& _Callback)
{
	mCallback = _Callback;
	_Emitter.mlstSlots.push_back(*this);
}

template<typename... TParameters>
const typename zEmitter<TParameters...>::Slot::Callback& zEmitter<TParameters...>::Slot::GetCallback()const
{
	return mCallback;
}

template<typename... TParameters>
void zEmitter<TParameters...>::Signal(TParameters..._Values)const
{
	auto it = mlstSlots.begin();
	while( it != mlstSlots.end() )
	{
		const Slot& slot = *it++; //Increment before invoking callback, so if callback removes slot, won't affect iteration
		slot.GetCallback()(_Values...);
	}	
}

template<typename... TParameters>
void zEmitter<TParameters...>::DisconnectAll()
{
	mlstSlots.Empty();
}
