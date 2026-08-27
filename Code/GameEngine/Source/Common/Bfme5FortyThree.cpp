// Three more: a bit test over a word array, a constant select and a walk over
// a list.

class Gen_000D2440
{
public:
	int bfmeHasBit(unsigned int index) const;

private:
	int m_bfmeHead[35];					// +0x00
	unsigned int m_bfmeWords[8];				// +0x8C
};

// ?bfmeHasBit@Gen_000D2440@@QBEHI@Z
int Gen_000D2440::bfmeHasBit(unsigned int index) const
{
	return (m_bfmeWords[index >> 5] & (1 << (index & 31))) != 0;
}

class BfmeTripleDL
{
public:
	int m_bfmeData[3];					// 12 bytes
};

class Gen_001EFCE0
{
public:
	int bfmeCost(void) const;

private:
	unsigned int bfmeSize(void) const
	{
		return m_bfmeFinish - m_bfmeStart;
	}

	int m_bfmeHead[17];					// +0x00
	BfmeTripleDL *m_bfmeStart;				// +0x44
	BfmeTripleDL *m_bfmeFinish;				// +0x48
};

// Two constants either way, so the answer comes out as a borrow masked with
// their difference.
// ?bfmeCost@Gen_001EFCE0@@QBEHXZ
int Gen_001EFCE0::bfmeCost(void) const
{
	return bfmeSize() > 0 ? 1 : 0x3FFFFFFF;
}

class BfmeNodeDL
{
public:
	int m_bfmeHead[2];					// +0x00
	int m_bfmeExtra;					// +0x08
	BfmeNodeDL *m_bfmeNext;					// +0x0C
	int m_bfmeCount;					// +0x10
	int m_bfmeLimit;					// +0x14
	bool m_bfmeArmed;					// +0x18
};

class Gen_00160FF0
{
public:
	bool bfmeIdle(void) const;

private:
	int m_bfmeHead[5];					// +0x00
	BfmeNodeDL *m_bfmeHead2;				// +0x14
};

// ?bfmeIdle@Gen_00160FF0@@QBE_NXZ
bool Gen_00160FF0::bfmeIdle(void) const
{
	for (BfmeNodeDL *node = m_bfmeHead2; node != 0; node = node->m_bfmeNext)
	{
		int count = node->m_bfmeCount;

		if (node->m_bfmeExtra != 0)
			++count;

		if (node->m_bfmeLimit > count && node->m_bfmeArmed)
			return false;
	}

	return true;
}
