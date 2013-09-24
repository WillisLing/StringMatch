#pragma once

#include <type_traits>

// 只在普通字符集上(如中英文混合)，且模式字符串较长的时候，表现优秀
template<typename Char_T>
class StringMatch_Horspool
{
protected:
	template<typename T>
	struct _TypeTraits
	{ // for char
		static_assert(std::is_same<T, char>::value, "Char_T type in StringMatch_Horspool must be char or wchar_t");
		inline static int tcsncmp(const char* str1, const char* str2, size_t maxCount)
		{
			return strncmp(str1, str2, maxCount);
		}
	};
	template<>
	struct _TypeTraits<wchar_t>
	{ // for wchar_t
		inline static int tcsncmp(const wchar_t* str1, const wchar_t* str2, size_t maxCount)
		{
			return wcsncmp(str1, str2, maxCount);
		}
	};

public:
	StringMatch_Horspool()
	{
	}

	void Compile(const Char_T* p, size_t p_len)
	{
		assert(p != nullptr && p_len != 0);
		m_pattern = p;
		m_patternLen = p_len;

		/* Preprocessing */
		for (size_t i = 0; i < ms_kCnt; ++i)
		{
			m_d[i] = p_len;
		}

		for (size_t j = 0; j < p_len - 1; ++j)
		{
			m_d[_EnsureIndex(p[j])] = p_len - j - 1;
		}
	}

	std::vector<size_t> FindAll(const Char_T* t, size_t t_len)
	{
		/* MUST invoke Compile before! */
		assert(t != nullptr && t_len != 0);
		std::vector<size_t> matches;

		/* Searching */
		size_t pos = 0;
		while (pos <= t_len - m_patternLen)
		{
			Char_T ch = t[pos + m_patternLen - 1];
			if (m_pattern[m_patternLen - 1] == ch &&
				_TypeTraits<Char_T>::tcsncmp(m_pattern, t + pos, m_patternLen - 1) == 0)
			{
				matches.push_back(pos);
			}
			pos += m_d[_EnsureIndex(ch)];
		}
		return matches;
	}

	static std::vector<size_t> FindAll(const Char_T* p, size_t p_len, const Char_T* t, size_t t_len)
	{
		StringMatch_Horspool _horspool;
		_horspool.Compile(p, p_len);
		return _horspool.FindAll(t, t_len);
	}

private:
	inline size_t _EnsureIndex(Char_T idx)
	{
		typedef std::make_unsigned<Char_T>::type UChar_T;
		return (UChar_T)idx;
	}

private:
	static const size_t		ms_kCnt = 1 << (8 * sizeof(Char_T)); // 2^(8 * sizeof(Char_T))
	intptr_t				m_d[ms_kCnt];
	size_t					m_patternLen;
	const Char_T*			m_pattern;
};

