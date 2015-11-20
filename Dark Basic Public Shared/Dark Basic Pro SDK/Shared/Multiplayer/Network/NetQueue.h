// NetQueue.h: interface for the CNetQueue class.
#include "windows.h"
#include "dplay.h"

#include <vector>

// External 
extern int gNetQueueCount;

#ifndef NETQUEUE
#define NETQUEUE

struct tagNetData
{
	int		id;
	DWORD	size;
	VOID	*msg;
};

struct QueuePacketStruct
{
	tagNetData*		pMsg;
	DPID			idFrom;
	DPID			idTo;
};

class CNetQueue  
{
	public:
		CNetQueue();
		virtual ~CNetQueue();

		void			Clear(void);
		void			AddToQueue(QueuePacketStruct* pQPacket);
		void			TakeNextFromQueue(QueuePacketStruct* pQPacket);
		int				QueueSize(void);

		// Link Data
		CNetQueue*		m_pNext;
		CNetQueue*		m_pPrev;

		// MSG Data
		QueuePacketStruct m_pQPacket;
};



#endif
