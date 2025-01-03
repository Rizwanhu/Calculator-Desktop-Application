#include "framework.h"
#include "WindowsProject1.h"
#include <string>
#include <sstream>
#include <cmath>
#include <cctype>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWndResult;                                // handle for the result display (TextBox)
std::wstring currentInput;                      // store current input

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Function to handle the input and process the operation
void ProcessInput(WCHAR input);

// Function to evaluate the expression
double EvaluateExpression(const std::wstring& expression);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// Registers the window class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Saves instance handle and creates main window
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 280, 400, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Create a text field for displaying results with bigger size
    hWndResult = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_AUTOHSCROLL,
        10, 10, 250, 40, hWnd, nullptr, hInstance, nullptr);
    // Create buttons for digits and operations with better alignment and size
    WCHAR buttonLabels[] = L"123+456-789*0=C/=";  // Buttons: digits and operators
    int xPos = 10, yPos = 60;

    for (int i = 0; i < 16; ++i)
    {
        WCHAR label[2] = { buttonLabels[i], L'\0' };  // Create a label for each button
        CreateWindowW(L"BUTTON", label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            xPos, yPos, 60, 60, hWnd, (HMENU)(i + 1), hInstance, nullptr);

        // Adjust the xPos for next button, add some spacing between them
        xPos += 70;

        // Start a new row after every 4th button
        if ((i + 1) % 4 == 0)
        {
            xPos = 10;  // Reset xPos to start the next row
            yPos += 70; // Move to the next row
        }
    }


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


// Processes messages for the main window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // Handle button clicks for calculator
        if (wmId >= 1 && wmId <= 16)
        {
            WCHAR buttonText[2];
            GetWindowTextW((HWND)lParam, buttonText, 2);
            ProcessInput(buttonText[0]);
        }

        // Exit action
        if (wmId == IDM_EXIT)
        {
            DestroyWindow(hWnd);
        }

        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // Add any drawing code here if needed
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Process the input and handle the calculation
void ProcessInput(WCHAR input)
{
    if (input == 'C')  // Clear the display
    {
        currentInput.clear();
        SetWindowTextW(hWndResult, L"");
    }
    else if (input == '=')  // Evaluate the current input expression
    {
        try
        {
            double result = EvaluateExpression(currentInput);
            std::wstringstream ss;
            ss.precision(10); // Control precision for small numbers
            ss << result;
            SetWindowTextW(hWndResult, ss.str().c_str());
        }
        catch (const std::exception&)
        {
            SetWindowTextW(hWndResult, L"Error");
        }
        currentInput.clear();
    }
    else
    {
        currentInput += input;  // Append the input character to the current expression
        SetWindowTextW(hWndResult, currentInput.c_str());
    }
}

// Function to evaluate the mathematical expression
double EvaluateExpression(const std::wstring& expression)
{
    // Simple implementation using basic parsing (a more advanced approach would require using a parser)
    double result = 0.0;
    double currentNumber = 0.0;
    wchar_t lastOperator = '+';

    for (size_t i = 0; i < expression.length(); ++i)
    {
        if (isdigit(expression[i]) || expression[i] == '.')
        {
            currentNumber = currentNumber * 10 + (expression[i] - L'0');
        }
        else
        {
            switch (lastOperator)
            {
            case '+':
                result += currentNumber;
                break;
            case '-':
                result -= currentNumber;
                break;
            case '*':
                result *= currentNumber;
                break;
            case '/':
                if (currentNumber != 0)
                    result /= currentNumber;
                else
                    throw std::invalid_argument("Division by zero");
                break;
            }
            currentNumber = 0.0;
            lastOperator = expression[i];
        }
    }

    // Final operation (after the last number)
    switch (lastOperator)
    {
    case '+':
        result += currentNumber;
        break;
    case '-':
        result -= currentNumber;
        break;
    case '*':
        result *= currentNumber;
        break;
    case '/':
        if (currentNumber != 0)
            result /= currentNumber;
        else
            throw std::invalid_argument("Division by zero");
        break;
    }

    return result;
}
