//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CEventProcessor
//
// - mix-in base class
//
#ifndef __EVENT_PROCESSOR__
#define __EVENT_PROCESSOR__

#include <queue>


//
class CNotifyEvent 
{
//
public:
	CNotifyEvent(long lCode, long param1=0, long param2=0, long param3=0);
	CNotifyEvent(const CNotifyEvent& src);
	//
	CNotifyEvent& operator=(const CNotifyEvent& src);

// data
public:
	long	m_lCode, m_lParam1, m_lParam2, m_lParam3;
};


//
class CEventProcessor
{
// operations
public:
	//
	virtual bool Notify(long lCode, long param1=0, long param2=0, long param3=0) { return false; }
	virtual bool PostNotify(long lCode, long param1=0, long param2=0, long param3=0);
	virtual bool IsQueueEmpty() { return (m_queueEvents.size() == 0); }
	virtual bool ProcessQueuedMessages() { return false; }
	virtual CNotifyEvent GetNextEvent();
	//
	virtual ~CEventProcessor();
	
// data
protected:
	std::queue<CNotifyEvent>	m_queueEvents;
};

#endif