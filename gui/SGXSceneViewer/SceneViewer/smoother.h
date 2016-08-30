
#pragma once

#include <vector>

namespace SynGlyphX
{
	// Template class to time-interpolate a value over multiple frames; helpful for camera smoothing.
	// T needs to support multiplication (by float), assignment, and default-construction to its zero
	// value.
	template<typename T>
	class Smoother
	{
	public:
		Smoother( unsigned int _slot_count = 8u );
		~Smoother() { delete[] _slots; }
		void update( const T& new_value, float new_duration );
		T smoothed_value();
		void enable( bool state ) { enabled = state; }
		void reset();
	private:
		struct entry_t
		{
			T value;
			float duration;
		};
		entry_t* _slots;
		unsigned int latest_slot;
		unsigned int slot_count;
		bool enabled;
	};

	template<typename T> Smoother<T>::Smoother( unsigned int _slot_count )
		: latest_slot( 0u ), slot_count( _slot_count ), enabled( true )
	{
		_slots = new entry_t[_slot_count];
		entry_t e;
		e.duration = 0.f;
		for ( unsigned int i = 0; i < _slot_count; ++i )
			_slots[i] = e;
	}

	template<typename T> void Smoother<T>::reset()
	{
		entry_t e;
		e.duration = 0.f;
		for ( unsigned int i = 0; i < slot_count; ++i )
			_slots[i] = e;
	}

	template<typename T> void Smoother<T>::update( const T& new_value, float new_duration )
	{
		latest_slot = ( latest_slot + 1 ) % slot_count;
		_slots[latest_slot].duration = new_duration;
		_slots[latest_slot].value = new_value;
	}

	template<typename T> T Smoother<T>::smoothed_value()
	{
		if ( enabled )
		{
			float total_interval = 0.f;
			T total_value;
			for ( unsigned int i = 0; i < slot_count; ++i )
			{
				total_interval += _slots[i].duration;
				total_value += _slots[i].value;
			}
			return total_value * ( 1.f / total_interval );
		}
		else
		{
			return _slots[latest_slot].value * ( 1.f / _slots[latest_slot].duration );
		}
	}
}
