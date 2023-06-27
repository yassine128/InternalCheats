#pragma once
#include "common.h"
#include <windowsx.h>

static COLORREF redColor = RGB(255, 0, 0);
static COLORREF blueColor = RGB(0, 0, 255);
static COLORREF greenColor = RGB(0, 255, 0);

static inline void drawEnemy(int x, int y, COLORREF color, HDC hdc) {
    double size = 10; // Length of the base

    // Create the Square vertices
    POINT vertex1 = { static_cast<LONG>(x), static_cast<LONG>(y) };
    POINT vertex2 = { static_cast<LONG>(x + size), static_cast<LONG>(y) };
    POINT vertex3 = { static_cast<LONG>(x), static_cast<LONG>(y + size) };
    POINT vertex4 = { static_cast<LONG>(x + size), static_cast<LONG>(y + size) };

    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HPEN hOldPen = SelectPen(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = SelectBrush(hdc, hBrush);

    POINT vertices[] = {
        { vertex1.x, vertex1.y },
        { vertex2.x, vertex2.y },
        { vertex4.x, vertex4.y },
        { vertex3.x, vertex3.y }
    };

    // Draw the triangle
    Polygon(hdc, vertices, sizeof(vertices) / sizeof(vertices[0]));

    SelectBrush(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectPen(hdc, hOldPen);
    DeleteObject(hPen);
}

static inline void drawPlayer(int x, int y, COLORREF color, HDC hdc, double angle) {
    double baseLength = 5; // Length of the base

    int centerX = SCREEN_X / 2; 
    int centerY = SCREEN_Y / 2; 

    // Create the triangle vertices
    POINT vertex1 = { static_cast<LONG>(centerX), static_cast<LONG>(centerY - baseLength) };
    POINT vertex2 = { static_cast<LONG>(centerX + 2), static_cast<LONG>(centerY + 5) };
    POINT vertex3 = { static_cast<LONG>(centerX - 2), static_cast<LONG>(centerY + 5) };

    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HPEN hOldPen = SelectPen(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = SelectBrush(hdc, hBrush);

    POINT vertices[] = {
        { vertex1.x, vertex1.y },
        { vertex2.x, vertex2.y },
        { vertex3.x, vertex3.y }
    };

    // Draw the triangle
    Polygon(hdc, vertices, sizeof(vertices) / sizeof(vertices[0]));

    SelectBrush(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectPen(hdc, hOldPen);
    DeleteObject(hPen);
}

static inline void drawRadarSight(HDC hdc) {
    int centerX = SCREEN_X / 2; 
    int centerY = SCREEN_Y / 2; 

    POINT vertex1 = { static_cast<LONG>(0), static_cast<LONG>(centerY) };
    POINT vertex2 = { static_cast<LONG>(SCREEN_X), static_cast<LONG>(centerY) };

    POINT vertex3 = { static_cast<LONG>(centerX), static_cast<LONG>(0) };
    POINT vertex4 = { static_cast<LONG>(centerX), static_cast<LONG>(SCREEN_Y) };

    HPEN hPen = CreatePen(PS_SOLID, 2, blueColor);
    HPEN hOldPen = SelectPen(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(blueColor);
    HBRUSH hOldBrush = SelectBrush(hdc, hBrush);

    POINT vertices[] = {
        { vertex1.x, vertex1.y },
        { vertex2.x, vertex2.y }
    };

    POINT vertices1[] = {
        { vertex3.x, vertex3.y },
        { vertex4.x, vertex4.y }
    };
    Polygon(hdc, vertices, sizeof(vertices) / sizeof(vertices[0]));
    Polygon(hdc, vertices1, sizeof(vertices1) / sizeof(vertices1[0]));

    SelectBrush(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectPen(hdc, hOldPen);
    DeleteObject(hPen);
}