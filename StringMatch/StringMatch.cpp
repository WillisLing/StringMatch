// StringMatch.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <cstddef>
#include <cassert>
#include <memory>

#include <string>
#include <type_traits>


// template<typename Char_T>
// inline size_t _DNDM_EnsureIndex(Char_T idx)
// {
// 	typedef std::make_unsigned<Char_T>::type UChar_T;
// 	return (UChar_T)idx;
// }
// 
// template<typename Char_T>
// size_t DNDM(const Char_T* p, size_t p_len, const Char_T* t, size_t t_len)
// {
// 	assert(p != nullptr && p_len != 0);
// 	assert(t != nullptr && t_len != 0);
// 	// Ԥ����
// 	assert(p_len <= 8 * sizeof(ptrdiff_t));
// 	const size_t kCnt = 1 << (8 * sizeof(Char_T)); // 2^(8 * sizeof(Char_T))
// 	ptrdiff_t B[kCnt];
// 	memset(B, 0, sizeof(ptrdiff_t) * kCnt);
// 
// 	for (size_t j = 0; j < p_len; ++j)
// 	{
// 		B[_DNDM_EnsureIndex(p[j])] |= (1 << (p_len - j - 1));
// 	}
// 	// ����
// 	size_t pos = 0;
// 	while (pos <= t_len - p_len)
// 	{
// 		int j = p_len - 1;
// 		int last = p_len;
// 		ptrdiff_t D = ~0;
// 		while (D != 0)
// 		{
// 			D &= B[_DNDM_EnsureIndex(t[pos + j])];
// 			if (D != 0)
// 			{
// 				if (j != 0)
// 				{
// 					last = j;
// 				} 
// 				else
// 				{
// 					return pos;
// 				}
// 			}
// 			--j;
// 			D <<= 1;
// 		}
// 		pos += last;
// 	}
// 	return -1;
// }

#include <windows.h>
#include <fstream>
#include <sstream>
#include <codecvt>

#include "StringMatch_BOM.h"
#include "StringMatch_DNDM.h"

std::wstring ReadFromFile(const wchar_t* filePath)
{
	//------------------------------------------------------------------------------

	const std::codecvt_mode le_bom = 
		static_cast<std::codecvt_mode>(std::little_endian | 
		std::generate_header | 
		std::consume_header);

	typedef std::codecvt_utf16<wchar_t, 0x10ffff, le_bom> wcvt_utf16le_bom;

	wchar_t buffer[] = L"\u0111\u0107\u010D";

	//------------------------------------------------------------------------------
	wcvt_utf16le_bom cvt(1);

	std::wifstream ifs;
	std::locale wloc(ifs.getloc(), &cvt);
	ifs.imbue(wloc);
	ifs.open(filePath, std::ios::in | std::ios::binary);

	if (ifs)
	{
		std::wstring line;
		std::wostringstream strstm;

		while (std::getline(ifs, line))
		{
			strstm << line;
		}

		return strstm.str();
	}

	return L"";
}

int _tmain(int argc, _TCHAR* argv[])
{
	LARGE_INTEGER counter, counter2, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);
	QueryPerformanceCounter(&counter2);

	
	std::wstring p = ReadFromFile(L"_pattern.txt");
	std::wstring t = ReadFromFile(L"_text.txt");
// 	std::string p = "1992��";
// 	std::string t = "������Щ���ģ��ٿ�����һ����������ͷ�Ŀ���ս��1992�꣬�Ƕѻ���ɽ��ʬ��������ս���ϵĶ̸��ǹ�������ǳ���������Ҳ����ս��Ⱦ����һ��Ѫ�죬ȽȽ�����ڳ������޵Ĺ����У����ǿ�����ؿ�����Ƭ�����ս�����԰͵�����ֱ�����ߵ���������ĸߵء����ּ䣬�������۶ϵĳ�ǹ���������ؽ���������ȼ�յ��������أ���ʬ����ʬ�ѻ���ɽ����ʬɽ�ϻ�������һֻֻ�֡��ţ����������������ġ�����ƬѪ�ȵ�ս���ϣ���ʬһ���ޱߡ�";

	std::vector<size_t> matches_STD;

	std::vector<size_t> matches_DNDM;
	StringMatch_DNDM<wchar_t> _DNDM;
	_DNDM.Compile(p.c_str(), p.length());

	std::vector<int> matches_BOM;
	StringMatch_BOM<wchar_t> _BOM;
	_BOM.Compile(p.c_str(), p.length());

	for (int n = 0; n < 20; ++n)
	{
		QueryPerformanceCounter(&counter);
		matches_DNDM = _DNDM.FindAll(t.c_str(), t.length());
		QueryPerformanceCounter(&counter2);
		double t1 = (counter2.QuadPart-counter.QuadPart)/(double)(freq.QuadPart);

		matches_STD.clear();
		QueryPerformanceCounter(&counter);
		for (size_t off = 0; off < t.length(); /* */)
		{
			size_t pos;
			if ((pos = t.find(p, off)) != (size_t)-1)
			{
				matches_STD.push_back(pos);
				off = pos + p.length();
			} 
			else
			{
				break;
			}
		}
		QueryPerformanceCounter(&counter2);
		double t2 = (counter2.QuadPart-counter.QuadPart)/(double)(freq.QuadPart);

		QueryPerformanceCounter(&counter);
		matches_BOM = _BOM.FindAll(t.c_str(), t.length());
		QueryPerformanceCounter(&counter2);
		double t3 = (counter2.QuadPart-counter.QuadPart)/(double)(freq.QuadPart);

		printf("DNDM: %fs, std::string.find: %fs, BOM: %fs \n", t1, t2, t3);
	}

	if (matches_DNDM.size() == matches_STD.size())
	{
		for (size_t i = 0; i < matches_STD.size(); ++i)
		{
			if (matches_DNDM[i] != matches_STD[i])
			{
				printf("DNDM match failed!\n");
			}
		}
	} 
	else
	{
		printf("DNDM match count failed!");
	}

	if (matches_BOM.size() == matches_STD.size())
	{
		for (size_t i = 0; i < matches_STD.size(); ++i)
		{
			if (matches_BOM[i] != matches_STD[i])
			{
				printf("BOM match failed!\n");
			}
		}
	} 
	else
	{
		printf("BOM match count failed!");
	}

	system("pause");
	return 0;
}

