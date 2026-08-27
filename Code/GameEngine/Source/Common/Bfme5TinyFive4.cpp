// Five more tiny ones: a "last one" test, a pair handed back, two counters
// stepped down while they are above zero, and a read of a volatile global kept
// only because it is volatile.

class Gen_007E4790
{
public:
	unsigned char bfmeIsLast(void) const;

private:
	int m_bfmeHead[15];					// +0x00
	int m_bfmeCount;					// +0x3C
	int m_bfmeGap[2];					// +0x40
	int m_bfmeIndex;					// +0x48
};

// ?bfmeIsLast@Gen_007E4790@@QBEEXZ
unsigned char Gen_007E4790::bfmeIsLast(void) const
{
	return m_bfmeIndex >= m_bfmeCount - 1;
}

class Gen_00662950
{
public:
	void bfmeGetPair(int *out) const;

private:
	int m_bfmeHead[3];					// +0x00
	int m_bfmeFirst;					// +0x0C
	int m_bfmeSecond;					// +0x10
};

// ?bfmeGetPair@Gen_00662950@@QBEXPAH@Z
void Gen_00662950::bfmeGetPair(int *out) const
{
	out[0] = m_bfmeFirst;
	out[1] = m_bfmeSecond;
}

class Gen_00695DC0
{
public:
	void bfmeDropFirst(void);
	void bfmeDropSecond(void);

private:
	int m_bfmeHead[388];					// +0x000
	unsigned int m_bfmeFirst;				// +0x610
	unsigned int m_bfmeSecond;				// +0x614
};

// ?bfmeDropFirst@Gen_00695DC0@@QAEXXZ
void Gen_00695DC0::bfmeDropFirst(void)
{
	unsigned int count = m_bfmeFirst;

	if (count > 0)
		m_bfmeFirst = count - 1;
}

// ?bfmeDropSecond@Gen_00695DC0@@QAEXXZ
void Gen_00695DC0::bfmeDropSecond(void)
{
	unsigned int count = m_bfmeSecond;

	if (count > 0)
		m_bfmeSecond = count - 1;
}

extern int g_bfmeGateBT;					// retail 0x012F9D04
extern volatile int g_bfmeProbeBT;				// retail 0x012F9D0C

// ?bfmeProbe@@YAHXZ
int __cdecl bfmeProbe(void)
{
	if (g_bfmeGateBT)
	{
		int value = g_bfmeProbeBT;
	}

	return 0;
}
