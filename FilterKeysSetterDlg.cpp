// FilterKeysSetterDlg.cpp : implementation file
//

#include "pch.h"
#include "FilterKeysSetter.h"
#include "FilterKeysSetterDlg.h"
#include <math.h>
#include ".\filterkeyssetterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFilterKeysSetterDlg dialog

CFilterKeysSetterDlg::CFilterKeysSetterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterKeysSetterDlg::IDD, pParent)
	, m_bUpdateIniFile(FALSE)
	, m_bSendChange(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterKeysSetterDlg::ErrorBox(const TCHAR* msg)
{
	MessageBox(msg, _T("FilterKeys Setter"), MB_ICONERROR);
}

void CFilterKeysSetterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_WAIT_EDIT, m_nWait);
	//DDV_MinMaxInt(pDX, m_nWait, 0, 20000);
	DDX_Text(pDX, IDC_DELAY_EDIT, m_nDelay);
	//DDV_MinMaxInt(pDX, m_nDelay, 0, 20000);
	DDX_Text(pDX, IDC_REPEAT_EDIT, m_nRepeat);
	//DDV_MinMaxInt(pDX, m_nRepeat, 0, 20000);
	DDX_Text(pDX, IDC_BOUNCE_EDIT, m_nBounce);
	//DDV_MinMaxInt(pDX, m_nBounce, 0, 20000);

	DDX_Check(pDX, IDC_ON, m_bOn);
	DDX_Check(pDX, IDC_AVAILABLE, m_bAvailable);
	DDX_Check(pDX, IDC_HOTKEYACTIVE, m_bHotKeyActive);
	DDX_Check(pDX, IDC_CONFIRMHOTKEY, m_bConfirmHotKey);
	DDX_Check(pDX, IDC_HOTKEYSOUND, m_bHotKeySound);
	DDX_Check(pDX, IDC_INDICATOR, m_bIndicator);
	DDX_Check(pDX, IDC_CLICK, m_bClick);

	DDX_Control(pDX, IDC_WAIT_EDIT, m_editWait);
	DDX_Control(pDX, IDC_DELAY_EDIT, m_editDelay);
	DDX_Control(pDX, IDC_REPEAT_EDIT, m_editRepeat);
	DDX_Control(pDX, IDC_BOUNCE_EDIT, m_editBounce);

	DDX_Control(pDX, IDC_IGNORE_QUICK, m_radioIgnoreQuick);
	DDX_Control(pDX, IDC_IGNORE_REPEATED, m_radioIgnoreRepeated);
	DDX_Radio(pDX, IDC_IGNORE_QUICK, m_nMode);

	DDX_Check(pDX, IDC_UPDATEINIFILE, m_bUpdateIniFile);
	DDX_Check(pDX, IDC_SENDCHANGE, m_bSendChange);

	DDX_Control(pDX, IDC_FLAGVAL, m_staticFlagVal);
	DDX_Control(pDX, IDC_CHARS_PER_SEC, m_staticCharsPerSec);
}

BEGIN_MESSAGE_MAP(CFilterKeysSetterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_IGNORE_QUICK, OnBnClickedIgnoreQuick)
	ON_BN_CLICKED(IDC_IGNORE_REPEATED, OnBnClickedIgnoreRepeated)
	ON_BN_CLICKED(IDC_ON, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_AVAILABLE, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_INDICATOR, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_CLICK, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_HOTKEYACTIVE, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_CONFIRMHOTKEY, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_HOTKEYSOUND, OnBnClickedFlag)
	ON_BN_CLICKED(IDC_SET_DEFAULTS, OnBnClickedSetDefaults)
	ON_EN_CHANGE(IDC_REPEAT_EDIT, OnEnChangeRepeatEdit)
	ON_BN_CLICKED(IDC_SET_NORMAL, OnBnClickedSetNormal)
	ON_BN_CLICKED(IDC_SET_REGISTRY, OnBnClickedSetRegistry)
	ON_BN_CLICKED(IDC_SET_CURRENT, OnBnClickedSetCurrent)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
	ON_BN_CLICKED(IDC_SET_ORIGINAL, OnBnClickedSetOriginal)
END_MESSAGE_MAP()


// CFilterKeysSetterDlg message handlers

BOOL CFilterKeysSetterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Stash current FilterKeys settings...
	ZeroMemory(&m_fkOriginal, sizeof(m_fkOriginal));
	m_fkOriginal.cbSize = sizeof(FILTERKEYS);
	BOOL ok = SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &m_fkOriginal, 0);

	// Init from current FilterKeys settings...
	OnBnClickedSetCurrent();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFilterKeysSetterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFilterKeysSetterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFilterKeysSetterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFilterKeysSetterDlg::OnOK()
{
	if (!UpdateData(TRUE)) {
		return;
	}
	if (!SaveSettings()) {
		return;
	}
	CDialog::OnOK();
}

void CFilterKeysSetterDlg::OnBnClickedIgnoreQuick()
{
	UpdateControls();
}

void CFilterKeysSetterDlg::OnBnClickedIgnoreRepeated()
{
	UpdateControls();
}

void CFilterKeysSetterDlg::OnBnClickedFlag()
{
	UpdateControls();
}

void CFilterKeysSetterDlg::SetFlags(DWORD dwFlags)
{
	m_bOn = !!(dwFlags & FKF_FILTERKEYSON);
	m_bAvailable = !!(dwFlags & FKF_AVAILABLE);
	m_bIndicator = !!(dwFlags & FKF_INDICATOR);
	m_bClick = !!(dwFlags & FKF_CLICKON);
	m_bHotKeyActive = !!(dwFlags & FKF_HOTKEYACTIVE);
	m_bConfirmHotKey = !!(dwFlags & FKF_CONFIRMHOTKEY);
	m_bHotKeySound = !!(dwFlags & FKF_HOTKEYSOUND);
}

DWORD CFilterKeysSetterDlg::GetFlagsVal()
{
	DWORD dwFlags = 0;
	if (m_bOn) dwFlags |= FKF_FILTERKEYSON;
	if (m_bAvailable) dwFlags |= FKF_AVAILABLE;
	if (m_bIndicator) dwFlags |= FKF_INDICATOR;
	if (m_bClick) dwFlags |= FKF_CLICKON;
	if (m_bHotKeyActive) dwFlags |= FKF_HOTKEYACTIVE;
	if (m_bConfirmHotKey) dwFlags |= FKF_CONFIRMHOTKEY;
	if (m_bHotKeySound) dwFlags |= FKF_HOTKEYSOUND;
	return dwFlags;
}

void CFilterKeysSetterDlg::UpdateFlagVal()
{
	DWORD dwFlags = GetFlagsVal();
	CString s;
	s.Format(_T("(%u)"), dwFlags);
	m_staticFlagVal.SetWindowText(s);
}

void CFilterKeysSetterDlg::OnBnClickedSetDefaults()
{
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	filter_keys.dwFlags = 122;
	filter_keys.iWaitMSec = 1000;
	filter_keys.iDelayMSec = 1000;
	filter_keys.iRepeatMSec = 500;
	filter_keys.iBounceMSec = 0;
	SetValues(filter_keys);
}

void CFilterKeysSetterDlg::OnEnChangeRepeatEdit()
{
	UpdateCharsPerSec();
}

void CFilterKeysSetterDlg::OnBnClickedSetNormal()
{
	int speed = 0;
	SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &speed, 0);
	double chars_per_sec = speed * (30.0 - 2.0) / 31.0 + 2.0;
	int repeat_ms = (int)floor(1000.0 / chars_per_sec + 0.5);

	int delay = 0;
	SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &delay, 0);
	int delay_ms = delay * 250 + 250;

	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	filter_keys.dwFlags = 59;
	filter_keys.iDelayMSec = delay_ms;
	filter_keys.iRepeatMSec = repeat_ms;
	SetValues(filter_keys);
}

static LONG GetDWORDRegKey(HKEY hKey, const WCHAR* strValueName, DWORD& nValue, DWORD nDefaultValue)
{
	nValue = nDefaultValue;
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey, strValueName, 0, NULL, (LPBYTE)&nResult, &dwBufferSize);
	if (ERROR_SUCCESS == nError) {
		nValue = nResult;
	}
	return nError;
}

static LONG GetBoolRegKey(HKEY hKey, const WCHAR* strValueName, bool& bValue, bool bDefaultValue)
{
	DWORD nDefValue((bDefaultValue) ? 1 : 0);
	DWORD nResult(nDefValue);
	LONG nError = GetDWORDRegKey(hKey, strValueName, nResult, nDefValue);
	if (ERROR_SUCCESS == nError) {
		bValue = (nResult != 0) ? true : false;
	}
	return nError;
}

static LONG GetStringRegKey(HKEY hKey, const WCHAR* strValueName, WCHAR* szValue, DWORD cbValue, const WCHAR* strDefaultValue)
{
	SIZE_T sizeInWords = cbValue / sizeof(wchar_t);
	DWORD dwBufferSize = cbValue;
	ULONG nError = ::RegQueryValueExW(hKey, strValueName, 0, NULL, (LPBYTE)szValue, &dwBufferSize);
	if (ERROR_SUCCESS != nError) {
		wcsncpy_s(szValue, sizeInWords, strDefaultValue, sizeInWords);
	}
	return nError;
}

static LONG GetNumericStringRegKey(HKEY hKey, const WCHAR* strValueName, INT& nValue, INT nDefaultValue)
{
	nValue = nDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	LONG nError = GetStringRegKey(hKey, strValueName, szBuffer, sizeof(szBuffer), L"");
	if (ERROR_SUCCESS == nError) {
		nValue = _wtoi(szBuffer);
	}
	return nError;
}

static LONG GetNumericStringRegKey(HKEY hKey, const WCHAR* strValueName, DWORD& nValue, DWORD nDefaultValue)
{
	INT iValue;
	INT iDefaultValue = (INT)nDefaultValue;
	LONG nError = GetNumericStringRegKey(hKey, strValueName, iValue, iDefaultValue);
	nValue = iValue;
	return nError;
}

void CFilterKeysSetterDlg::OnBnClickedSetRegistry()
{
	HKEY hKey;
	LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Accessibility\\Keyboard Response", 0, KEY_READ, &hKey);
	if (lRes == ERROR_SUCCESS) {
		FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
		GetNumericStringRegKey(hKey, L"DelayBeforeAcceptance", filter_keys.iWaitMSec, 1000);
		GetNumericStringRegKey(hKey, L"AutoRepeatDelay", filter_keys.iDelayMSec, 1000);
		GetNumericStringRegKey(hKey, L"AutoRepeatRate", filter_keys.iRepeatMSec, 500);
		GetNumericStringRegKey(hKey, L"BounceTime", filter_keys.iBounceMSec, 0);
		GetNumericStringRegKey(hKey, L"Flags", filter_keys.dwFlags, 122);
		RegCloseKey(hKey);
		SetValues(filter_keys);
	}
	else {
		OnBnClickedSetDefaults();
	}
}

void CFilterKeysSetterDlg::OnBnClickedSetCurrent()
{
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	BOOL ok = SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &filter_keys, 0);
	if (!ok) {
		ErrorBox(_T("Failed to fetch current settings"));
	}
	SetValues(filter_keys);
}

void CFilterKeysSetterDlg::SetValues(const FILTERKEYS& filter_keys)
{
	UpdateData(TRUE);
	m_nWait = filter_keys.iWaitMSec;
	m_nDelay = filter_keys.iDelayMSec;
	m_nRepeat = filter_keys.iRepeatMSec;
	m_nBounce = filter_keys.iBounceMSec;
	m_nMode = m_nBounce ? 1 : 0;
	SetFlags(filter_keys.dwFlags);
	UpdateData(FALSE);
	UpdateControls();
}

void CFilterKeysSetterDlg::UpdateControls()
{
	UpdateData(TRUE);
	UpdateFlagVal();
	UpdateCharsPerSec();
	if (m_nMode == 0) {
		m_editWait.EnableWindow(TRUE);
		m_editDelay.EnableWindow(TRUE);
		m_editRepeat.EnableWindow(TRUE);
		m_editBounce.EnableWindow(FALSE);
	}
	else {
		m_editWait.EnableWindow(FALSE);
		m_editDelay.EnableWindow(FALSE);
		m_editRepeat.EnableWindow(FALSE);
		m_editBounce.EnableWindow(TRUE);
	}
}

void CFilterKeysSetterDlg::UpdateCharsPerSec()
{
	CString s;
	m_editRepeat.GetWindowText(s);
	int val = _wtoi(s);
	if (val) {
		s.Format(_T("(%.1f per second)"), 1000.0 / val);
	}
	else {
		s = "(0 per second)";
	}
	m_staticCharsPerSec.SetWindowText(s);
}

void CFilterKeysSetterDlg::OnBnClickedApply()
{
	if (!UpdateData(TRUE)) {
		return;
	}
	SaveSettings();
}

bool CFilterKeysSetterDlg::SaveSettings()
{
	if (m_nMode == 0) {
		if (m_nWait > 20000) {
			ErrorBox(_T("Ignore under value is too large.\nMaximum value is 20000."));
			m_editWait.SetFocus();
			return false;
		}
		if (m_nDelay > 20000) {
			ErrorBox(_T("Repeat delay value is too large.\nMaximum value is 20000."));
			m_editDelay.SetFocus();
			return false;
		}
		if (m_nRepeat > 20000) {
			ErrorBox(_T("Repeat rate value is too large.\nMaximum value is 20000."));
			m_editRepeat.SetFocus();
			return false;
		}
	}
	else {
		if (m_nBounce > 20000) {
			ErrorBox(_T("Bounce value is too large.\nMaximum value is 20000."));
			m_editBounce.SetFocus();
			return false;
		}
	}
	FILTERKEYS filter_keys = { sizeof(FILTERKEYS) };
	if (m_nMode == 0) {
		filter_keys.iWaitMSec = m_nWait;
		filter_keys.iDelayMSec = m_nDelay;
		filter_keys.iRepeatMSec = m_nRepeat;
	}
	else {
		filter_keys.iBounceMSec = m_nBounce;
	}
	filter_keys.dwFlags = GetFlagsVal();
	UINT fWinIni = 0;
	if (m_bUpdateIniFile) fWinIni |= SPIF_UPDATEINIFILE;
	if (m_bSendChange) fWinIni |= SPIF_SENDCHANGE;
	bool ok = !!SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &filter_keys, fWinIni);
	if (!ok) {
		ErrorBox(_T("Failed to save new settings."));
	}
	return ok;
}

void CFilterKeysSetterDlg::OnBnClickedSetOriginal()
{
	SetValues(m_fkOriginal);
}
