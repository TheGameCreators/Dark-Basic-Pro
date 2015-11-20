#include "sAnimationSet.h"

sAnimationSet::sAnimationSet ( )
{
	m_Name      = NULL;
	m_Animation = NULL;
	m_Length    = 0;
	m_Next      = NULL;
}

sAnimationSet::~sAnimationSet ( )
{
	SAFE_DELETE_ARRAY ( m_Name );
	
	SAFE_DELETE ( m_Animation );
	SAFE_DELETE ( m_Next );
}

sAnimationSet* sAnimationSet::FindSet ( char* Name )
{
	sAnimationSet* AnimSet = NULL;

	// return first instance if name is null
	if ( Name == NULL )
		return this;

	// compare names and return if exact match
	if ( m_Name != NULL && !strcmp ( Name, m_Name ) )
		return this;

	// search next in list
	if ( m_Next != NULL )
	{
		if ( ( AnimSet = m_Next->FindSet ( Name ) ) != NULL )
			return AnimSet;
	}

	return NULL;
}

void sAnimationSet::Update ( float Time, BOOL Smooth )
{
	sAnimation *Anim;

	Anim = m_Animation;

	while ( Anim != NULL )
	{
		if ( !m_Length )
		{
			Anim->Update ( 0, FALSE );
		}
		else
		{
			if ( Time >= m_Length && Anim->m_Loop == FALSE )
			{
				Anim->Update ( Time, FALSE );
			}
			else
			{
				float thisTime = Time;
				if ( thisTime > m_Length ) thisTime = thisTime - m_Length;
				Anim->Update ( thisTime, Smooth );
			}
		}

		Anim = Anim->m_Next;
	}
}