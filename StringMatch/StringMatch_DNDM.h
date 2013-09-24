#pragma once

#include <memory>
#include <vector>
#include <type_traits>

// 在模式字符串＜＝机器字长时，表现优秀
template<typename Char_T>
class StringMatch_DNDM
{
public:

	StringMatch_DNDM()
		: m_patternLen(0)
	{
	}

	void Compile(const Char_T* p, size_t p_len)
	{
		assert(p != nullptr && p_len != 0);
		m_patternLen = p_len;

		/* Preprocessing */
		// 由于要作位运算，所以这里具有长度限制: x86 上最大 32, x64 上最大 64
		assert(p_len <= 8 * sizeof(intptr_t));
		memset(m_B, 0, sizeof(intptr_t) * ms_kCnt);

		for (size_t j = 0; j < p_len; ++j)
		{
			m_B[_EnsureIndex(p[j])] |= (1 << (p_len - j - 1));
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
			int j = m_patternLen - 1;
			int last = m_patternLen;
			intptr_t D = ~0;
			while (D != 0)
			{
				D &= m_B[_EnsureIndex(t[pos + j])];
				if (D != 0)
				{
					if (j != 0)
					{
						last = j;
					} 
					else
					{
						matches.push_back(pos);
					}
				}
				--j;
				D <<= 1;
			}
			pos += last;
		}
		return matches;
	}

	static std::vector<size_t> FindAll(const Char_T* p, size_t p_len, const Char_T* t, size_t t_len)
	{
		StringMatch_DNDM _DNDM;
		_DNDM.Compile(p, p_len);
		return _DNDM.FindAll(t, t_len);
	}

private:
	inline size_t _EnsureIndex(Char_T idx)
	{
		typedef std::make_unsigned<Char_T>::type UChar_T;
		return (UChar_T)idx;
	}

private:
	static const size_t		ms_kCnt = 1 << (8 * sizeof(Char_T)); // 2^(8 * sizeof(Char_T))
	intptr_t				m_B[ms_kCnt];
	size_t					m_patternLen;
};

