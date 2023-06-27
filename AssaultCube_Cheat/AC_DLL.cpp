#include "offsets.h"
#include "playerObject.h"
#include "UI.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::size_t MaxPlayers;

playerObject* myPlayer;
std::vector<playerObject*> listOfPlayers;

// Function to retrieve the base address of the executable
DWORD_PTR GetExecutableBaseAddress()
{
    HMODULE hModule = GetModuleHandleA("ac_client.exe");  // or GetModuleHandleW(NULL) for wide strings
    return (DWORD_PTR)hModule;
}

void ClearConsole()
{
    COORD topLeft = { 0, 0 };
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    DWORD cellsWritten;
    DWORD consoleSize;

    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
    consoleSize = screenBufferInfo.dwSize.X * screenBufferInfo.dwSize.Y;

    // Fill the console with spaces
    FillConsoleOutputCharacter(consoleHandle, ' ', consoleSize, topLeft, &cellsWritten);
    // Reset the cursor position to the top left
    SetConsoleCursorPosition(consoleHandle, topLeft);
}

void findMyPlayer() {
    DWORD_PTR baseAddress = GetExecutableBaseAddress();
    auto localPlayer = reinterpret_cast<std::uintptr_t*>(baseAddress + offsets::dwLocalPlayer);
    myPlayer = new playerObject{
        reinterpret_cast<int*>(*localPlayer + offsets::pHealth),
        reinterpret_cast<float*>(*localPlayer + offsets::pSouthNorth),
        reinterpret_cast<float*>(*localPlayer + offsets::pWestEast),
        reinterpret_cast<char*>(*localPlayer + offsets::pName),
        reinterpret_cast<float*>(*localPlayer + offsets::viewAngle)
    };
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcBuffer;
    static HBITMAP hbmBuffer;
    static RECT clientRect;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        hdcBuffer = nullptr;
        hbmBuffer = nullptr;
        GetClientRect(hwnd, &clientRect);
        return 0;
    }
    case WM_SIZE:
    {
        // Destroy previous buffer
        if (hdcBuffer != nullptr) {
            SelectObject(hdcBuffer, hbmBuffer);
            DeleteObject(hbmBuffer);
            DeleteDC(hdcBuffer);
        }

        // Create new buffer
        HDC hdc = GetDC(hwnd);
        hdcBuffer = CreateCompatibleDC(hdc);
        hbmBuffer = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        ReleaseDC(hwnd, hdc);

        SelectObject(hdcBuffer, hbmBuffer);

        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Paint to off-screen buffer
        HDC hdcMem = hdcBuffer;
        HBITMAP hbmMem = hbmBuffer;

        RECT windowRect;
        GetClientRect(hwnd, &windowRect);

        FillRect(hdcMem, &windowRect, (HBRUSH)(COLOR_WINDOWTEXT + 1));

        int centerX = SCREEN_X / 2;
        int centerY = SCREEN_Y / 2; 

        for (playerObject* p : listOfPlayers) {
            if (p != nullptr) {
                if (p->name != myPlayer->name) {
                    int distX = (int)*(p->westPos) - (int)*(myPlayer->westPos);
                    int distY = (int)*(p->southPos) - (int)*(myPlayer->southPos);

                    int hyp = static_cast<int>(std::sqrt(std::pow(distX, 2) + std::pow(distY, 2)));

                    int objectAngle = static_cast<int>(std::atan2(distY, distX) * (180.0 / M_PI));
                    int playerRotation = static_cast<int>(*(myPlayer->viewAngle)); // Assuming the player's view angle is stored as a float or double
                    int relativeAngle = (objectAngle - playerRotation) % 360;
                    if (relativeAngle < 0) {
                        relativeAngle += 360;
                    }

                    int newX = hyp * std::cos(relativeAngle * (M_PI / 180.0));
                    int newY = hyp * std::sin(relativeAngle * (M_PI / 180.0));

                    drawEnemy(centerX + newX, centerY + newY, redColor, hdcMem);
                }
            }
        }
        drawRadarSight(hdc); 
        drawPlayer(centerX, centerY, greenColor, hdcMem, 360 - *(myPlayer->viewAngle));

        // Copy the off-screen buffer to the window
        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
    {
        // Clean up buffer
        SelectObject(hdcBuffer, hbmBuffer);
        DeleteObject(hbmBuffer);
        DeleteDC(hdcBuffer);

        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void fillList() {
    listOfPlayers.clear();

    DWORD_PTR baseAddress = GetExecutableBaseAddress();
    uintptr_t* baseList = reinterpret_cast<std::uintptr_t*>(baseAddress + offsets::entityList);
    MaxPlayers = *reinterpret_cast<std::uintptr_t*>(baseAddress + offsets::nPlayers) - 1;

    for (int i = 0; i < MaxPlayers; i++) {
        auto entity = reinterpret_cast<std::uintptr_t*>(((i + 1) * 0x04) + *baseList); // Pointer to the next player
        listOfPlayers.push_back(
            new playerObject{
                reinterpret_cast<int*>(*entity + offsets::pHealth),
                reinterpret_cast<float*>(*entity + offsets::pSouthNorth),
                reinterpret_cast<float*>(*entity + offsets::pWestEast),
                reinterpret_cast<char*>(*entity + offsets::pName),
                reinterpret_cast<float*>(*entity + offsets::viewAngle)
            });

    }
}

BOOL radar() {

    *(myPlayer->viewAngle) = 0; // Reset le joueur à 0

    HWND hwnd;
    MSG msg;
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = (LPCSTR)L"WindowClass";

    if (!RegisterClass(&wc))
        return -1;

    hwnd = CreateWindowEx(
        0,
        (LPCSTR)L"WindowClass",
        (LPCSTR)L"Hello World",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        640,
        480,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd)
        return -1;

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Event loop
    while (true)
    {
        fillList(); 
        // Check for messages in the queue
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Process the message
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Break out of the loop if WM_QUIT message is received
        if (msg.message == WM_QUIT)
            break;

        // Update game logic and player positions here

        // Force a repaint of the window
        InvalidateRect(hwnd, nullptr, FALSE);
        UpdateWindow(hwnd);
    }

    return 0;
}

void displayUserInfo(const HINSTANCE hinstDLL) {
    AllocConsole();
    FILE* pConsole;
    freopen_s(&pConsole, "CONOUT$", "w", stdout);

    std::string previousDisplay;

    while (!(GetAsyncKeyState('P') & 0x8000)) {
        std::string currentDisplay;
        for (playerObject* p : listOfPlayers) {
            if (p != nullptr)
                currentDisplay += p->getPlayerDetails();
        }

        if (currentDisplay != previousDisplay)
        {
            ClearConsole();
            std::cout << currentDisplay << std::endl;
            previousDisplay = currentDisplay;
        }
    }

    if (pConsole != nullptr)
        fclose(pConsole);
    FreeConsole();
    FreeLibraryAndExitThread(hinstDLL, 0);
}

void InfiniteHealth(const HINSTANCE hinstDLL) { 
    DWORD_PTR baseAddress = GetExecutableBaseAddress();
    const auto pLocalPlayer = *reinterpret_cast<std::uintptr_t*>(baseAddress + offsets::dwLocalPlayer);
    auto& health = *reinterpret_cast<int*>(pLocalPlayer + offsets::pHealth);
    
    while (!(GetAsyncKeyState('P') & 0x8000)) {

        //std::this_thread::sleep_for(std::chrono::milliseconds(1)); 

        // Execute cheat here
        health = 1000; 
    }
    
    // unload DLL 
    FreeLibraryAndExitThread(hinstDLL, 0);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        fillList(); 
        findMyPlayer(); 
        radar();
        //displayUserInfo(hinstDLL); 
        //FIND_ENEMY(hinstDLL); 
        FreeLibraryAndExitThread(hinstDLL, 0);
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}