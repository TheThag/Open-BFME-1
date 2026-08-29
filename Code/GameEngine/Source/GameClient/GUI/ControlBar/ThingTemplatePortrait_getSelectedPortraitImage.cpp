// The BFME portrait name is resolved once, then released after the image is
// cached. The StringBase header layout makes the empty-name test inline while
// its releaseBuffer body remains the established retail helper.

template <typename T>
class StringBase
{
friend class AsciiString;

private:
	void releaseBuffer();

	struct Header
	{
		int referenceCount;
		unsigned short length;
		unsigned short capacity;
		T data[1];
	};
	Header *m_data;
};

class AsciiString
{
public:
	bool isNotEmpty() const
	{
		return m_data != 0 && m_data->length != 0;
	}

	void clear()
	{
		((StringBase<char> *)this)->releaseBuffer();
	}

private:
	struct Header
	{
		int referenceCount;
		unsigned short length;
		unsigned short capacity;
		char data[1];
	};
	Header *m_data;
};

class Image;

class MappedImageCollection
{
public:
	const Image *findImageByName(const AsciiString &name);
};

class ThingTemplatePortraitShim
{
public:
	const Image *getSelectedPortraitImage(void) const;

private:
	char m_bfmeBeforePortraitName[0x34];
	AsciiString m_bfmePortraitName;
	char m_bfmeBeforeCachedPortrait[0x394 - 0x38];
	mutable const Image *m_bfmeCachedPortrait;
};

extern MappedImageCollection *TheMappedImageCollection;

// ?getSelectedPortraitImage@ThingTemplatePortraitShim@@QBEPBVImage@@XZ
const Image *ThingTemplatePortraitShim::getSelectedPortraitImage(void) const
	// retail body 0x0013EEC0
{
	if (m_bfmePortraitName.isNotEmpty() && TheMappedImageCollection != 0)
	{
		m_bfmeCachedPortrait = TheMappedImageCollection->findImageByName(
			m_bfmePortraitName);
		((AsciiString *)&m_bfmePortraitName)->clear();
	}
	return m_bfmeCachedPortrait;
}
