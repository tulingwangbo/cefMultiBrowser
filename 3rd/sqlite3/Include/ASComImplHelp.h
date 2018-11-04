// ASComImplHelp.h
// Com接口帮助定义
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef ASCIHELP_STANDARD_REF

//---------------------------------------------------------------
//COM中关于引用计数的实现
//---------------------------------------------------------------
#define ASCIHELP_STANDARD_REF()\
	protected:struct _RefC{long nC;_RefC():nC(0){}} m_RefCount;\
	public:\
	virtual ULONG STDMETHODCALLTYPE AddRef( void){return InterlockedIncrement(&m_RefCount.nC);}\
	virtual ULONG STDMETHODCALLTYPE Release( void)\
	{\
		long nRet = InterlockedDecrement(&m_RefCount.nC);\
		ASSERT(nRet>=0);\
		if(nRet<=0)delete this;\
		return nRet;\
	}

//---------------------------------------------------------------
// COM中关于标准IUnknown接口的实现
// 使用该宏后，仅仅需要实现 QueryInterface
//---------------------------------------------------------------
#define ASCIHELP_STANDARD_UNKNOWN()\
	ASCIHELP_STANDARD_REF();\
	public:	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);


//---------------------------------------------------------------
// 只有一种接口的QueryInterface的实现
//---------------------------------------------------------------
#define ASCIHELP_ONE_QUERYINTERFACE(OneInterface)\
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppObj)\
	{\
		if(ppObj==NULL)return E_POINTER;\
		if(riid==IID_IUnknown)\
		{\
			*ppObj = (IUnknown*)this;\
			((IUnknown*)*ppObj)->AddRef();\
		}\
		else if(riid==__uuidof(OneInterface))\
		{\
			*ppObj = (OneInterface *)this;\
			((OneInterface*)*ppObj)->AddRef();\
		}\
		else\
			*ppObj = NULL;\
		return *ppObj ? S_OK : E_NOINTERFACE;\
	}


//---------------------------------------------------------------
// COM中关于标准IUnknown接口＋另外一种接口的实现
//---------------------------------------------------------------
#define ASCIHELP_STANDARD_UNKNOWN_AI(OneInterface)\
	ASCIHELP_STANDARD_REF();\
	public:	ASCIHELP_ONE_QUERYINTERFACE(OneInterface);



//--------------------------------------------------------------------------
// CASMPtr  智能指针
//--------------------------------------------------------------------------
template <class T>
class _NoAddRefReleaseOnCASMPtr : public T
{
private:
	virtual ULONG STDMETHODCALLTYPE AddRef()=0;
	virtual ULONG STDMETHODCALLTYPE Release()=0;
};

template <class T>
class CASMPtr
{
public:
	T* p;
public:
	CASMPtr() throw()				{p = NULL;}

	template <typename Q>
	CASMPtr(Q* lp) throw()	
	{
		p = NULL;
		if(lp==NULL || FAILED(lp->QueryInterface(__uuidof(T), (void**)&p)))
			p=NULL;
	}
	CASMPtr(T* lp) throw()			{p = NULL;*this=lp;}

	template <class Q>
	CASMPtr(CASMPtr<Q>& Ptr) throw()	
	{
		p = NULL;
		if(Ptr==NULL || FAILED(Ptr->QueryInterface(&p)))
			p=NULL;
	}
	CASMPtr(CASMPtr<T>& Ptr)throw()	{p = NULL;*this = Ptr;}

	~CASMPtr() throw()				{if (p)p->Release();}

	//////////////////////////////////////////////////////////////////////////

	operator T*() const throw()		{return p;}
	T& operator*() const			{return *p;}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		ASSERT(p==NULL);
		return &p;
	}
	_NoAddRefReleaseOnCASMPtr<T>* operator->() const throw()
	{
		ASSERT(p!=NULL);
		return (_NoAddRefReleaseOnCASMPtr<T>*)p;
	}
	bool operator!() const throw()	{return (p == NULL);}
	bool operator!=(T* pT) const{return !operator==(pT);}
	bool operator==(T* pT) const throw(){return p == pT;}

	template <typename Q>
	T* operator=(Q* lp) throw()
	{
		T* pT = p;
		if(lp==NULL || FAILED(lp->QueryInterface(__uuidof(T), (void**)&p)))
			p = NULL;
		if(pT)pT->Release();
		return p;
	}

	T* operator=(T* lp) throw()
	{
		if(p==lp)return p;
		if (lp)lp->AddRef();
		if (p)	p->Release();
		p = lp;
		return lp;
	}

	template <class Q>
	T* operator=(CASMPtr<Q>& Ptr) throw(){return *this=(Q*)Ptr;}
	T* operator=(CASMPtr<T>& Ptr) throw(){return *this=(T*)Ptr;}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}

	template <class Q>
	HRESULT QueryInterface(Q** pp) const throw()
	{
		ASSERT(pp != NULL);
		return p->QueryInterface(__uuidof(Q), (void**)pp);
	}
};

#endif
