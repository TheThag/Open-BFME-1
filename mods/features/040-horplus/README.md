# 040-horplus — tactical-camera Hor+

## Status

`040-horplus` is a development feature. It is registered as unshipped in
`tools/modbuild.py`, so it is not copied into `mods/dist`. Build it separately
with:

```text
PYTHONPATH=/tmp/capstone-py python3 tools/modbuild.py \
  --only 040-horplus -o build/mods/lotrbfme-040-horplus.exe
```

The feature changes the BFME tactical 3D camera only. It is separate from
`041-cutscenefix`, which handles fullscreen movie rectangles.

## What the retail camera does

BFME's `W3DView` maintains a `CameraClass` for the tactical/world view. The
retail view setup updates the camera's aspect ratio, viewport, view plane, and
transform as the view dimensions or camera position change. The readable BFME
layout reconstruction shows the relevant `setWidth` order:

```text
store W3DView width
  -> CameraClass::Set_Aspect_Ratio(width / viewHeight)
  -> update the normalized viewport right edge
  -> CameraClass::Set_View_Plane(width / displayWidth * 50 degrees, -1)
```

The retail projection is based on BFME's native 4:3 tactical framing. On a
wider framebuffer, leaving that plane unchanged narrows the horizontal world
view relative to the display. `040-horplus` preserves the native vertical
framing and reveals additional world horizontally as the display becomes
wider. It does not rescale 2D UI primitives or apply a global renderer
transform.

The 4:3 reference is a camera-design baseline, not a resolution assumption:
the current display dimensions are queried at runtime and the adjustment is
continuous.

## Payload calculation

The payload accepts the BFME `W3DView` in `ESI` and first filters it against
the verified global `TheTacticalView` object. It also requires the global
`TheDisplay` object, a valid 3D camera at `W3DView+0x104`, and non-zero current
dimensions.

For the tactical view it reads:

* display width and height through the `Display` virtuals at `+0x2C` and
  `+0x30`;
* the view width at `W3DView+0x18`; and
* the current view FOV at `W3DView+0x6C`.

It then performs the following operations:

```text
horizontal_fov = current_view_fov * view_width / display_width
CameraClass::Set_View_Plane(horizontal_fov, -1)

display_scale = (display_width / display_height) * (3 / 4)
camera plane min/max X and Y *= display_scale
CameraClass::FrustumValid = false
```

The call to the existing retail `CameraClass::Set_View_Plane` keeps the
engine's tangent/FOV calculation and the `-1` vertical-FOV convention. The
subsequent plane scaling makes the resulting plane equivalent to retaining
the 4:3 vertical extent while expanding the horizontal extent for a wider
display. Invalidating `FrustumValid` makes the renderer rebuild derived
frustum data from the new plane.

At 4:3, `display_scale` is exactly `1`, so the feature leaves the native
camera plane unchanged. At wider ratios it is greater than `1`; the tests
verify that the vertical FOV remains at the 4:3 reference while the
horizontal FOV increases.

## Hook sites

All four hooks are post-operation sites. The retail camera code runs first;
the payload only reapplies the final tactical camera plane and then the
relocated retail instructions resume.

| RVA | Path | Why this site is needed |
|---|---|---|
| `0x0073DC3E` | `W3DView::setHeight` | Catches height/aspect changes after the retail `Set_Aspect_Ratio` call. |
| `0x0073DDF8` | `W3DView::setWidth` | Catches width changes after the retail viewport and `Set_View_Plane` work. |
| `0x00742609` | BFME tactical camera-transform path | Catches the path that writes the plane, updates the transform, and can bypass the ordinary setters. |
| `0x00931304` | direct `CameraClass::Set_Transform` tail | Catches locked/scripted tactical-camera transforms; the payload accepts it only when `ESI` is the tactical view's camera. |

The first three sites receive `ESI` as the `W3DView`. The fourth receives
`ESI` as the `CameraClass` and performs the additional identity check through
`TheTacticalView+0x104`. This prevents an unrelated camera from being changed.

The direct retail tangent routine used by the payload is
`CameraClass::Set_View_Plane` at RVA `0x00931780` (runtime VA
`0x00D31780`). It is called rather than reimplementing the engine's camera
math.

## Scope and non-effects

The feature touches only the global tactical 3D camera plane. It does not
change:

* menus, HUD/control-bar geometry, APT or GameWindow coordinates;
* mouse/input coordinates or hit testing;
* fullscreen, windowed, cameo, or loading-screen movies;
* the 2D renderer, textures, UVs, or draw rectangles;
* world/UI rendering order; or
* the existing `041-cutscenefix` behavior.

The `TheTacticalView` filter is important because BFME has additional views
and camera-transform callers. A global `CameraClass` transform patch would
alter cameras outside the tactical world view.

## Verification

The feature's focused tests cover the native 4:3 case, wider display ratios,
tactical subview FOV behavior, and the increase in horizontal view. The mod
test also verifies the retail bytes around all four sites, the fixed
`Set_View_Plane` call address, the generated cave shims, exact stolen-prologue
replay, and the tactical-camera identity filter.

The normal MSVC 7.1 `/NODEFAULTLIB` build emits a small cave payload with no
CRT/runtime dependency. The feature is intentionally not promoted to
`FEATURES`; use `--only 040-horplus` for development builds.

Relevant source and tests:

* `src/horplus.cpp` — payload and BFME field/vtable evidence
* `tools/tests/test_horplus_fov.py` — projection math checks
* `tools/tests/test_horplus_mod.py` — cave, hook, and retail-byte checks
* `Code/GameEngineDevice/Source/W3DDevice/GameClient/W3DViewSetWidthBfmeLayout.cpp` — readable retail operation order

The repository has no game-runtime visual harness in this checkout, so the
verification is source/binary, numerical, and emitted-patch based rather than
an in-game screenshot comparison.
