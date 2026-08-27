// Four more: another counted handle, a bounded indexed read, and two copies
// written in assembly -- the register saves land where the first instruction
// that needs them is.

class BfmeThingCX
{
public:
	int m_bfmeHead;						// +0x00
	unsigned short m_bfmeRefs;				// +0x04
};

class BfmeHandleCX
{
public:
	BfmeHandleCX(void)
	{
		m_bfmeThing = 0;
	}

	BfmeHandleCX(const BfmeHandleCX &other)
	{
		BfmeThingCX *thing = other.m_bfmeThing;

		m_bfmeThing = thing;

		if (thing)
			++thing->m_bfmeRefs;
	}

	~BfmeHandleCX(void)
	{
		if (m_bfmeThing)
			--m_bfmeThing->m_bfmeRefs;
	}

	BfmeThingCX *m_bfmeThing;				// +0x00
};

class Gen_006D2660
{
public:
	BfmeHandleCX bfmeGet(void) const;

private:
	char m_bfmeHead[0x1C];					// +0x00
	BfmeHandleCX m_bfmeHandle;				// +0x1C
};

// ?bfmeGet@Gen_006D2660@@QBE?AVBfmeHandleCX@@XZ
BfmeHandleCX Gen_006D2660::bfmeGet(void) const
{
	return m_bfmeHandle;
}

class BfmePairDE
{
public:
	int m_bfmeFirst;					// +0x00
	int m_bfmeSecond;					// +0x04
};

class BfmeVecDE
{
public:
	int bfmeSize(void) const
	{
		return m_bfmeFinish - m_bfmeStart;
	}

	BfmePairDE &bfmeAt(int index) const
	{
		return m_bfmeStart[index];
	}

	BfmePairDE *m_bfmeStart;				// +0x00
	BfmePairDE *m_bfmeFinish;				// +0x04
};

class Gen_009CBC90
{
public:
	int bfmeAt(int index) const;

private:
	int m_bfmeHead[2];					// +0x00
	BfmeVecDE m_bfmeVector;					// +0x08
};

// ?bfmeAt@Gen_009CBC90@@QBEHH@Z
int Gen_009CBC90::bfmeAt(int index) const
{
	if (index < m_bfmeVector.bfmeSize())
		return m_bfmeVector.bfmeAt(index).m_bfmeSecond;

	return 0;
}

// The copy is the compiler's own memcpy, not hand-written assembly: only the
// intrinsic puts the register saves between the argument loads.
extern "C" void * __cdecl memcpy(void *destination, const void *source, unsigned int bytes);

#pragma intrinsic(memcpy)

// ?bfmeCopy@@YAXPBXIPAX@Z
void __cdecl bfmeCopy(const void *source, unsigned int bytes, void *destination)
{
	memcpy(destination, source, bytes);
}

// ?bfmeCopyMmx@@YAXPAXHH@Z
// "offset" is an assembler keyword, so the stride cannot carry that name.
void __cdecl bfmeCopyMmx(void *source, int stride, int bytes)
{
	__asm
	{
		mov esi, source
		mov ecx, stride
		mov edx, bytes
	bfmeNext:
		movq mm0, [esi]
		movq [esi+ecx], mm0
		add esi, 8
		sub edx, 8
		jg bfmeNext
	}
}
