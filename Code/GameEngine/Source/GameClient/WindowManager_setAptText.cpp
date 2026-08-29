// cl: /DNDEBUG /DWIN32 /MD /D_STLP_USE_STATIC_LIB
// stlport

// Set the Unicode payload for an APT variable and notify its listener.
// The named-record hash table lives at WindowManager +0x44.  Its 8-byte value
// is the listener pointer followed by the mutable UnicodeString payload.

#define _STLP_NO_EXCEPTIONS 1
#include <hash_map>

#include "../../../Libraries/Source/WWVegas/WWLib/string_base.h"

// The retail UnicodeString default constructor is visible to this STLport
// instantiation as a one-dword zeroing inline.  Keep that constructor inline
// here; its copy/assignment/destructor remain the existing library bodies.
class UnicodeString
{
public:
	UnicodeString() : m_text( 0 ) {}
	UnicodeString( const UnicodeString &other );
	~UnicodeString();
	UnicodeString &operator=( const UnicodeString &other );

private:
	wchar_t *m_text;
};

// The key is the upstream StringBase<char> wrapper used by the adjacent
// hash-map instantiations.  Its copy constructor is inline, while destruction
// is supplied by the retail AsciiString body.
class AsciiString
{
public:
	// A single StringBase<char> member preserves the one-word retail layout.
	AsciiString( const AsciiString &other ) : m_string( other.m_string ) {}
	~AsciiString();

	StringBase<char> m_string;
};

inline bool operator==( const AsciiString &left, const AsciiString &right )
{
	return left.m_string == right.m_string;
}

namespace rts
{
	template <typename T> struct hash
	{
		unsigned int operator()( T value ) const;
	};
}

struct AptTextRecord
{
	void *m_listener;
	UnicodeString m_text;

	AptTextRecord() : m_listener( 0 ), m_text() {}
	// The retail temporary cleanup clears the listener dword before releasing
	// its UnicodeString; the field is already dead at this point.
	~AptTextRecord() { m_listener = 0; }
};

typedef _STL::hash_map<AsciiString, AptTextRecord, rts::hash<AsciiString> > AptTextMap;

// retail 0x0046B490, reached through ILT 0x00041BDC.  The value-pair
// constructor and destructor are the existing APT map helpers at 0x00467890
// and 0x00467710; the table find/insert bodies are 0x0046A3A0/0x0046AC80.
template AptTextRecord &AptTextMap::operator[]( const AsciiString & );

class AptTextListener
{
public:
	virtual void slot00();
	virtual void setText( const UnicodeString &text );
};

class WindowManager
{
public:
	void bfme_setAptText( const AsciiString &name, const UnicodeString &text );

private:
	char m_bfmeHead[0x44];
	AptTextMap m_bfmeAptText;
};

// ?bfme_setAptText@WindowManager@@QAEXABVAsciiString@@ABVUnicodeString@@@Z
void WindowManager::bfme_setAptText( const AsciiString &name, const UnicodeString &text )
{
	AptTextRecord &record = m_bfmeAptText[ name ];
	record.m_text = text;
	if ( record.m_listener != 0 )
		((AptTextListener *)record.m_listener)->setText( record.m_text );
}
