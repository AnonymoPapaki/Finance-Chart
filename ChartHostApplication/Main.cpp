#include <Windows.h>
#include "..\\FChart\\api.h"
#include <sstream>
#include <string>
#include <windowsx.h>
#define TEST(exp) try{exp}catch(std::exception e){MessageBoxA(0,e.what(),0,0);}


LRESULT CALLBACK proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int32_t(*factory)(FCHART_FACTORY_STRUCT*) = 0;

void initchart();
fchart::IChart *chart;
fchart::DataManipulator::IFactory *dataFactory;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShow)
{

	auto lib = LoadLibrary(LR"(..\Debug\FChart.dll)");
	factory = (decltype(factory))GetProcAddress(lib, "fchart_factory");

	WNDCLASSEX wc = { 0 };
	HWND hWnd = NULL;
	MSG msg = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = L"hostApp";
	wc.lpfnWndProc = proc;
	
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"Host App",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);
	RECT rc;
	GetWindowRect(hWnd, &rc);
	FCHART_FACTORY_STRUCT s;
	s.id = FCHART_FACTORY_ID_CHART;
	s.hWnd = hWnd;
	s.x = 0;
	s.y = 0;
	s.height = rc.bottom - rc.top;
	s.width = rc.right - rc.left;
	s.ppOut = reinterpret_cast<void**>(&chart);
	factory(&s);
	
	s.id = FCHART_FACTORY_ID_DATAMANIMUPATOR_FACTORY;
	s.ppOut = reinterpret_cast<void**>(&dataFactory);
	factory(&s);

	ShowWindow(hWnd, nShow);
	UpdateWindow(hWnd);
	
	initchart();
	while (GetMessage(&msg,NULL,0,0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	FreeLibrary(lib);
	return 0;
}


fchart::Quotation q[] = 
{
#include "testquotes.array"
};

void initchart()
{
	//TEST TEST TEST TEST TEST 
#if 1

	//create price series candlestick
	chart
		->SetAreaChartPositionType(fchart::ChartAreaPositionType::Stack)
		->CreateDataBuffer(L"buffer")
		->SetData(L"buffer", q, _countof(q), fchart::SetDataType::Append)
		->GetChartArea(L"default")
		->CreateSeries(L"price")
		->SetBufferSource(L"buffer")
		->SetSeriesType(fchart::SeriesType::Candlestick);

	//create sma series line
	auto sma = dataFactory->CreateSMA(10);
	
	chart
		->CreateDataBuffer(L"sma buffer")
		->GetChartArea(L"default")
		->CreateSeries(L"sma")
		->SetSeriesType(fchart::SeriesType::Line)
		->SetBufferSource(L"sma buffer")
		;
	chart
		->SetDataManipulator(sma, L"buffer", L"sma buffer");

		;
	chart->UpdateBuffer(L"sma buffer");

	chart
		->GetChartArea(L"default")
		->CreateAxis(L"y", fchart::AxisType::Vertical)
		->SetBufferSource(L"buffer")
		;
	chart
		->GetChartArea(L"default")
		->CreateAxis(L"x", fchart::AxisType::Horizontal)
		->SetBufferSource(L"buffer")
		->SetDataType(fchart::AxisDataType::Date)
		
		;


	chart
		->GetChartArea(L"default")
		->FocusLast(L"price");
	

#endif
#if 1
	chart
		->CreateDataBuffer(L"rsi buffer")
		->CreateChartArea(L"chartArea")
		->CreateSeries(L"rsi")
		->SetSeriesType(fchart::SeriesType::Line)
		->SetBufferSource(L"rsi buffer")
		;
	auto rsi = dataFactory->CreateRSI();

	chart
		->SetDataManipulator(rsi, L"buffer", L"rsi buffer")
		->UpdateBuffer(L"rsi buffer")
		->GetChartArea(L"chartArea")
		->CreateAxis(L"y", fchart::AxisType::Vertical)
		->SetBufferSource(L"rsi buffer")
		;
	chart
		->GetChartArea(L"chartArea")
		->CreateAxis(L"x", fchart::AxisType::Horizontal)
		->SetBufferSource(L"rsi buffer")
		->SetDataType(fchart::AxisDataType::Date)
		
		;
	chart
		->GetChartArea(L"chartArea")
		->FocusLast(L"rsi")
		;

#endif
#if 0
	chart
		->CreateChartArea(L"chartArea1")
		->CreateSeries(L"price")
		->AddData(q, _countof(q));

	chart
		->GetChartArea(L"chartArea1")
		->SetXAxisSync(true)
		->GetAxis(L"default x")
		->SetSourceSeries(L"price")
		->SetDataType(fchart::AxisDataType::Date);
#endif
	
	
}

LRESULT CALLBACK proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	POINT p;
	switch (msg)
	{
	case WM_CREATE:
	
		break;
	case WM_SIZE:
		p.x = GET_X_LPARAM(lParam);
		p.y = GET_Y_LPARAM(lParam);
		//ScreenToClient(hWnd, &p);
		chart->SetSize(p.x,p.y);
		break;
		
	case WM_PAINT:
		
		chart->Render();
		
		//ValidateRect(hWnd, NULL);
		break;
	case WM_CLOSE:
		chart->Release();
		PostQuitMessage(0);
		
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}