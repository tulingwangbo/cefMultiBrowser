#pragma once


#define B2H(pD, B, HC)\
{\
	int v = ((B)>>4)&0xf;\
	*pD++ = v>9 ? (v-10+HC) : (v+'0');\
	v = (B)&0xf;\
	*pD++ = v>9 ? (v-10+HC) : (v+'0');\
}

inline void M2W(OUT std::wstring& wcstring, IN LPCSTR szString, int nLen, UINT CodePage)
{
	wcstring.clear();
	if (szString == NULL || szString[0] == 0 || nLen==0)return;
	if(nLen>0 && szString[nLen-1]==0)
		nLen=-1;
	int n = MultiByteToWideChar(CodePage, 0, szString, nLen, NULL, 0);
	if (n <= 0)return;

	if(nLen>0)
		wcstring.resize(n);
	else
		wcstring.resize(n - 1);//自动检查字符串长度的方式下，返回的n包括了结尾的0
	MultiByteToWideChar(CodePage, 0, szString, nLen, wcstring._Myptr(), (int)wcstring.length()); //限制了返回的长度
}

inline void W2M(OUT std::string& cstring, IN LPCWSTR wcstring, int nLen, UINT CodePage)
{
	cstring.clear();
	if (wcstring == NULL || wcstring[0] == 0 || nLen==0)return;
	if(nLen>0 && wcstring[nLen-1]==0)
		nLen=-1;
	int n = WideCharToMultiByte(CodePage, 0, wcstring, nLen, NULL, 0, NULL, 0);
	if (n <= 0)return;

	if(nLen>0)
		cstring.resize(n);
	else
		cstring.resize(n - 1);//自动检查字符串长度的方式下，返回的n包括了结尾的0
	WideCharToMultiByte(CodePage, 0, wcstring, nLen, cstring._Myptr(), (int)cstring.length(), NULL, 0);
}


class CM2W
{
public:
	CM2W(IN LPCSTR psz, IN UINT nCodePage)					{M2W(m_str, psz, -1, nCodePage);}
	CM2W(IN LPCSTR psz, IN int nLen, IN UINT nCodePage)		{M2W(m_str, psz, nLen, nCodePage);}
	operator LPCWSTR() const								{return( m_str.c_str());}
	std::wstring m_str;
};

class CW2M
{
public:
	CW2M(IN LPCWSTR psz, IN UINT nCodePage)					{W2M(m_str, psz, -1, nCodePage);}
	CW2M(IN LPCWSTR psz, IN int nLen, IN UINT nCodePage)	{W2M(m_str, psz, nLen, nCodePage);}
	operator LPCSTR() const									{return( m_str.c_str());}
	std::string m_str;
};


template<class T>
int _GuidToString(const GUID &guid, T strGUID[40], bool bUpperCase, bool bWithBrks)
{
	char HC = bUpperCase ? 'A' : 'a';

	T* p = strGUID;
	if(bWithBrks)
		*p++ = '{';

	BYTE* pS = (BYTE*)&guid.Data1;
	B2H(p, pS[3], HC);
	B2H(p, pS[2], HC);
	B2H(p, pS[1], HC);
	B2H(p, pS[0], HC);
	*p++ = '-';

	pS = (BYTE*)&guid.Data2;
	B2H(p, pS[1], HC);
	B2H(p, pS[0], HC);
	*p++ = '-';

	pS = (BYTE*)&guid.Data3;
	B2H(p, pS[1], HC);
	B2H(p, pS[0], HC);
	*p++ = '-';

	pS = (BYTE*)guid.Data4;
	B2H(p, pS[0], HC);
	B2H(p, pS[1], HC);
	*p++ = '-';

	B2H(p, pS[2], HC);
	B2H(p, pS[3], HC);
	B2H(p, pS[4], HC);
	B2H(p, pS[5], HC);
	B2H(p, pS[6], HC);
	B2H(p, pS[7], HC);

	if(bWithBrks)
		*p++ = '}';

	*p++ = 0;

	return int(p-strGUID)-1;
}

inline int GuidToString(const GUID &guid, OUT TCHAR strGUID[40], bool bUpperCase, bool bWithBrks)
{
	return _GuidToString<TCHAR>(guid, strGUID, bUpperCase, bWithBrks);
}

