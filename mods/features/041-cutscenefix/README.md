# 041-cutscenefix — contain global fullscreen movies

## Verified retail path

The reconstructed game source shows the original generic global movie draw as

```cpp
drawVideoBuffer(m_videoBuffer, 0, 0, getWidth(), getHeight());
```

with `VideoBuffer::Rect(0, 0, 1, 1)`. That is a complete source frame mapped to
the complete display rectangle: it stretches the source image to the display
aspect ratio. It does not crop through UVs, and this path does not clear the
unused framebuffer area first.

The BFME retail binary has a more specific movie implementation than that
unmatched source body. The global `Display::playLogoMovie` body at runtime VA
`0x0080E3B0` stores the opened stream at `Display+0x34`. The movie update path
calls the `W3DDisplay` virtual at `+0x164` with mode `1` at runtime VA
`0x0080F968`. The implementation at runtime VA `0x00AEE0C0` then:

* reads the stream's height and width through its `+0x2C` and `+0x30` virtual
  methods, respectively;
* computes a vertical extent as `display_width * source_height /
  source_width`, with no clamp to the display height, so the original
  destination is a cover/fill calculation that can crop vertically;
* passes `Display+0xF8`, `+0xFC`, `+0x100`, and `+0x104` as the four destination
  coordinates to the matched `drawImage` slot at `W3DDisplay+0xE0`; and
* ends the rendering operation without first covering the complete framebuffer
  with black.

The matched `drawImage` body builds its source rectangle from the image's full
width and height, so the original path does not crop the source through a UV
adjustment. The verified failure in the active BFME retail path is therefore
cover/fill destination geometry with possible raster clipping, compounded by
the missing black clear; it is not source-UV cropping. The generic
reconstructed body is a separate full-frame stretch implementation and is not
the hook target.

The same helper is called with modes `0` and `3` by other stream-service code.
The hook therefore handles mode `1` only. `InGameUI`, window/cameo playback,
and `LoadScreen` own separate stream/buffer paths and are not routed through
this global mode-1 call.

## Fix

At the verified pre-draw site inside the mode-1 helper, `cutscenefix` gets the
live source and display dimensions, computes the largest centered integer
rectangle that fits without cropping, and writes those four proven destination
fields. It then
temporarily disables the retail image clip flag, queues an opaque black
full-display rectangle through the matched `drawFillRect` slot at `+0xC0`,
flushes it through the matched Render2D slot at `+0xDC`, and restores the clip
flag. The original retail movie draw follows the shim and uses the corrected
rectangle with its complete source image.

The fit is calculated from the dimensions at runtime. No display or movie
aspect ratio is hardcoded. The integer rounding can make one edge differ by at
most a pixel, while both destination dimensions remain inside the framebuffer.

The hook target is RVA `0x006EE185` (runtime VA `0x00AEE185`). It is the six-byte
`mov edx,[esi+0x104]` immediately before the retail helper loads the remaining
destination coordinates and calls the global movie image path. The displaced
instruction is replayed after the payload, so the original image call consumes
the corrected fields. The helper entry is RVA `0x006EE0C0` (runtime VA
`0x00AEE0C0`), but it is not the patch site: an entry hook is too early because
the retail update path recalculates the cover rectangle before this draw.
