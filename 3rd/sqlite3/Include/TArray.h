#pragma once

#include "new.h"


/************************************************************************/
//								CTArray
/************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CTArray<TYPE> GetData()等取值函数与CArray不同，其它完全一样
//////////////////////////////////////////////////////////////////////////

template<class TYPE, class ARG_TYPE=const TYPE&>
class CTArray// : public CObject
{
public:
	// Construction
	CTArray();
	~CTArray();

	// Attributes
	INT_PTR		GetSize() const;
	INT_PTR		GetCount() const;
	BOOL		IsEmpty() const;
	INT_PTR		GetUpperBound() const;
	void		SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

	// Operations
	// Clean up
	void		FreeExtra();
	void		RemoveAll();

	// Accessing elements
	const TYPE& GetAt(INT_PTR nIndex) const;
	TYPE&		GetAt(INT_PTR nIndex);
	void		SetAt(INT_PTR nIndex, ARG_TYPE newElement);
	const TYPE& ElementAt(INT_PTR nIndex) const;
	TYPE&		ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE*		GetData();

	// Potentially growing the array
	void		SetAtGrow(INT_PTR nIndex, ARG_TYPE newElement);
	INT_PTR		Add(ARG_TYPE newElement);
	INT_PTR		Append(const CTArray& src);
	void		Copy(const CTArray& src);
	INT_PTR		Append(const TYPE aSrc[], INT_PTR nCount);
	void		Copy(const TYPE aSrc[], INT_PTR nCount);

	// overloaded operator helpers
	const TYPE& operator[](INT_PTR nIndex) const;
	TYPE& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void	InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount = 1);
	void	RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void	InsertAt(INT_PTR nStartIndex, CTArray* pNewArray);

	void	Swap(CTArray& other);

	// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount

private:
	CTArray(const CTArray& objectSrc);              // no implementation
	void operator=(const CTArray& objectSrc);       // no implementation
};

/************************************************************************/
/************************************************************************/
/************************************************************************/

#define TARRAYTemplate			template<class TYPE, class ARG_TYPE>
#define CTArrayClass			CTArray<TYPE, ARG_TYPE>

template<class TYPE>
inline void BATCopyElements(TYPE* pDest, const TYPE* pSrc, INT_PTR nCount)
{
	while (nCount--)
		*pDest++ = *pSrc++;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CTArray<TYPE> inline functions

TARRAYTemplate
inline INT_PTR CTArrayClass::GetSize() const
	{ return m_nSize; }
TARRAYTemplate
inline INT_PTR CTArrayClass::GetCount() const
	{ return m_nSize; }
TARRAYTemplate
inline BOOL CTArrayClass::IsEmpty() const
	{ return m_nSize == 0; }
TARRAYTemplate
inline INT_PTR CTArrayClass::GetUpperBound() const
	{ return m_nSize-1; }

TARRAYTemplate
inline void CTArrayClass::RemoveAll()
	{ SetSize(0, -1); }

TARRAYTemplate
inline const TYPE& CTArrayClass::GetAt(INT_PTR nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; }

TARRAYTemplate
inline TYPE& CTArrayClass::GetAt(INT_PTR nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; }

TARRAYTemplate
inline void CTArrayClass::SetAt(INT_PTR nIndex, ARG_TYPE newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement; }

TARRAYTemplate
inline const TYPE& CTArrayClass::ElementAt(INT_PTR nIndex)const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; }

TARRAYTemplate
inline TYPE& CTArrayClass::ElementAt(INT_PTR nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; }

TARRAYTemplate
inline const TYPE* CTArrayClass::GetData() const
	{ return (const TYPE*)m_pData; }

TARRAYTemplate
inline TYPE* CTArrayClass::GetData()
	{ return (TYPE*)m_pData; }

TARRAYTemplate
inline INT_PTR CTArrayClass::Add(ARG_TYPE newElement)
	{ INT_PTR nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; }

TARRAYTemplate
inline const TYPE& CTArrayClass::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }

TARRAYTemplate
inline TYPE& CTArrayClass::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }

TARRAYTemplate
inline void	CTArrayClass::Swap(CTArray& other)
	{Eswap(m_pData, other.m_pData);
	Eswap(m_nSize, other.m_nSize);
	Eswap(m_nMaxSize, other.m_nMaxSize);
	Eswap(m_nGrowBy, other.m_nGrowBy);}

/************************************************************************/
//						CTArray
/************************************************************************/

TARRAYTemplate
CTArrayClass::CTArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

TARRAYTemplate
CTArrayClass::~CTArray()
{
	//ASSERT_VALID(this);

	if (m_pData != NULL)
	{
		for( int i = 0; i < m_nSize; i++ )
			(m_pData + i)->~TYPE();
		delete[] (BYTE*)m_pData;
	}
}

TARRAYTemplate
void CTArrayClass::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	//ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	//if(nNewSize < 0 )
	//	AfxThrowInvalidArgException();

	if (nGrowBy >= 0)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			for( int i = 0; i < m_nSize; i++ )
				(m_pData + i)->~TYPE();
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create buffer big enough to hold number of requested elements or
		// m_nGrowBy elements, whichever is larger.
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(TYPE));    // no overflow
#endif
		size_t nAllocSize = max(nNewSize, m_nGrowBy);
		m_pData = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
		memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(TYPE));
		for( int i = 0; i < nNewSize; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( m_pData + i ) ) TYPE;
#pragma pop_macro("new")
		m_nSize = nNewSize;
		m_nMaxSize = nAllocSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
			for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
				::new( (void*)( m_pData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			for( int i = 0; i < m_nSize-nNewSize; i++ )
				(m_pData + nNewSize + i)->~TYPE();
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

		//if(nNewMax  < m_nMaxSize)
		//	AfxThrowInvalidArgException();

#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];

		// copy new data from old
		memcpy_s(pNewData, (size_t)nNewMax * sizeof(TYPE), m_pData, (size_t)m_nSize * sizeof(TYPE));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
		for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( pNewData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

TARRAYTemplate
INT_PTR CTArrayClass::Append(const CTArray& src)
{
	//ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	INT_PTR nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	BATCopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

TARRAYTemplate
INT_PTR CTArrayClass::Append(const TYPE aSrc[], INT_PTR nCount)
{
	//ASSERT_VALID(this);

	INT_PTR nOldSize = m_nSize;
	SetSize(m_nSize + nCount);
	BATCopyElements<TYPE>(m_pData + nOldSize, aSrc, nCount);
	return nOldSize;
}

TARRAYTemplate
void CTArrayClass::Copy(const CTArray& src)
{
	//ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	BATCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

TARRAYTemplate
void CTArrayClass::operator=(const CTArray& src)
{ 	
	//ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself
	SetSize(src.m_nSize);
	BATCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

TARRAYTemplate
void CTArrayClass::Copy(const TYPE aSrc[], INT_PTR nCount)
{
	//ASSERT_VALID(this);

	SetSize(nCount);
	BATCopyElements<TYPE>(m_pData, aSrc, nCount);
}

TARRAYTemplate
void CTArrayClass::FreeExtra()
{
	//ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

TARRAYTemplate
void CTArrayClass::SetAtGrow(INT_PTR nIndex, ARG_TYPE newElement)
{
	//ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

TARRAYTemplate
void CTArrayClass::InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount /*=1*/)
{
	//ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	//if(nIndex < 0 || nCount <= 0)
	//	AfxThrowInvalidArgException();

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		INT_PTR nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		for( int i = 0; i < nCount; i++ )
			(m_pData + nOldSize + i)->~TYPE();
		// shift old data up to fill gap
		::memmove_s(m_pData + nIndex + nCount, (nOldSize-nIndex) * sizeof(TYPE),
			m_pData + nIndex, (nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(TYPE));
		for( int i = 0; i < nCount; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( m_pData + nIndex + i ) ) TYPE;
#pragma pop_macro("new")
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

TARRAYTemplate
void CTArrayClass::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	//ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	INT_PTR nUpperBound = nIndex + nCount;
	ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

	//if(nIndex < 0 || nCount < 0 || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	//	AfxThrowInvalidArgException();

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nUpperBound);
	for( int i = 0; i < nCount; i++ )
		(m_pData + nIndex + i)->~TYPE();
	if (nMoveCount)
	{
		memmove_s(m_pData + nIndex, (size_t)nMoveCount * sizeof(TYPE),
			m_pData + nUpperBound, (size_t)nMoveCount * sizeof(TYPE));
	}
	m_nSize -= nCount;
}

TARRAYTemplate
void CTArrayClass::InsertAt(INT_PTR nStartIndex, CTArray* pNewArray)
{
	//ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	//ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (INT_PTR i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}