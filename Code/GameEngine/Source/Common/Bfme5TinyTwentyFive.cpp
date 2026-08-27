// Six more: a deadline test with a bit read behind it, two size/flag tests, a
// linear search, a two-member swap and a walk that hands back the node ahead
// of the one it was given.

class BfmeClockCJ
{
public:
	int m_bfmeHead[15];					// +0x00
	int m_bfmeNow;						// +0x3C
	char m_bfmeGap[0x50];					// +0x40
	unsigned char m_bfmeEnabled;				// +0x90
};

extern BfmeClockCJ *g_bfmeSwitchCJ;				// retail 0x012F0898

class BfmeBitsCS
{
public:
	char m_bfmeHead[0x90];					// +0x00
	unsigned char m_bfmeBits;				// +0x90
};

class Gen_00298B80
{
public:
	unsigned char bfmeExpired(void) const;

private:
	int m_bfmeHead[2];					// +0x00
	BfmeBitsCS *m_bfmeBits;					// +0x08
	int m_bfmeGap[5];					// +0x0C
	unsigned int m_bfmeDeadline;				// +0x20
};

// ?bfmeExpired@Gen_00298B80@@QBEEXZ
unsigned char Gen_00298B80::bfmeExpired(void) const
{
	unsigned int now = (unsigned int)g_bfmeSwitchCJ->m_bfmeNow;

	if (now < m_bfmeDeadline)
		return 0;

	unsigned char bits = m_bfmeBits->m_bfmeBits;

	bits = bits >> 2;
	bits = ~bits;
	bits = bits & 1;

	return bits;
}

class BfmeTripleCS
{
public:
	int m_bfmeData[3];					// 12 bytes
};

class Gen_0029A5E0
{
public:
	bool bfmeHasAny(void) const;

private:
	unsigned int bfmeSize(void) const
	{
		return m_bfmeFinish - m_bfmeStart;
	}

	int m_bfmeHead[8];					// +0x00
	BfmeTripleCS *m_bfmeStart;				// +0x20
	BfmeTripleCS *m_bfmeFinish;				// +0x24
};

// ?bfmeHasAny@Gen_0029A5E0@@QBE_NXZ
bool Gen_0029A5E0::bfmeHasAny(void) const
{
	return bfmeSize() > 0;
}

class BfmeOwnerCS
{
public:
	char m_bfmeHead[0x118];					// +0x000
	signed char m_bfmeLevel;				// +0x118
	char m_bfmeGap[3];					// +0x119
	unsigned int m_bfmeFlags;				// +0x11C
};

class Gen_0029A7A0
{
public:
	bool bfmeFlagged(void) const;

private:
	int m_bfmeHead[2];					// +0x00
	BfmeOwnerCS *m_bfmeOwner;				// +0x08
};

// ?bfmeFlagged@Gen_0029A7A0@@QBE_NXZ
bool Gen_0029A7A0::bfmeFlagged(void) const
{
	BfmeOwnerCS *owner = m_bfmeOwner;

	// The sign bit is tested as a mask, which is what makes the branch js
	// rather than the jl a signed comparison would give.
	if (!(owner->m_bfmeFlags & 0x1000000) && !(owner->m_bfmeLevel & 0x80))
		return false;

	return true;
}

class Gen_002E3AB0
{
public:
	int *bfmeFind(int key);

private:
	int m_bfmeHead[31];					// +0x00
	int *m_bfmeStart;					// +0x7C
	int *m_bfmeFinish;					// +0x80
};

// ?bfmeFind@Gen_002E3AB0@@QAEPAHH@Z
int *Gen_002E3AB0::bfmeFind(int key)
{
	int *it = m_bfmeStart;
	int *finish = m_bfmeFinish;

	while (it != finish)
	{
		if (*it == key)
			return it;

		++it;
	}

	return 0;
}

class Gen_003390D0
{
public:
	void bfmeSwap(Gen_003390D0 *other);

private:
	int m_bfmeHead;						// +0x00
	int m_bfmeFirst;					// +0x04
	int m_bfmeSecond;					// +0x08
};

// Each pair goes through the helper by address, which is what stops the
// second load being hoisted over the first store.
inline void bfmeSwapInt(int *first, int *second)
{
	int value = *first;

	*first = *second;
	*second = value;
}

// ?bfmeSwap@Gen_003390D0@@QAEXPAV1@@Z
void Gen_003390D0::bfmeSwap(Gen_003390D0 *other)
{
	bfmeSwapInt(&m_bfmeFirst, &other->m_bfmeFirst);
	bfmeSwapInt(&m_bfmeSecond, &other->m_bfmeSecond);
}

class BfmeNodeCS
{
public:
	int m_bfmeHead;						// +0x00
	BfmeNodeCS *m_bfmeNext;					// +0x04
};

class Gen_0034FC60
{
public:
	BfmeNodeCS *bfmeFindPrev(BfmeNodeCS *node);

private:
	int m_bfmeHead[7];					// +0x00
	BfmeNodeCS *m_bfmeList;					// +0x1C
};

// ?bfmeFindPrev@Gen_0034FC60@@QAEPAVBfmeNodeCS@@PAV2@@Z
BfmeNodeCS *Gen_0034FC60::bfmeFindPrev(BfmeNodeCS *node)
{
	BfmeNodeCS *it = m_bfmeList;

	if (it == node)
		return 0;

	while (it)
	{
		if (it->m_bfmeNext == node)
			return it;

		it = it->m_bfmeNext;
	}

	return 0;
}
