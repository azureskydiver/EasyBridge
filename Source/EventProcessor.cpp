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
#include "stdafx.h"
#include "EventProcessor.h"




////////////////////////////////////////////////////////////////////////
//
// CNotifyEvent 
//
////////////////////////////////////////////////////////////////////////

// CNotifyEvent constructor
CNotifyEvent::CNotifyEvent(long lCode, long param1, long param2, long param3) :
			m_lCode(lCode), m_lParam1(param1), m_lParam2(param2), m_lParam3(param3)
{}

// CNotifyEvent copy constructor
CNotifyEvent::CNotifyEvent(const CNotifyEvent& src)
{
	operator=(src);
}


//
// operator=()
//
CNotifyEvent& CNotifyEvent::operator=(const CNotifyEvent& src)
{
	m_lCode = src.m_lCode; 
	m_lParam1 = src.m_lParam1; 
	m_lParam2 = src.m_lParam2; 
	m_lParam3 = src.m_lParam3;
	return *this;
}




////////////////////////////////////////////////////////////////////////
//
// CEventProcessor
//
////////////////////////////////////////////////////////////////////////


// constructor
/*
CEventProcessor::CEventProcessor()
{
}
*/

// destructor
CEventProcessor::~CEventProcessor() 
{ 
	while(!m_queueEvents.empty())
		m_queueEvents.pop(); 
}


//
// PostNotify()
//
bool CEventProcessor::PostNotify(long lCode, long param1, long param2, long param3) 
{
	m_queueEvents.push(CNotifyEvent(lCode, param1, param2, param3)); 
	return true;
}


//
// GetNextEvent()
//
CNotifyEvent CEventProcessor::GetNextEvent() 
{
	CNotifyEvent event = m_queueEvents.front();
	m_queueEvents.pop();
	return event;
}


