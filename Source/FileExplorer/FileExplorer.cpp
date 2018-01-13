// FileExplorer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FileExplorer.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <Shobjidl.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#pragma comment (lib,"Shell32.lib")
#pragma comment (lib,"Shlwapi.lib")


#pragma comment(lib, "comctl32.lib")
#include <time.h>
#define MAX_LOADSTRING 100
#define _CRT_SECURE_NO_WARNINGS
#define ID_LISTVIEW 330
#define ID_TREEVIEW 333
#define KB 1
#define MB 2
#define GB 3
#define TB 4
#define RADIX 10

HTREEITEM createPC();

BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
LRESULT onNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void onSize(HWND hWnd, UINT message, int sx, int sy);
void onDestroy(HWND hwnd);
void onCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);

void getSystemDrives(WCHAR** &pszLink, WCHAR** &pszName);

HWND createTreeView(HWND parentWnd, int ID, HINSTANCE hParentInst, int nWidth, int nHeight, int x, int y);
HTREEITEM addItemToTreeView(HTREEITEM item, WCHAR *s, WCHAR *link);
HWND createListView(HWND parentWnd, int ID, HINSTANCE hParentInst, int nWidth, int nHeight, int x, int y);
void createColumnListView();
void addItemToListView(WCHAR* s, WCHAR* link, int level);

WCHAR* getTime();
void loadFile(WCHAR * path);
LPWSTR getType(WIN32_FIND_DATA fd);
LPWSTR convert(__int64 nSize);

void browseFolder(WCHAR* path, HTREEITEM item, BOOL istree);
void catchEventTreeView(int sk, HTREEITEM item);
bool deletePreFolder(HTREEITEM hItemParent);

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
bool checkChildFolder(HTREEITEM parent, WCHAR* path);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FILEEXPLORER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILEEXPLORER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILEEXPLORER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FILEEXPLORER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance HANDLE and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance HANDLE in a global variable and
//        create and display the main program window.
//

int g_width = 300;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance HANDLE in our global variable


   WCHAR buffer[255];
   GetCurrentDirectory(255, buffer);
   WCHAR  config[255];
   wsprintf(config, L"%s\\config.ini", buffer);
   WCHAR tam[10];
   int x, y, w, h;
   GetPrivateProfileString(L"app", L"left", L"100", tam, 10, config);
   x = _wtoi(tam);
   GetPrivateProfileString(L"app", L"top", L"100", tam, 10, config);
   y = _wtoi(tam);
   GetPrivateProfileString(L"app", L"right", L"900", tam, 10, config);
   w = _wtoi(tam);
   GetPrivateProfileString(L"app", L"bottom", L"700", tam, 10, config);
   h = _wtoi(tam);
   GetPrivateProfileString(L"app", L"dodai", L"200", tam, 10, config);
   g_width = _wtoi(tam);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      x, y, w-x, h-y, NULL, NULL, hInstance, NULL);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, 3);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
HINSTANCE g_hInst;
HWND g_hWnd;
HWND g_TreeView;
HWND g_ListView;

int count;
TVINSERTSTRUCT tv;
TVITEM tvi;
LVITEM lvi;

WCHAR** link;
WCHAR** name;
WIN32_FIND_DATA wfd;

RECT rect;
BOOL change;
POINT vtbd;
HWND g_StatusBar;
HIMAGELIST hImageList;
WCHAR* size;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, onCreate);
		HANDLE_MSG(hWnd, WM_NOTIFY, onNotify);
		HANDLE_MSG(hWnd, WM_SIZE, onSize);
		HANDLE_MSG(hWnd, WM_DESTROY, onDestroy);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



// Message HANDLEr for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	
	RECT rect;
	GetWindowRect(hwnd, &rect);

	InitCommonControls();
	g_TreeView = createTreeView(hwnd, ID_TREEVIEW, g_hInst, rect.right / 3, rect.bottom, CW_USEDEFAULT, 0);
	g_ListView = createListView(hwnd, ID_LISTVIEW, g_hInst, (rect.right - rect.left) * 2 / 3 + 1, rect.bottom, rect.right / 3, 0);
	g_StatusBar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,0, 0, 0, 0, hwnd, NULL, hInst, NULL);
	int sizes[3] = { 100, (rect.right / 3) * 2, -1};
	SendMessage(g_StatusBar, SB_SETPARTS, 3, (LPARAM)&sizes);
	SendMessage(g_StatusBar, SB_SETMINHEIGHT, 50, (LPARAM)0);
	SendMessage(g_StatusBar, SB_SETTEXT, 0, (LPARAM)L"STATUS BAR");

	hImageList = ImageList_Create(32, 32, ILC_COLOR16 | ILC_MASK, 0, 0);

	//Create Tree View
	getSystemDrives(link, name);
	
	
	HTREEITEM pc = createPC();

	for (int i = 0; i < count; i++)
	{
		if (pc != NULL)
		{
			HTREEITEM itempTemp = addItemToTreeView(pc,name[i], link[i]);
			checkChildFolder(itempTemp, link[i]);
		}
	}
	

	//Create List View
	createColumnListView();
	for (int i = 0; i < count; i++)
	{
			addItemToListView(name[i], link[i], i);
	}

	return TRUE;
}

LRESULT onNotify(HWND hwnd, int idFrom, NMHDR *pnm) {
	LPNMTREEVIEW nmtree = (LPNMTREEVIEW)pnm;
	NMHDR nmhdr = nmtree->hdr;
	if (idFrom == ID_TREEVIEW) {
		if (nmhdr.code == TVN_ITEMEXPANDING) {
			catchEventTreeView(pnm->code, nmtree->itemNew.hItem);
		}
		else if (pnm->code == TVN_SELCHANGED)
		{
			HTREEITEM parent = TreeView_GetSelection(g_TreeView);
			catchEventTreeView(pnm->code, parent);
		}
		else if (pnm->code == NM_DBLCLK) {
			HTREEITEM parent = TreeView_GetSelection(g_TreeView);
			catchEventTreeView(pnm->code, parent);
		}
	}

	else if (idFrom == ID_LISTVIEW) {
		LPNMLISTVIEW nmlist = (LPNMLISTVIEW)pnm;
		NMHDR nmhdr2 = nmlist->hdr;
		if (nmhdr2.code == NM_CLICK) {
			WCHAR buffer[300];
			//lvi.iItem = ListView_GetNextItem(g_ListView, -1, LVNI_SELECTED);
			if (lvi.iItem == -1)
				return 0;
			LVITEM lvTypeDisk;
			lvTypeDisk.mask = LVIF_TEXT;
			lvTypeDisk.iItem = ListView_GetNextItem(g_ListView, -1, LVNI_FOCUSED);
			lvTypeDisk.iSubItem = 3;
			lvTypeDisk.pszText = buffer;
			lvTypeDisk.cchTextMax = 256;

			ListView_GetItem(g_ListView, &lvTypeDisk);
			WORD a;
			SendMessage(g_StatusBar, SB_SETTEXT, 2, (LPARAM)buffer);
		}

		else if (((LPNMHDR)pnm)->code == NM_DBLCLK) {
			lvi.iItem = ListView_GetNextItem(g_ListView, -1, LVNI_SELECTED);
			if (lvi.iItem == -1)
				return 0;
			ListView_GetItem(g_ListView, &lvi);
			loadFile((WCHAR*)lvi.lParam);
		}
	}
	return 0;
}


void onDestroy(HWND hwnd) {
	WCHAR buffer[255];
	GetCurrentDirectory(255, buffer);
	WCHAR  config[255];
	wsprintf(config, L"%s\\config.ini", buffer);
	WCHAR tam[10];
	GetWindowRect(hwnd, &rect);
	_itow(rect.left, tam, 10);
	WritePrivateProfileString(L"app", L"left", tam, config);
	_itow(rect.top, tam, 10);
	WritePrivateProfileString(L"app", L"top", tam, config);
	_itow(rect.right, tam, 10);
	WritePrivateProfileString(L"app", L"right", tam, config);
	_itow(rect.bottom, tam, 10);
	WritePrivateProfileString(L"app", L"bottom", tam, config);
	_itow(g_width, tam, 10);
	WritePrivateProfileString(L"treeview", L"dodai", tam, config);
	PostQuitMessage(0);
}


//POINT vtc;

void onSize(HWND hWnd, UINT message, int x, int y) {
	GetClientRect(hWnd, &rect);
	MoveWindow(g_TreeView, 0, 0, g_width, rect.bottom - 50, 1);
	MoveWindow(g_ListView, g_width + 5, 0, rect.right - g_width - 10, rect.bottom - 50,1);
	//MoveWindow(g_StatusBar, 0, rect.bottom - 40, x , 40, 1);
	MoveWindow(g_StatusBar, 0, rect.bottom - 50, rect.right, 50, 1);
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	change = false;
	ReleaseCapture();

}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags) {
	if (x >= g_width && x <= g_width + 5) {
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	}

	if (change == true)
	{
		g_width = g_width + x - vtbd.x;
		vtbd.x = x;
		vtbd.y = y;
		if (g_width <= 100) {
			g_width = 101;
			return;
		}
		SendMessage(hwnd, WM_SIZE, 0, 0);
	}
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags) {
	SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	if (x >= g_width && x <= g_width + 5) {
		change = true;
	}
	vtbd.x = x;
	vtbd.y = y;
	SetCapture(hwnd);
}

HWND createTreeView(HWND parentWnd, int ID, HINSTANCE hParentInst, int nWidth, int nHeight, int x, int y) {
	HWND hwndTV;
	hwndTV = CreateWindow(WC_TREEVIEW, L"Tree View", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS,
		x, y, nWidth, nHeight, parentWnd, (HMENU)ID_TREEVIEW, hParentInst, NULL);
	return hwndTV;
}

HWND createListView(HWND parentWnd, int ID, HINSTANCE hParentInst, int nWidth, int nHeight, int x, int y) {
	HWND hwndLV;
	hwndLV = CreateWindow(WC_LISTVIEW, L"List View", WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_ICON,
		x, y, nWidth, nHeight, parentWnd, (HMENU)ID_LISTVIEW, hParentInst, NULL);
	return hwndLV;
}

void getSystemDrives(WCHAR** &pszLink, WCHAR** &pszName) {
	WCHAR buffer[105];
	GetLogicalDriveStrings(105, buffer);

	for (int i = 0; buffer[i] != '\0'; i += 4) {
		count++;
	}

	pszName = new WCHAR*[count];
	pszLink = new WCHAR*[count];

	//Cắt link
	int j = 0, k;
	for (int i = 0; i < count; i++) {
		k = 0;
		pszLink[i] = new WCHAR[4];

		pszName[i] = new WCHAR[2];
		pszName[i][0] = buffer[j];
		pszName[i][1] = '\0';

		for (; j < 4 * (i + 1); j++)
		{
			pszLink[i][k++] = buffer[j];
		}
	}
}


HTREEITEM createPC() {
	tv.hParent = TVI_ROOT;
	tv.item.cchTextMax = 8;
	
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON5));
	int icon = ImageList_AddIcon(hImageList, hIcon);
	SendMessage(g_TreeView, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);
	tv.item.iImage = icon;
	tv.item.iSelectedImage = icon;

	tv.item.lParam = (LPARAM)L"This PC";
	tv.item.pszText = L"This PC";
	tv.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_CHILDREN;
	tv.item.cChildren = 1;
	return (HTREEITEM)SendMessage(g_TreeView, TVM_INSERTITEM, 0, (LPARAM)&tv);
}

HTREEITEM addItemToTreeView(HTREEITEM item, WCHAR *s, WCHAR *link) {
	tv.hInsertAfter = item;
	tv.item.cchTextMax = 50;

	if (link != L"Hihi:\\") {
		WORD a;
		HICON hicon = ExtractAssociatedIcon(hInst, link, &a);
		int icon = ImageList_AddIcon(hImageList, hicon);
		SendMessage(g_TreeView, TVM_SETIMAGELIST, 0, (LPARAM)hImageList);
		tv.item.iImage = icon;
		tv.item.iSelectedImage = icon;
	}

	tv.item.lParam = (LPARAM)link;
	tv.item.pszText = s;
	tv.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	
	tv.hParent = item;

	return (HTREEITEM)SendMessage(g_TreeView, TVM_INSERTITEM, 0, (LPARAM)&tv);
}

void createColumnListView() {
	LVCOLUMN column;
	column.cx = 300;
	column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	column.fmt = LVCFMT_LEFT;
	column.pszText = L"Name";
	column.iSubItem = 0;
	ListView_InsertColumn(g_ListView, 0, &column);

	column.cx = 200;
	column.pszText = L"Date Modified";
	ListView_InsertColumn(g_ListView, 1, &column);

	column.cx = 150;
	column.pszText = L"Type";
	ListView_InsertColumn(g_ListView, 2, &column);

	column.cx = 100;
	column.pszText = L"Size";
	ListView_InsertColumn(g_ListView, 3, &column);
}

void addItemToListView(WCHAR* s, WCHAR* link, int level) {
	lvi.cchTextMax = 50;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	
	WORD a;
	HICON hicon = ExtractAssociatedIcon(hInst,link, &a);
	int icon = ImageList_AddIcon(hImageList, hicon);
	ListView_SetImageList(g_ListView, hImageList, 1);
	lvi.iImage = icon;
	
	lvi.iItem = level;
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM)link;
	lvi.pszText = s;
	ListView_InsertItem(g_ListView, &lvi);
	ListView_SetItemText(g_ListView, level, 1, getTime());
	ListView_SetItemText(g_ListView, level, 2, getType(wfd));
	ListView_SetItemText(g_ListView, level, 3, convert(wfd.nFileSizeLow));
}

void loadFile(WCHAR * path)
{
	GetFileAttributesEx(path, GetFileExInfoStandard, &wfd);
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		ListView_DeleteAllItems(g_ListView);
		browseFolder(path, NULL, 0);
	}
	else {
		ShellExecute(NULL, L"open", path, NULL, NULL, SW_SHOWNORMAL);
	}
}

WCHAR* getTime() {
	ULARGE_INTEGER ull;
	FILETIME a = wfd.ftLastWriteTime;
	ull.LowPart = a.dwLowDateTime;
	ull.HighPart = a.dwHighDateTime;
	time_t timeNow = ull.QuadPart / 10000000ULL - 11644473600ULL;
	tm *ltm = localtime(&timeNow);
	if (ltm == NULL)
		return '\0';
	else {
		WCHAR* t = new WCHAR[30];
		wsprintf(t, L"%d-%d-%d %d:%d", ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min);
		return t;
	}
}


LPWSTR getType(WIN32_FIND_DATA fd)
{
	if (fd.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY)
		return  L"File folder";
	int nDotPos = wcsrchr(fd.cFileName, L'.') - fd.cFileName;
	int len = wcslen(fd.cFileName);
	if (nDotPos < 0 || nDotPos >= len) //Nếu không tìm thấy
		return L"Không biết";
	TCHAR *szExtension = new TCHAR[len - nDotPos + 1];
	int i;
	for (i = nDotPos; i < len; ++i)
		szExtension[i - nDotPos] = fd.cFileName[i];
	szExtension[i - nDotPos] = NULL; //Kí tự kết thúc chuỗi
	if (!wcsicmp(szExtension, L".htm") || !wcsicmp(szExtension, L".html"))
		return L"Web page";
	TCHAR pszOut[256];
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 256;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return L"Không biết";
	}
	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszOut, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return L"Không biết";
	}
	RegCloseKey(hKey);
	TCHAR *pszPath = new TCHAR[1000];
	dwSize = 1000;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, pszOut, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return L"Không biết";
	}

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszPath, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return L"Không biết";
	}
	RegCloseKey(hKey);

	return pszPath;
}



LPWSTR convert(__int64 nSize)
{
	int nType = 0;
	while (nSize >= 1048576) 
	{
		nSize /= 1024;
		++nType;
	}
	__int64 nRight;
	if (nSize >= 1024)
	{
		nRight = nSize % 1024;
		while (nRight > 99)
			nRight /= 10;
		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;
	TCHAR *buffer = new TCHAR[11];
	_itow_s(nSize, buffer, 11, RADIX);
	if (nRight != 0 && nType > KB)
	{
		wcscat(buffer, L".");
		TCHAR *right = new TCHAR[3];
		_itow_s(nRight, right, 3, RADIX);
		wcscat(buffer, right);
	}
	if (buffer[0] == L'0')
		return L"";
	switch (nType)
	{
	case 0://Bytes
		wcscat(buffer, L" bytes");
		break;
	case KB:
		wcscat(buffer, L" KB");
		break;
	case MB:
		wcscat(buffer, L" MB");
		break;
	case GB:
		wcscat(buffer, L" GB");
		break;
	case TB:
		wcscat(buffer, L" TB");
		break;
	}
	return buffer;
}

void catchEventTreeView(int sk, HTREEITEM item)
{
	WCHAR* buffer = new WCHAR[256];
	tvi.cchTextMax = 256;
	tvi.hItem = item;
	tvi.pszText = buffer;
	tvi.mask = TVIF_TEXT;

	TreeView_GetItem(g_TreeView, &tvi);

	if (wcscmp((LPWSTR)tvi.pszText, L"This PC") != 0 && tvi.hItem != NULL)
	{
		if (sk == NM_DBLCLK)
		{
			ListView_DeleteAllItems(g_ListView);
			browseFolder((LPWSTR)tvi.lParam, item, 1);
			browseFolder((LPWSTR)tvi.lParam, NULL, 0);
		}
		else if (sk == TVN_ITEMEXPANDING)
		{
			browseFolder((LPWSTR)tvi.lParam, item, 1);
		}
		else if (sk == TVN_SELCHANGED)
		{
			ListView_DeleteAllItems(g_ListView);
			browseFolder((LPWSTR)tvi.lParam, NULL, 0);
		}
	}
	
}

bool deletePreFolder(HTREEITEM hItemParent)
{
	HTREEITEM hCurSelChild = TreeView_GetChild(g_TreeView, hItemParent);
	if (hCurSelChild == NULL)
		return true;
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hCurSelChild;
	TreeView_GetItem(g_TreeView, &tv);
	if (!wcscmp((LPWSTR)tv.lParam, L"Hihi:\\"))
		TreeView_DeleteItem(g_TreeView, hCurSelChild);
	else
		return false;
	return true;
}

void browseFolder(WCHAR* path, HTREEITEM item, BOOL istree)
{
	LPSHELLFOLDER folder = NULL;
	LPENUMIDLIST itemfirst = NULL;
	LPITEMIDLIST itemdata = NULL;

	LPITEMIDLIST itemstart = ILCreateFromPath(path);
	SHBindToObject(NULL, itemstart, NULL, IID_IShellFolder, (void**)&folder);

	if (istree)
	{
		folder->EnumObjects(NULL, SHCONTF_FOLDERS, &itemfirst);
		if (!deletePreFolder(item))
			return;
	}
	else {
		ListView_DeleteAllItems(g_ListView);
		folder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &itemfirst);
	}

	int level = 0;
	while (1)
	{
		HRESULT trave = itemfirst->Next(1, &itemdata, NULL);
		if (trave == S_FALSE)
			break;
		if (trave == S_OK)
		{
			SHGetDataFromIDList(folder, itemdata, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA));
			WCHAR *link = new WCHAR[1000];
			wsprintf(link, L"%s%s\\",path,wfd.cFileName);
			if (istree) {
				HTREEITEM itemTemp = addItemToTreeView(item, wfd.cFileName, link);
				checkChildFolder(itemTemp, link);
			}
			else {
				addItemToListView(wfd.cFileName, link, level);
				level++;
			}
		}
	}
}


bool checkChildFolder(HTREEITEM parent, WCHAR* path) {
	LPSHELLFOLDER folder = NULL;
	LPENUMIDLIST itemfirst = NULL;
	LPITEMIDLIST itemdata = NULL;
	LPITEMIDLIST itemstart = ILCreateFromPath(path);
	SHBindToObject(NULL, itemstart, NULL, IID_IShellFolder, (void**)&folder);
	if (folder == NULL)
		return false;
	folder->EnumObjects(NULL, SHCONTF_FOLDERS, &itemfirst);
	if (itemfirst == NULL)
		return false;
	while (itemfirst->Next(1, &itemdata, NULL) == S_OK)
	{
		SHGetDataFromIDList(folder, itemdata, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA));
		addItemToTreeView(parent, L"Hihi", L"Hihi:\\");
		return true;
	}
}