#include "Gamerules.hpp"
#include "Living_cells.hpp"
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include "basewin.h"
#include <map>
/*Linker instructions*/
#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")

using namespace std;
typedef vector<pair<int, int>> LISTPOS;

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public BaseWindow<MainWindow>
{
    //Pointers to the graphic ressources
    ID2D1Factory            *pFactory;
    ID2D1HwndRenderTarget   *pRenderTarget;
    ID2D1SolidColorBrush    *pBrush;
    ID2D1SolidColorBrush* pBrushBlack;
    IDWriteFactory    *pDWriteFactory;
    IDWriteTextFormat* pTextFormat;
    HRESULT CreateGraphicsResources();
    HRESULT CreateDeviceIndependentResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();
    //Pointer to a Living_cells instance
    Living_cells *p_living_cells; 
    //Point at which the user will look at
    pair<float, float> m_center;
    //Number of cells we want to plot on the screen
    int n_cells;
    //Icon
    HICON hIcon;
    //Name of the users choice of configuration
    WCHAR* m_shape_name;
    //Length of the name of the configuration (because we need to use some WCHAR)
    int m_name_len; 

public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), n_cells(40), pBrushBlack(NULL), pDWriteFactory(NULL), pTextFormat(NULL)
    {
        //Main window constructor : the default configuration is the blinker
        p_living_cells = new Living_cells();
        m_center = make_pair(0, 0);
        m_shape_name = L"Blinker";
        m_name_len = 7;
    }
    PCWSTR  ClassName() const { return L"Game of life"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

HRESULT MainWindow::CreateGraphicsResources()
{
    //Create brushes and render target
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);
        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(0,1.0f,0);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
            const D2D1_COLOR_F black = D2D1::ColorF(0,0,0);
            hr = pRenderTarget->CreateSolidColorBrush(black, &pBrushBlack);
        }
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(pDWriteFactory),
            reinterpret_cast<IUnknown**>(&pDWriteFactory)
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        static const WCHAR msc_fontName[] = L"Verdana";
        static const FLOAT msc_fontSize = 30;
        hr = pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &pTextFormat
        );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    //Destructor of the graphic resources
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
    SafeRelease(&pBrushBlack);
    SafeRelease(&pFactory);
    SafeRelease(&pTextFormat);
    SafeRelease(&pDWriteFactory);
}

void MainWindow::OnPaint()
{
    //Function that does all the drawings
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::White) );
        D2D1_SIZE_F size = pRenderTarget->GetSize();
        //Get all the positions from the back-end game of life
        LISTPOS positions = p_living_cells->get_cells_to_frontend(); 
        //Draw the corresponding rectangles, only if they need to be drawn
        for (int i = 0; i < positions.size(); i++) {
            if ((positions[i].first + m_center.first) * size.width / n_cells >= 0 && (positions[i].first + m_center.first + 1) * size.width / n_cells <= size.width && (positions[i].second + m_center.second) * size.height / n_cells >=0 && (positions[i].second + 1 + m_center.second) * size.height / n_cells <= size.height) {
                pRenderTarget->FillRectangle(D2D1::RectF((positions[i].first + m_center.first) * size.width / n_cells, (positions[i].second + m_center.second) * size.height / n_cells, (positions[i].first + 1 + m_center.first) * size.width / n_cells, (positions[i].second + 1 + m_center.second) * size.height / n_cells), pBrush);
                pRenderTarget->DrawRectangle(D2D1::RectF((positions[i].first + m_center.first) * size.width / n_cells, (positions[i].second + m_center.second) * size.height / n_cells, (positions[i].first + 1 + m_center.first) * size.width / n_cells, (positions[i].second + 1 + m_center.second) * size.height / n_cells), pBrushBlack);
            }
        }
        //Text drawing : overall, this draws the number of generations done and the name of the chosen configuration
        WCHAR sc_message[] = L"Generation n°XXXXXXXX";
        int n_generation = p_living_cells->get_generation();
        string str_gen = to_string(n_generation);
        for (int i = 0; i < 8; i++) {
            if (i < str_gen.size()) {
                sc_message[ARRAYSIZE(sc_message) - 2 - i] = str_gen[str_gen.size() - 1 - i];
            }
            else {
                sc_message[ARRAYSIZE(sc_message) - 2 - i] = ' ';
            }
        }
        pRenderTarget->DrawText(
            sc_message,
            ARRAYSIZE(sc_message) - 1,
            pTextFormat,
            D2D1::RectF(size.width-400, size.height-60, size.width-20, size.height-20),
            pBrushBlack
        );
        pRenderTarget->DrawText(
            m_shape_name,
            m_name_len,
            pTextFormat,
            D2D1::RectF(size.width - 400, size.height - 100, size.width - 20, size.height - 60),
            pBrushBlack
        );
        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
        pRenderTarget->Resize(size);
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;
    if (!win.Create(L"Game of life", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }
    ShowWindow(win.Window(), nCmdShow);
    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //Message handling function
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(m_hwnd, GWLP_HINSTANCE);
    switch (uMsg)
    {
    case WM_CREATE:
        SetWindowPos(m_hwnd, NULL, 0, 0, 1000, 1000, SWP_NOMOVE | SWP_NOZORDER);
        hIcon = (HICON)LoadImage(hInstance, L"icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
        SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        if (FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        return 0;
    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        delete p_living_cells;
        return 0;
    case WM_PAINT:
        OnPaint();
        return 0;
    case WM_KEYDOWN :
        //Handles all users keyboard inputs
        switch (wParam) {
        case VK_UP :
            m_center.second++;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break; 
        case VK_DOWN:
            m_center.second--;
            InvalidateRect(m_hwnd, NULL, TRUE); 
            break;
        case VK_LEFT:
            m_center.first++;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_RIGHT:
            m_center.first--;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_ADD : 
            n_cells--;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_SUBTRACT : 
            n_cells++;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break; 
        case VK_NUMPAD0:
            p_living_cells->set_living_from_keyboard('a');
            m_shape_name = L"Block";
            m_name_len = 5;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break; 
        case VK_NUMPAD1:
            p_living_cells->set_living_from_keyboard('b');
            m_shape_name = L"Beehive";
            m_name_len = 7;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD2:
            p_living_cells->set_living_from_keyboard('c');
            m_shape_name = L"Loaf";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD3:
            p_living_cells->set_living_from_keyboard('d');
            m_shape_name = L"Boat";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD4:
            p_living_cells->set_living_from_keyboard('e');
            m_shape_name = L"Tub";
            m_name_len = 3;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD5:
            p_living_cells->set_living_from_keyboard('f');
            m_shape_name = L"Cthulhu";
            m_name_len = 7;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD6:
            p_living_cells->set_living_from_keyboard('g');
            m_shape_name = L"Blinker";
            m_name_len = 7;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD7:
            p_living_cells->set_living_from_keyboard('h');
            m_shape_name = L"Toad";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD8:
            p_living_cells->set_living_from_keyboard('i');
            m_shape_name = L"Beacon";
            m_name_len = 6;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_NUMPAD9:
            p_living_cells->set_living_from_keyboard('j');
            m_shape_name = L"Pulsar";
            m_name_len = 6;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F1:
            p_living_cells->set_living_from_keyboard('k');
            m_shape_name = L"Penta-decathlon";
            m_name_len = 15;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F2:
            p_living_cells->set_living_from_keyboard('l');
            m_shape_name = L"Glider";
            m_name_len = 6;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F3:
            p_living_cells->set_living_from_keyboard('m');
            m_shape_name = L"LWSS";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F4:
            p_living_cells->set_living_from_keyboard('n');
            m_shape_name = L"MWSS";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F5:
            p_living_cells->set_living_from_keyboard('o');
            m_shape_name = L"HWSS";
            m_name_len = 4;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;

        case VK_F6:
            p_living_cells->set_living_from_keyboard('p');
            m_shape_name = L"Canada goose";
            m_name_len = 12;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F7:
            p_living_cells->set_living_from_keyboard('q');
            m_shape_name = L"60P5H2V0";
            m_name_len = 8;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F8:
            p_living_cells->set_living_from_keyboard('r');
            m_shape_name = L"Smoker";
            m_name_len = 6;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F9:
            p_living_cells->set_living_from_keyboard('s');
            m_shape_name = L"Gosper glider gun";
            m_name_len = 17;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_DECIMAL:
            p_living_cells->set_living_from_keyboard('t');
            m_shape_name = L"Simkin glider gun";
            m_name_len = 17 ;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F11:
            p_living_cells->set_living_from_keyboard('u');
            m_shape_name = L"1 hive to 4";
            m_name_len = 11;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_F12:
            p_living_cells->set_living_from_keyboard('v');
            m_shape_name = L"Stairs";
            m_name_len = 6;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_MULTIPLY:
            p_living_cells->set_living_from_keyboard('w');
            m_shape_name = L"Explosion";
            m_name_len = 9;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_DIVIDE:
            p_living_cells->set_living_from_keyboard('x');
            m_shape_name = L"Expansion";
            m_name_len = 9;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_SHIFT:
            p_living_cells->set_living_from_keyboard('y');
            m_shape_name = L"Custom configuration";
            m_name_len = 20;
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        case VK_SPACE : 
            p_living_cells->evolve();   
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
        default :
            break; 
        }
        return 0 ;
    case WM_SIZE:
        Resize();
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
