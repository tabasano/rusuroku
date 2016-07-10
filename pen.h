#pragma once

HPEN penline = CreatePen(PS_SOLID, 1, RGB(220, 0, 100));   // Left channel wave
HPEN penline2 = CreatePen(PS_SOLID, 1, RGB(0, 200, 250));  // Right channel wave etc.
HPEN penlineC = CreatePen(PS_SOLID, 1, RGB(100, 100, 200));
HPEN penb = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
HPEN pen1 = CreatePen(PS_SOLID, 1, RGB(0, 55, 55));
HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(0, 155, 155));
HPEN pen3 = CreatePen(PS_SOLID, 1, RGB(0, 205, 205));
HPEN pen4 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
HPEN penfullwav = CreatePen(PS_SOLID, 1, RGB(20, 60, 255));
HPEN pendead = penfullwav;
