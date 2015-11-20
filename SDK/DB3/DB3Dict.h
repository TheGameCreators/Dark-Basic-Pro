#pragma once

#include "DB3.h"
#include "DB3Task.h"

DB_ENTER_NS()

template<typename T> class TDictionary
{
public:
	enum EFindOption
//	enum class EFindOption
	{
		ExistingOnly,
		CreateIfNotExist
	};
	enum EOnDestruct
//	enum class EOnDestruct
	{
		DeleteEntries,
		DeleteEntriesAndData
	};
	enum ECase
//	enum class ECase
	{
		Sensitive,
		Insensitive
	};

	struct SEntry {
	friend class TDictionary<T>;
	public:
		T *P;
		SEntry *Entries;

		inline SEntry()
		: P(nullptr), Entries(nullptr)
		{
		}
		inline ~SEntry() {
			delete [] Entries;
			Entries = nullptr;
		}
	};

	TDictionary(const char *allowed, ECase casing=ECase::Sensitive, EOnDestruct onDtor=EOnDestruct::DeleteEntries);
	~TDictionary();

	SEntry *Find(const char *key);
	SEntry *Lookup(const char *key);

	void Purge();

protected:
	SEntry *m_Entries;
	CLock m_Lock;

	u8 m_Convmap[256];
	u8 m_NumEntries;

	EOnDestruct m_OnDtor;

	u8 GenerateConvmap(const char *allowed, ECase casing);
	SEntry *FindFromEntry(SEntry *&entries, const char *str, EFindOption opt);

	void DeleteFromEntry(SEntry *entry);
};

template<typename T>
inline u8 TDictionary<T>::GenerateConvmap(const char *allowed, ECase casing)
{
	u8 i, j, k;
	u8 bias;

	for(i=0; i<255; i++)
		m_Convmap[i] = 0xFF;

	bias = 0;
	for(i=0; allowed[i] && i<255; i++)
	{
		k = ((u8 *)allowed)[i];
		if (k >= 255)
			return 0;

		if (casing==ECase::Insensitive)
		{
			if (k>='a' && k<='z')
			{
				if (m_Convmap[k - 'a' + 'A'] != 0xFF)
				{
					bias++;
					continue;
				}
			}
			else if(k>='A' && k<='Z')
			{
				if (m_Convmap[k - 'A' + 'a'] != 0xFF)
				{
					bias++;
					continue;
				}
			}
		}

		m_Convmap[k] = i - bias;
	}

	i -= bias;

	for(j=0; j<i; j++)
	{
		for(k=j + 1; k<i; k++)
		{
			if (allowed[j]==allowed[k])
				return 0; //duplicate entries
		}
	}

	// allow for case insensitivity by adjusting the conversion-map to point to the same entries for different characters
	if (casing==ECase::Insensitive)
	{
		if (m_Convmap['a']!=0xFF)
		{
			for(j='A'; j<='Z'; j++)
			{
				if (m_Convmap[j]==0xFF)
					continue;

				i--;
			}

			for(j='A'; j<='Z'; j++)
				m_Convmap[j] = m_Convmap[j - 'A' + 'a'];
		}
		else if(m_Convmap['A']!=0xFF)
		{
			for(j='a'; j<='z'; j++)
			{
				if (m_Convmap[j]==0xFF)
					continue;

				i--;
			}

			for(j='a'; j<='z'; j++)
				m_Convmap[j] = m_Convmap[j - 'a' + 'A'];
		}
		else
			return 0;
	}

	return i;
}

template<typename T> inline
typename TDictionary<T>::SEntry *TDictionary<T>::FindFromEntry(SEntry *&entries, const char *str, EFindOption opt)
{
	u8 i;

	assert(m_Entries != nullptr);
	assert(entries != nullptr);
	assert(str != nullptr);

	i = m_Convmap[*(u8 *)str++];
	if (i==0xFF)
		return nullptr;

	if (*str=='\0')
		return &entries[i];

	if (!entries[i].Entries)
	{
		if (opt==EFindOption::CreateIfNotExist)
		{
			entries[i].Entries = new SEntry[m_NumEntries];
			memset(reinterpret_cast<void *>(entries[i].Entries), 0, sizeof(SEntry)*m_NumEntries);
		}

		if (!entries[i].Entries)
			return nullptr;
	}

	return FindFromEntry(entries[i].Entries, str, opt);
}
template<typename T>
inline void TDictionary<T>::DeleteFromEntry(SEntry *entry)
{
	u8 i;

	if (!entry)
		return;

	if (entry->Entries)
	{
		for(i=0; i<m_NumEntries; i++)
			DeleteFromEntry(&entry->Entries[i]);
	}

	delete entry->P;
	entry->P = nullptr;
}

template<typename T>
inline TDictionary<T>::TDictionary(const char *allowed, ECase casing, EOnDestruct onDtor)
: m_OnDtor(onDtor)
{
	assert(allowed != nullptr);

	m_NumEntries = GenerateConvmap(allowed, casing);
	assert(m_NumEntries != 0);

	m_Entries = new SEntry[m_NumEntries];
	memset(reinterpret_cast<void *>(m_Entries), 0, sizeof(SEntry)*m_NumEntries);
}
template<typename T>
inline TDictionary<T>::~TDictionary()
{
	CAutolock autolock(m_Lock);

	if (m_OnDtor==EOnDestruct::DeleteEntriesAndData)
		Purge();

	delete [] m_Entries;
	m_Entries = nullptr;
	
	m_NumEntries = 0;
}

template<typename T>
inline typename TDictionary<T>::SEntry *TDictionary<T>::Find(const char *key)
{
	CAutolock autolock(m_Lock);

	return FindFromEntry(m_Entries, key, EFindOption::ExistingOnly);
}
template<typename T>
inline typename TDictionary<T>::SEntry *TDictionary<T>::Lookup(const char *key)
{
	CAutolock autolock(m_Lock);

	return FindFromEntry(m_Entries, key, EFindOption::CreateIfNotExist);
}

template<typename T>
inline void TDictionary<T>::Purge()
{
	CAutolock autolock(m_Lock);

	u8 i;
	for(i=0; i<m_NumEntries; i++)
	{
		DeleteFromEntry(&m_Entries[i]);
		//m_Entries[i] = nullptr;
	}
}

DB_LEAVE_NS()
