//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CObjectWithProperties
//
// - mix-in base class
//
#ifndef __OBJECT_WITH_PROPERTIES__
#define __OBJECT_WITH_PROPERTIES__

//
class CObjectWithProperties
{
// operations
public:
	//
	virtual int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	virtual LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return NULL; }
	virtual LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return NULL; }
	virtual double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	//
	virtual int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	virtual int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	virtual int SetValue(int nItem, LPCTSTR szValue=NULL, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	virtual int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	virtual int SetValueDouble(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0) { return 0; }
	//
	virtual void RefreshProperties() {}
	//
	virtual bool Notify(long lCode, long param1, long param2) { return false; }
};

#endif