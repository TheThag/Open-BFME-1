#!/usr/bin/env python3
"""Focused tests for cutscenefix's integer aspect-fit contract."""
from unittest import TestCase


def fit(source_width, source_height, display_width, display_height):
    """Python model of the CRT-free integer fit used by the payload."""
    if min(source_width, source_height, display_width, display_height) <= 0:
        return None

    if source_width * display_height >= display_width * source_height:
        width = display_width
        height = display_width * source_height // source_width
    else:
        height = display_height
        width = display_height * source_width // source_height
    return ((display_width - width) // 2, (display_height - height) // 2,
            width, height)


CASES = (
    ("4:3 source on 16:9", 640, 480, 1920, 1080, (240, 0, 1440, 1080)),
    ("16:9 source on 4:3", 1920, 1080, 1024, 768, (0, 96, 1024, 576)),
    ("16:9 source on 21:9", 1920, 1080, 2560, 1080, (320, 0, 1920, 1080)),
    ("16:9 source on 32:9", 1920, 1080, 3840, 1080, (960, 0, 1920, 1080)),
    ("matching aspect", 1920, 1080, 2560, 1440, (0, 0, 2560, 1440)),
    ("odd resolution", 853, 479, 1365, 777, None),
)


class CutsceneFitTest(TestCase):
    def test_representative_rectangles(self):
        for name, sw, sh, dw, dh, expected in CASES:
            with self.subTest(name=name):
                rect = fit(sw, sh, dw, dh)
                if expected is not None:
                    self.assertEqual(rect, expected)
                else:
                    self.assertIsNotNone(rect)
                left, top, width, height = rect
                self.assertLessEqual(width, dw)
                self.assertLessEqual(height, dh)
                self.assertLessEqual(abs((dw - width) - 2 * left), 1)
                self.assertLessEqual(abs((dh - height) - 2 * top), 1)

                # The complete source is visible; integer rounding is no more
                # than one source-pixel edge away from the exact ratio.
                self.assertLessEqual(abs(width * sh - height * sw), max(sw, sh))

    def test_odd_case_has_no_predefined_aspect_dependency(self):
        rect = fit(853, 479, 1365, 777)
        self.assertEqual(rect, (0, 5, 1365, 766))

    def test_all_fit_dimensions_are_positive_and_inside_display(self):
        for sw, sh, dw, dh in (
                (1, 1, 7, 11), (11, 7, 7, 11), (11, 7, 13, 5),
                (409, 271, 1377, 913), (1377, 913, 409, 271)):
            with self.subTest(source=(sw, sh), display=(dw, dh)):
                left, top, width, height = fit(sw, sh, dw, dh)
                self.assertGreater(width, 0)
                self.assertGreater(height, 0)
                self.assertGreaterEqual(left, 0)
                self.assertGreaterEqual(top, 0)
                self.assertLessEqual(left + width, dw)
                self.assertLessEqual(top + height, dh)


if __name__ == "__main__":
    import unittest

    unittest.main()
