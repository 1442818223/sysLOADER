

// 驱动加载器Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "驱动加载器.h"
#include "驱动加载器Dlg.h"
#include "afxdialogex.h"
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// C驱动加载器Dlg 对话框



C驱动加载器Dlg::C驱动加载器Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C驱动加载器Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C驱动加载器Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &C驱动加载器Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &C驱动加载器Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// C驱动加载器Dlg 消息处理程序

BOOL C驱动加载器Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C驱动加载器Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void C驱动加载器Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C驱动加载器Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



std::string GenerateRandomServiceName(size_t length) {
	const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string serviceName;
	for (size_t i = 0; i < length; ++i) {
		serviceName += charset[rand() % (sizeof(charset) - 1)];
	}
	return serviceName;
}

std::string randomServiceName;
void C驱动加载器Dlg::OnBnClickedButton1()
{
	srand(static_cast<unsigned int>(time(nullptr))); // 用当前时间初始化随机种子
	 randomServiceName = GenerateRandomServiceName(6); // 生成8个字符的随机服务名


	
	// 选择文件对话框
	static TCHAR szBuffer[MAX_PATH] = { 0 }; // 选中文件的路径
	static OPENFILENAME ofn = { 0 }; // 结构体初始化

	ofn.lStructSize = sizeof(ofn); // 结构体大小
	ofn.hwndOwner = this->m_hWnd; // 窗口句柄
	ofn.lpstrFilter = _T("驱动文件 (*.sys)\0*.sys\0所有文件 (*.*)\0*.*\0"); // 要选择的文件后缀
	ofn.lpstrInitialDir = _T("C:\\Program Files"); // 默认的文件路径
	ofn.lpstrFile = szBuffer; // 存放文件的缓冲区
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer); // 缓冲区大小
	ofn.nFilterIndex = 1; // 默认选择的过滤器
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER; // 标志

	// 调用选择文件对话框
	if (GetOpenFileName(&ofn)) {

		// 选择成功，szBuffer 中保存了文件路径
		// 处理选中的文件路径

		// 获取文件名（不包括路径）
		TCHAR fileName[MAX_PATH];
		PathStripPath(szBuffer); // 获取文件名而不包括路径
		StringCchCopy(fileName, MAX_PATH, szBuffer); // 安全地拷贝文件名
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
				// 获取临时文件夹路径
		TCHAR tempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, tempPath);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 构造完整文件路径
		TCHAR fullPath[MAX_PATH];
		PathCombineW(fullPath, tempPath, L"MMMMMu.sys");
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			  // 创建新的文件夹路径
		TCHAR newFolder[MAX_PATH];
		PathCombineW(newFolder, tempPath, std::wstring(randomServiceName.begin(), randomServiceName.end()).c_str());
		// 尝试创建文件夹，如果已存在，则修改文件夹名
		int counter = 1;
		while (CreateDirectoryW(newFolder, NULL) == 0 && GetLastError() == ERROR_ALREADY_EXISTS) {
			// 如果文件夹已存在，修改名称
			TCHAR tempFolder[MAX_PATH];
			swprintf(tempFolder, MAX_PATH, L"%s_%d", newFolder, counter++);
			wcscpy_s(newFolder, MAX_PATH, tempFolder); // 更新文件夹路径
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// 复制MMMMMu.sys到新文件夹
		TCHAR destinationPath[MAX_PATH];
		PathCombineW(destinationPath, newFolder, fileName); // 目标文件路径
		if (!CopyFileW(fullPath, destinationPath, FALSE)) {
			// 复失败
			exit;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//将MMMMMu.sys改名为目标文件
		TCHAR newFileName[MAX_PATH];
		PathCombineW(newFileName, newFolder, fileName); // 新的文件名路径
		if (!MoveFileEx(destinationPath, newFileName, MOVEFILE_REPLACE_EXISTING)) {
			exit;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TCHAR 要启动的驱动文件路径[MAX_PATH];
		PathCombineW(要启动的驱动文件路径, newFolder, fileName); // <要启动的驱动文件名>
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


				// 构造目标文件的完整路径
		TCHAR targetFile[MAX_PATH];
		PathCombineW(targetFile, newFolder, L"Other.sys");
		// 复制文件并重命名为 Other.sys
		if (!CopyFileW(ofn.lpstrFile, targetFile, FALSE)) {
			exit;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						//MessageBoxW(L"Rename successful", L"Success", MB_OK);

						// 先定义一个用于存储单字节字符路径的数组
		char 路径[MAX_PATH];

		// 进行宽字符到单字节字符的转换
		WideCharToMultiByte(CP_UTF8, 0, 要启动的驱动文件路径, -1, 路径, sizeof(路径), NULL, NULL);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		 
		// // 定义用于存储单字节字符的数组 //不知道为啥无法将目标驱动名复制给服务名 只有随机才行
		//char fileNameChar[MAX_PATH];
		//WideCharToMultiByte(CP_UTF8, 0, fileName, -1, fileNameChar, sizeof(fileNameChar), NULL, NULL);
		
		// 创建服务命令
		char createCmd[512];
		snprintf(createCmd, sizeof(createCmd), "sc create %s binPath= \"%s\" type= kernel start= demand",
			randomServiceName.c_str(), 路径);
		int resultCreate = system(createCmd);
		if (resultCreate != 0) {
			printf("创建服务失败\n");
			DWORD error = GetLastError();
			TCHAR errorMsg[256];
			wsprintf(errorMsg, L"创建服务失败 error code: %d", error);
			MessageBoxW(errorMsg, L"Error", MB_OK);
		}

		// 启动服务命令
		char startCmd[512];
		snprintf(startCmd, sizeof(startCmd), "sc start %s", randomServiceName.c_str());
		int resultStart = system(startCmd);
		if (resultStart != 0) {
			printf("启动服务失败\n");
			DWORD error = GetLastError();
			TCHAR errorMsg[256];
			wsprintf(errorMsg, L"启动服务失败 error code: %d", error);
			MessageBoxW(errorMsg, L"Error", MB_OK);
		}


	}
	else {
		// 选择失败，处理错误
	}

}


void C驱动加载器Dlg::OnBnClickedButton2()
{
	// 确保 randomServiceName 已经在 OnBnClickedButton1 中被设置
	if (randomServiceName.empty()) {
		MessageBoxW(L"服务名无效，请先创建服务。", L"错误", MB_OK);
		return;
	}

	// 停止服务命令
	char stopCmd[512];
	snprintf(stopCmd, sizeof(stopCmd), "sc stop %s", randomServiceName.c_str());

	int resultStop = system(stopCmd);
	if (resultStop != 0) {
		printf("停止服务失败\n");
		DWORD error = GetLastError();
		TCHAR errorMsg[256];
		wsprintf(errorMsg, L"停止服务失败 error code: %d", error);
		MessageBoxW(errorMsg, L"Error", MB_OK);
		return; // 停止失败时不继续删除
	}

	// 卸载服务命令
	char deleteCmd[512];
	snprintf(deleteCmd, sizeof(deleteCmd), "sc delete %s", randomServiceName.c_str());

	int resultDelete = system(deleteCmd);
	if (resultDelete != 0) {
		printf("卸载服务失败\n");
		DWORD error = GetLastError();
		TCHAR errorMsg[256];
		wsprintf(errorMsg, L"卸载服务失败 error code: %d", error);
		MessageBoxW(errorMsg, L"Error", MB_OK);
	}
	else {
		MessageBoxW(L"服务卸载成功", L"成功", MB_OK);
	}
}

