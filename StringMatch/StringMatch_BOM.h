#pragma once

#include <forward_list>
#include <vector>

template<typename Char_T>
class StringMatch_BOM
{
public:

	static const int UNDEFINED = -1;

	StringMatch_BOM()
		: m_x(nullptr)
		, m_m(0)
	{
	}

	void Compile(const Char_T *x, int m)
	{
		assert(x != nullptr && m != 0);
		m_x = x;
		m_m = m;

		/* Preprocessing */
		m_T.swap( std::vector<bool>(m + 1, false) );
		m_L.swap( std::vector<std::forward_list<int>>(m + 1) );
		Oracle(x, m);
	}

	std::vector<int> FindAll(const Char_T *y, int n)
	{
		/* MUST invoke Compile before! */
		assert(y != nullptr && n != 0);
		std::vector<int> matches;

		/* Searching */
		int j = 0;
		while (j <= n - m_m)
		{
			int i = m_m - 1;
			int p = m_m;
			int period, q;
			int shift = m_m;

			while (i + j >= 0 && (q = GetTransition(m_x, p, y[i + j])) != UNDEFINED)
			{
				p = q;
				if (m_T[p])
				{
					period = shift;
					shift = i;
				}
				--i;
			}
			if (i < 0)
			{
				matches.push_back(j);
				shift = period;
			}
			j += shift;
		}

		return matches;
	}

	static std::vector<int> FindAll(const Char_T *x, int m, const Char_T *y, int n)
	{
		StringMatch_BOM _BOM;
		_BOM.Compile(x, m);
		return _BOM.FindAll(y, n);
	}


private:
	int GetTransition(const Char_T *x, int p, Char_T c)
	{
		if (p > 0 && x[p - 1] == c)
		{
			return (p - 1);
		}
		else
		{
			for (auto value : m_L[p])
			{
				if (x[value] == c)
				{
					return value;
				} 
			}
			return (UNDEFINED);
		}
	}

	void SetTransition(int p, int q)
	{
		m_L[p].push_front(q);
	}

	void Oracle(const Char_T *x, int m)
	{
		int p;
		std::vector<int> S(m + 1);

		S[m] = m + 1;
		for (int i = m; i > 0; --i)
		{
			Char_T c = x[i - 1];
			p = S[i];

			int q;
			while (p <= m && (q = GetTransition(x, p, c)) == UNDEFINED)
			{
				SetTransition(p, i - 1);
				p = S[p];
			}

			S[i - 1] = (p == m + 1 ? m : q);
		}
		p = 0;
		while (p <= m)
		{
			m_T[p] = true;
			p = S[p];
		}
	}

private:
	const Char_T*						m_x;
	int									m_m;
	std::vector<bool>					m_T;
	std::vector<std::forward_list<int>> m_L;
};

