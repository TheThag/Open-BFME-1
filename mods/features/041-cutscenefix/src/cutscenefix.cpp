// 041-cutscenefix — aspect-preserving presentation for global movies.
//
// This payload runs at the entry of BFME's retail W3DDisplay movie-render
// helper.  The hook is deliberately filtered to its mode-1 call: the global
// Display movie path calls the helper with 1, while the other stream-service
// paths use 0 or 3.  Window/cameo and load-screen movies do not call this
// helper with the global Display object.

typedef int Int;
typedef unsigned int UnsignedInt;
typedef float Real;

extern "C" const int _fltused = 0;

enum
{
	DISPLAY_GET_WIDTH_SLOT = 0x2C / 4,
	DISPLAY_GET_HEIGHT_SLOT = 0x30 / 4,
	DISPLAY_IS_CLIPPING_SLOT = 0x8C / 4,
	DISPLAY_ENABLE_CLIPPING_SLOT = 0x90 / 4,
	DISPLAY_DRAW_FILL_RECT_SLOT = 0xC0 / 4,
	DISPLAY_RENDER_2D_SLOT = 0xDC / 4,

	DISPLAY_VIDEO_STREAM = 0x34,
	MOVIE_LEFT = 0xF8,
	MOVIE_TOP = 0xFC,
	MOVIE_RIGHT = 0x100,
	MOVIE_BOTTOM = 0x104
};

// These are the documented VC7.1 spellings for thiscall methods: this in ECX,
// an unused EDX, and explicit arguments on the stack.
typedef UnsignedInt(__fastcall *DimensionFunction)(void *self, void *edx);
typedef void(__fastcall *EnableClippingFunction)(void *self, void *edx, Int onoff);
typedef void(__fastcall *FillRectFunction)(void *self, void *edx, Real x, Real y,
	Real width, Real height, UnsignedInt color);
typedef void(__fastcall *RenderFunction)(void *self, void *edx);

struct MovieRect
{
	UnsignedInt left;
	UnsignedInt top;
	UnsignedInt width;
	UnsignedInt height;
};

static void *read_pointer(void *base, Int offset)
{
	return *(void **)((unsigned char *)base + offset);
}

static Real *read_real(void *base, Int offset)
{
	return (Real *)((unsigned char *)base + offset);
}

static UnsignedInt display_dimension(void *display, Int slot)
{
	DimensionFunction function = (DimensionFunction)(*(void ***)display)[slot];
	return function(display, 0);
}

// Compare a/b with c/d without a 64-bit multiply.  The continued-fraction
// form matters here because the payload is linked without the CRT, so using a
// 64-bit cross-product would require an unresolved compiler helper on VC7.1.
static Int compare_fractions(UnsignedInt a, UnsignedInt b,
	UnsignedInt c, UnsignedInt d)
{
	Int direction = 1;
	for (;;)
	{
		UnsignedInt a_quotient = a / b;
		UnsignedInt c_quotient = c / d;
		if (a_quotient != c_quotient)
			return direction * (a_quotient < c_quotient ? -1 : 1);

		UnsignedInt a_remainder = a % b;
		UnsignedInt c_remainder = c % d;
		if (a_remainder == 0 || c_remainder == 0)
		{
			if (a_remainder == c_remainder)
				return 0;
			return direction * (a_remainder == 0 ? -1 : 1);
		}

		a = b;
		b = a_remainder;
		c = d;
		d = c_remainder;
		direction = -direction;
	}
}

// floor(numerator * multiplier / denominator), split so the quotient part is
// already known to fit in the resulting display dimension for real screens.
static UnsignedInt scaled_dimension(UnsignedInt numerator,
	UnsignedInt multiplier, UnsignedInt denominator)
{
	UnsignedInt quotient = numerator / denominator;
	UnsignedInt remainder = numerator % denominator;
	return quotient * multiplier + (remainder * multiplier) / denominator;
}

static int aspect_fit(UnsignedInt source_width, UnsignedInt source_height,
	UnsignedInt display_width, UnsignedInt display_height, MovieRect *result)
{
	if (source_width == 0 || source_height == 0 || display_width == 0 ||
		display_height == 0 || result == 0)
		return 0;

	if (compare_fractions(source_width, source_height,
		display_width, display_height) >= 0)
	{
		result->width = display_width;
		result->height = scaled_dimension(display_width, source_height, source_width);
	}
	else
	{
		result->height = display_height;
		result->width = scaled_dimension(display_height, source_width, source_height);
	}

	if (result->width == 0 || result->height == 0 ||
		result->width > display_width || result->height > display_height)
		return 0;

	result->left = (display_width - result->width) / 2;
	result->top = (display_height - result->height) / 2;
	return 1;
}

static void clear_movie_framebuffer(void *display, UnsignedInt width,
	UnsignedInt height)
{
	void **vtable = *(void ***)display;
	DimensionFunction is_clipping =
		(DimensionFunction)vtable[DISPLAY_IS_CLIPPING_SLOT];
	EnableClippingFunction enable_clipping =
		(EnableClippingFunction)vtable[DISPLAY_ENABLE_CLIPPING_SLOT];
	UnsignedInt was_clipping = is_clipping(display, 0);
	if (was_clipping)
		enable_clipping(display, 0, 0);

	// drawFillRect is the matched retail W3DDisplay solid-rectangle path.  Its
	// rectangle is queued in Render2D, so flush that queue before the movie's
	// drawImage call.  The opaque black color is the game's normal ARGB layout.
	FillRectFunction fill_rect =
		(FillRectFunction)vtable[DISPLAY_DRAW_FILL_RECT_SLOT];
	fill_rect(display, 0, 0.0f, 0.0f, (Real)width, (Real)height, 0xFF000000);

	RenderFunction render = (RenderFunction)vtable[DISPLAY_RENDER_2D_SLOT];
	render(display, 0);
	if (was_clipping)
		enable_clipping(display, 0, 1);
}

static void set_movie_rect(void *display, const MovieRect &rect)
{
	*read_real(display, MOVIE_LEFT) = (Real)rect.left;
	*read_real(display, MOVIE_TOP) = (Real)rect.top;
	*read_real(display, MOVIE_RIGHT) = (Real)(rect.left + rect.width);
	*read_real(display, MOVIE_BOTTOM) = (Real)(rect.top + rect.height);
}

// RVA 0x006EE0C0 / runtime VA 0x00AEE0C0.  The retail body returns with
// ret 4, so its entry argument is the mode dword copied by cave.py's
// stack:0 shim.
extern "C" __declspec(dllexport)
void __cdecl cutscenefix_render_movie(void *display, Int mode)
{
	if (display == 0 || mode != 1)
		return;

	// Display+0x34 is the global movie's active VideoStreamInterface.  In the
	// BFME stream interface the proven +0x2C/+0x30 methods are height and width,
	// respectively (the order used by the retail aspect calculation too).
	void *stream = read_pointer(display, DISPLAY_VIDEO_STREAM);
	if (stream == 0)
		return;

	UnsignedInt display_width =
		display_dimension(display, DISPLAY_GET_WIDTH_SLOT);
	UnsignedInt display_height =
		display_dimension(display, DISPLAY_GET_HEIGHT_SLOT);
	if (display_width == 0 || display_height == 0)
		return;

	DimensionFunction stream_height =
		(DimensionFunction)(*(void ***)stream)[0x2C / 4];
	DimensionFunction stream_width =
		(DimensionFunction)(*(void ***)stream)[0x30 / 4];
	UnsignedInt source_height = stream_height(stream, 0);
	UnsignedInt source_width = stream_width(stream, 0);

	// Clear even if a frame's dimensions are temporarily unavailable.  The
	// global movie path must not expose the world/UI or a previous frame in the
	// area outside the next valid movie rectangle.
	clear_movie_framebuffer(display, display_width, display_height);

	MovieRect rect;
	if (aspect_fit(source_width, source_height, display_width, display_height, &rect))
		set_movie_rect(display, rect);
}
