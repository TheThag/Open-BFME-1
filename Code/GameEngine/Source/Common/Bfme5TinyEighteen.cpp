// Four slightly bigger ones: a bit test with an explicit mask -- the mask is
// in the source, x86 would have done it anyway -- a ratio with a default when
// the divisor is zero (the divisor goes to a stack slot because the divide is
// an integer one), a walk that answers one only if nothing is busy, and
// another word in a table tested for zero.

// ?bfmeTestBit@@YAHPAIH@Z
int __cdecl bfmeTestBit(unsigned int *bits, int index)
{
	return (*bits & (1 << (index & 31))) != 0;
}

extern float g_bfmeDefaultCJ;					// retail 0x01075334

class Gen_000C7DE0
{
public:
	float bfmeRatio(void) const;

private:
	int m_bfmeTag;						// +0x00
	int m_bfmeValue;					// +0x04
	int m_bfmeCount;					// +0x08
};

// ?bfmeRatio@Gen_000C7DE0@@QBEMXZ
float Gen_000C7DE0::bfmeRatio(void) const
{
	int count = m_bfmeCount;

	if (count == 0)
		return g_bfmeDefaultCJ;

	return (float)m_bfmeValue / count;
}

class BfmeNodeCJ
{
public:
	int m_bfmeHead[2];					// +0x00
	int m_bfmeBusy;						// +0x08
	BfmeNodeCJ *m_bfmeNext;					// +0x0C
};

class Gen_00161030
{
public:
	unsigned char bfmeAllIdle(void) const;

private:
	int m_bfmeHead[5];					// +0x00
	BfmeNodeCJ *m_bfmeList;					// +0x14
};

// ?bfmeAllIdle@Gen_00161030@@QBEEXZ
unsigned char Gen_00161030::bfmeAllIdle(void) const
{
	BfmeNodeCJ *node = m_bfmeList;

	while (node)
	{
		if (node->m_bfmeBusy)
			return 0;

		node = node->m_bfmeNext;
	}

	return 1;
}

class BfmeSlotCJ
{
public:
	char m_bfmeHead[6];					// +0x00
	short m_bfmeTag;					// +0x06
	char m_bfmeRest[8];					// +0x08
};

class Gen_000EE640
{
public:
	bool bfmeIsFree(int index) const;

private:
	char m_bfmeHead[0x63C];					// +0x000
	BfmeSlotCJ *m_bfmeSlots;				// +0x63C
};

// ?bfmeIsFree@Gen_000EE640@@QBE_NH@Z
bool Gen_000EE640::bfmeIsFree(int index) const
{
	BfmeSlotCJ *slots = m_bfmeSlots;

	return slots[index].m_bfmeTag == 0;
}
