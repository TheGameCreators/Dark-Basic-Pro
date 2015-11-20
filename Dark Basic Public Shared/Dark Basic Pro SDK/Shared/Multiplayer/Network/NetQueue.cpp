// NetQueue.cpp: implementation of the CNetQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "NetQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetQueue::CNetQueue()
{
	m_pQPacket.pMsg=NULL;
	m_pNext=NULL;
	m_pPrev=NULL;
}

CNetQueue::~CNetQueue()
{
	if(m_pQPacket.pMsg)
	{
		GlobalFree(m_pQPacket.pMsg);
		m_pQPacket.pMsg=NULL;
	}
}

void CNetQueue::Clear(void)
{
	if(m_pNext)
	{
		if(m_pNext->m_pNext)
			m_pNext->m_pNext->Clear();
		
		delete m_pNext;
		m_pNext=NULL;
		m_pPrev=NULL;
	}
}

int	CNetQueue::QueueSize(void)
{
	int iCount=0;
	CNetQueue* pCurrent = this;
	while(pCurrent)
	{
		iCount=iCount+1;
		pCurrent=pCurrent->m_pNext;
	}
	return iCount;
}

void CNetQueue::AddToQueue(QueuePacketStruct* pQPacket)
{
	if(m_pNext==NULL)
	{
		// Create item and link it to list
		CNetQueue* pNewItem = new CNetQueue;
		pNewItem->m_pPrev=this;
		m_pNext=pNewItem;

		// Build MSG Data
		int id=pQPacket->pMsg->id;
		DWORD size = pQPacket->pMsg->size;
		char* pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
		memcpy(pData, &id, sizeof(int));
		memcpy(pData+sizeof(int), &size, sizeof(DWORD));
		memcpy(pData+sizeof(int)+sizeof(DWORD), &pQPacket->pMsg->msg, size);

		// Fill item with MSG data
		m_pQPacket.pMsg = (tagNetData*)pData;
		m_pQPacket.idFrom = pQPacket->idFrom;
		m_pQPacket.idTo = pQPacket->idTo;
	}
	else
		m_pNext->AddToQueue(pQPacket);
}

void CNetQueue::TakeNextFromQueue(QueuePacketStruct* pQPacket)
{
	// Take items from start of list as it is a queue
	if(m_pNext==NULL)
	{
		// No more in Queue (apart from this one)
	}
	else
	{
		// Build MSG Data for pMsg
		if(m_pQPacket.pMsg)
		{
			int id=m_pQPacket.pMsg->id;

			DWORD size = m_pQPacket.pMsg->size;
			char* pData = (char*)GlobalAlloc(GMEM_FIXED, sizeof(int) + sizeof(DWORD) + size);
			memcpy(pData, &id, sizeof(int));
			memcpy(pData+sizeof(int), &size, sizeof(DWORD));
			memcpy(pData+sizeof(int)+sizeof(DWORD), &m_pQPacket.pMsg->msg, size);

			// Fill item with MSG data
			pQPacket->pMsg = (tagNetData*)pData;
			pQPacket->idFrom = m_pQPacket.idFrom;
			pQPacket->idTo = m_pQPacket.idTo;

			// Free MSG Data stored in this queue item
			if(m_pNext->m_pQPacket.pMsg)
			{
				GlobalFree(m_pNext->m_pQPacket.pMsg);
				m_pNext->m_pQPacket.pMsg=NULL;
			}

			// Delete first in list and relink rest
			CNetQueue* pStore=NULL;
			if(m_pNext->m_pNext)
			{
				pStore = m_pNext->m_pNext;
				pStore->m_pPrev = this;
			}

			delete m_pNext;
			m_pNext=pStore;
		}
	}
}
