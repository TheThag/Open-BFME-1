#!/usr/bin/env python3
"""Evidence checks for ShellMap1.map using the existing 040 camera path."""
from pathlib import Path
import sys
from unittest import TestCase

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

from cave import PE  # noqa: E402

EXE = ROOT / "baselines/bfme1/workshop-vanilla-1.03/files/lotrbfme.exe"


def read(path):
    return (ROOT / path).read_text()


class ShellMapEvidenceTest(TestCase):
    def test_shellmap_is_selected_as_a_normal_game_shell_map(self):
        shell = read("Code/GameEngine/Source/GameClient/GUI/Shell/Shell.cpp")
        self.assertIn("TheWritableGlobalData->m_pendingFile = TheGlobalData->m_shellMapName", shell)
        self.assertIn("msg->appendIntegerArgument(GAME_SHELL)", shell)

        logic = read("Code/GameEngine/Source/GameLogic/System/GameLogic.cpp")
        self.assertIn("loadMapINI( TheGlobalData->m_mapName )", logic)
        self.assertIn("TheTerrainLogic->loadMap( TheGlobalData->m_mapName, false )", logic)


    def test_shellmap_uses_the_only_attached_3d_view(self):
        ui = read("Code/GameEngine/Source/GameClient/InGameUI.cpp")
        self.assertIn("TheTacticalView = createView();", ui)
        self.assertIn("TheDisplay->attachView( TheTacticalView );", ui)
        self.assertIn("TheTacticalView->setWidth( TheDisplay->getWidth());", ui)

        display = read("Code/GameEngine/Source/GameClient/Display.cpp")
        self.assertIn("for( View *v = m_viewList; v; v = v->getNextView() )", display)
        self.assertIn("v->drawView();", display)

        view = read("Code/GameEngineDevice/Source/W3DDevice/GameClient/W3DView.cpp")
        self.assertIn("W3DDisplay::m_3DScene->doRender( m_3DCamera );", view)


    def test_game_shell_makes_the_existing_view_full_display_height(self):
        logic = read("Code/GameEngine/Source/GameLogic/System/GameLogic.cpp")
        self.assertIn("if(m_gameMode == GAME_SHELL)", logic)
        self.assertIn("HideControlBar();", logic)

        controlbar = read("Code/GameEngine/Source/GameClient/GUI/GUICallbacks/ControlBarVisibility.cpp")
        self.assertIn("setHeight(\n\t\t\t\treinterpret_cast<BFMEDisplayVTable *>( TheDisplay )->getHeight() );", controlbar)


    def test_retail_view_assignment_and_040_hooks_are_binary_anchored(self):
        if not EXE.exists():
            return

        pe = PE(EXE)
        # InGameUI::init: W3DGameClient::createView, store to TheTacticalView,
        # then init/attach. These bytes are the retail assignment, not an
        # address inferred from source adjacency.
        self.assertEqual(pe.read(0x00440D98, 11),
                         b"\xff\x92\xb0\x01\x00\x00\xa3\x00\x16\x2f\x01")

        # The identity-filtered post-operation hooks remain the established
        # tactical-camera sites; ShellMap reaches this singleton rather than a
        # new camera address.
        self.assertEqual(pe.read(0x0073DC3E, 6), b"\x8b\x86\x04\x01\x00\x00")
        self.assertEqual(pe.read(0x0073DDF8, 5), b"\x5f\x5e\x83\xc4\x18")
        self.assertEqual(pe.read(0x00742609, 5), b"\xa1\xe0\x7f\x2f\x01")
        self.assertEqual(pe.read(0x00931304, 5), b"\x5e\xc2\x04\x00\xcc")


if __name__ == "__main__":
    import unittest

    unittest.main()
