// Four more: a third masked table read, a three-float difference, a search
// over four slots and a linear search.

extern int g_bfmeTableDJc[];					// retail 0x012AD6B0

class Gen_001C3FF0
{
public:
	int bfmeLookup(int index) const;

private:
	unsigned int m_bfmeMask;				// +0x00
};

// ?bfmeLookup@Gen_001C3FF0@@QBEHH@Z
int Gen_001C3FF0::bfmeLookup(int index) const
{
	if (m_bfmeMask & (1 << (index & 31)))
		return g_bfmeTableDJc[index];

	return 0;
}

class BfmeVec3DG
{
public:
	BfmeVec3DG(void)
	{
	}

	BfmeVec3DG(const BfmeVec3DG &other)
	{
		m_bfmeX = other.m_bfmeX;
		m_bfmeY = other.m_bfmeY;
		m_bfmeZ = other.m_bfmeZ;
	}

	float m_bfmeX;						// +0x00
	float m_bfmeY;						// +0x04
	float m_bfmeZ;						// +0x08
};

class Gen_00148990
{
public:
	BfmeVec3DG bfmeDelta(const BfmeVec3DG *point) const;

private:
	int m_bfmeHead[14];					// +0x00
	float m_bfmeX;						// +0x38
	float m_bfmeY;						// +0x3C
	float m_bfmeZ;						// +0x40
};

// ?bfmeDelta@Gen_00148990@@QBE?AVBfmeVec3DG@@PBV2@@Z
BfmeVec3DG Gen_00148990::bfmeDelta(const BfmeVec3DG *point) const
{
	BfmeVec3DG delta;

	delta.m_bfmeX = point->m_bfmeX - m_bfmeX;
	delta.m_bfmeY = point->m_bfmeY - m_bfmeY;
	delta.m_bfmeZ = point->m_bfmeZ - m_bfmeZ;

	return delta;
}

class BfmeOwnerDK
{
public:
	char m_bfmeHead[0x4FB];					// +0x000
	bool m_bfmeReady;					// +0x4FB
};

class BfmeSlotDK
{
public:
	int m_bfmeHead;						// +0x00
	BfmeOwnerDK *m_bfmeOwner;				// +0x04
};

class Gen_001EAEB0
{
public:
	BfmeSlotDK *bfmeFind(void) const;

private:
	int m_bfmeHead[2];					// +0x00
	BfmeSlotDK *m_bfmeSlots[4];				// +0x08
};

// ?bfmeFind@Gen_001EAEB0@@QBEPAVBfmeSlotDK@@XZ
BfmeSlotDK *Gen_001EAEB0::bfmeFind(void) const
{
	for (int index = 0; index < 4; ++index)
	{
		BfmeSlotDK *slot = m_bfmeSlots[index];

		if (slot != 0 && slot->m_bfmeOwner->m_bfmeReady)
			return slot;
	}

	return 0;
}

class BfmeThingDK
{
public:
	void *m_bfmeKey;					// +0x00
};

class Gen_00233D80
{
public:
	BfmeThingDK *bfmeFind(void *key) const;

private:
	int m_bfmeHead[137];					// +0x000
	BfmeThingDK **m_bfmeStart;				// +0x224
	BfmeThingDK **m_bfmeFinish;				// +0x228
};

// ?bfmeFind@Gen_00233D80@@QBEPAVBfmeThingDK@@PAX@Z
BfmeThingDK *Gen_00233D80::bfmeFind(void *key) const
{
	BfmeThingDK **it = m_bfmeStart;
	BfmeThingDK **finish = m_bfmeFinish;

	while (it != finish)
	{
		BfmeThingDK *thing = *it;

		if (key == thing->m_bfmeKey)
			return thing;

		++it;
	}

	return 0;
}
