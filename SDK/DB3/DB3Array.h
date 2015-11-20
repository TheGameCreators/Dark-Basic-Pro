#pragma once

#include "DB3.h"
#include "DB3Task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DB_ENTER_NS()

template<typename T>
class TArray
{
protected:
	enum
	{
		Granularity = 2048
	};

	uint			m_Capacity;
	uint			m_Length;
	T				*m_Array;
	CLock			m_Lock;

	inline uint AlignSize(uint x)
	{
		x += Granularity;
		x -= x%Granularity;

		return x;
	}

	inline bool CheckMemory(uint size)
	{
		CAutolock autolock(m_Lock);

		if (!size)
		{
			if (m_Array)
			{
				free(reinterpret_cast<void *>(m_Array));
				m_Array = nullptr;
				m_Capacity = 0;
			}

			return true;
		}

		size = AlignSize(size);

		if (!m_Array)
		{
			m_Array = reinterpret_cast<T *>(malloc(size*sizeof(T)));
			m_Capacity = size;
			return m_Array != nullptr;
		}

		T *p;

		p = reinterpret_cast<T *>(realloc(m_Array, size*sizeof(T)));
		if (!p)
			return false;

		m_Array = p;
		m_Capacity = size;
		return true;
	}

public:
	inline TArray()
	: m_Capacity(0), m_Length(0), m_Array(nullptr), m_Lock()
	{
	}
	inline ~TArray()
	{
	}

	inline CLock &GetLock() const
	{
		return m_Lock;
	}

	inline uint Size() const
	{
		return m_Length;
	}
	inline bool AllocateSlot(uint &index, uint count=1)
	{
		CAutolock autolock(m_Lock);

		if (!count || !CheckMemory(m_Length + count))
			return false;

		index = m_Length;
		m_Length += count;

		for(uint i=index; i<m_Length; i++)
			m_Array[i] = static_cast<T>(0);

		return true;
	}
	inline bool CheckSlot(uint index)
	{
		CAutolock autolock(m_Lock);

		if (index < m_Length)
			return true;

		if (!CheckMemory(index))
			return false;

		m_Length = index + 1;
		return true;
	}

	inline bool Push(const T &x)
	{
		CAutolock autolock(m_Lock);

		db3::uint i;
		if (!AllocateSlot(i))
			return false;

		m_Array[i] = x;
		return true;
	}

	inline T &Element(uint index)
	{
		return m_Array[index];
	}

	inline T &operator[](uint index)
	{
		return m_Array[index];
	}
};

DB_LEAVE_NS()
