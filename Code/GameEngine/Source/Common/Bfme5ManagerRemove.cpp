// cl: /DNDEBUG /MD /EHsc

class BfmeOwnedP
{
public:
    virtual ~BfmeOwnedP(void);
};

class BfmeManagerBucket
{
public:
    char m_bfmeHead[0x88];
    BfmeManagerBucket *m_bfmeNext;
};

class BfmeManagerP
{
public:
    void bfmeRemove(BfmeOwnedP *element);

private:
    char m_bfmeHead[0xA8];
    BfmeManagerBucket *m_bfmeFirst;
};

// The incremental-link thunk is a member helper with the retail call shape
// (bucket in ECX, element on the stack), despite its generated no-argument
// identity.
extern void j_0000b325(void);
struct BfmeBucketRemoveThunk
{
    void remove(BfmeOwnedP *element);
};

// The global at 0x012ED668 is shared by several recovered subsystem views.
// Slot 0x168 is the notification entry used by this manager.
struct BfmeNotifyThunk
{
    void notify(BfmeOwnedP *element);
};

static __forceinline void bfmeNotify(void *sink, BfmeOwnedP *element)
{
    void **vtable = *(void ***)sink;
    typedef void (BfmeNotifyThunk::*NotifyFn)(BfmeOwnedP *);
    union { void *asVoid; NotifyFn asMember; } fnCast;
    fnCast.asVoid = vtable[0x168 / 4];
    (reinterpret_cast<BfmeNotifyThunk *>(sink)->*fnCast.asMember)(element);
}

// ?bfmeRemove@BfmeManagerP@@QAEXPAVBfmeOwnedP@@@Z
void BfmeManagerP::bfmeRemove(BfmeOwnedP *element)
{
    BfmeManagerBucket *bucket = m_bfmeFirst;

    while (bucket)
    {
        typedef void (BfmeBucketRemoveThunk::*RemoveFn)(BfmeOwnedP *);
        union { void *asVoid; RemoveFn asMember; } fnCast;
        fnCast.asVoid = reinterpret_cast<void *>(j_0000b325);
        (reinterpret_cast<BfmeBucketRemoveThunk *>(bucket)->*fnCast.asMember)(element);
        bucket = bucket->m_bfmeNext;
    }

    bfmeNotify(*(void **)0x012ED668, element);
}
