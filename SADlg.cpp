// SADlg.cpp : implementation file
//

#include "stdafx.h"
#include "SA.h"
#include "SADlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSADlg dialog

CSADlg::CSADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSADlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSADlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSADlg)
	DDX_Control(pDX, IDC_CITY_FRAME, m_CityFrame);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSADlg, CDialog)
	//{{AFX_MSG_MAP(CSADlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSADlg message handlers

BOOL CSADlg::OnInitDialog()
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


	srand((unsigned)time(NULL));

	CRect r;
	m_CityFrame.GetClientRect(&r);

	m_memDC.CreateCompatibleDC(GetDC());
	m_bmp.CreateCompatibleBitmap(GetDC(), r.Width(), r.Height());
	m_font.CreatePointFont(110, L"Arial");
	m_pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_brush.CreateSolidBrush(RGB(255, 255, 255));

	m_memDC.SelectObject(&m_bmp);
	m_memDC.SelectObject(&m_font);
	m_memDC.SelectObject(&m_pen);
	m_memDC.SelectObject(&m_brush);

	m_CityFrame.GetClientRect(&r);
	m_memDC.Rectangle(&r);

	m_memDC.SetBkMode(TRANSPARENT);

	COLORREF oldTxtCol = (m_memDC.SetTextColor(RGB(0, 100, 10)));

	alpha = 1.0;
	beta = 0.5;
	rho = 0.5;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSADlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSADlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
		CPaintDC dc(this); // device context for painting

		CRect r;
		m_CityFrame.GetWindowRect(&r);
		ScreenToClient(r);

		dc.BitBlt(r.left, r.top, r.right - r.left, r.bottom - r.top, &m_memDC, 0, 0, SRCCOPY);

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSADlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

/************************************}
{READING THE MAP FROM A FILE         }
{************************************/
BOOL CSADlg::InitializeMap()
{

	int i, j, k;

	int stop;

	CFile f;
	if (!f.Open(L"tsp-c.dat", CFile::modeRead | CFile::shareDenyWrite))
	{
		MessageBox(L"Please put tsp-c.dat in the folder containing the program.");
		return FALSE;
	}

	float fBuf;

	for (i = 0; i < NCITIES; i++)
	{
		f.Read(&fBuf, sizeof(float));
		loc[i][0] = (double)fBuf;
		f.Read(&fBuf, sizeof(float));
		loc[i][1] = (double)fBuf;
	};

	f.Close();
	for (j = 0; j < NCITIES; j++)
	{
		do
		{
			stop = 1;
			route[j] = rand() % NCITIES;
			k = 0;

			while (k < j)
			{
				if (route[k] == route[j]) stop = 0;
				k++;
			};
		} while (!stop);
	};

	distance = 0;

	for (i = 0; i < NCITIES; i++)
		for (j = 0; j < NCITIES; j++)
			intens[i][j] = 0.01;

	for (i = 0; i < NCITIES; i++)
		for (j = 1; j < NCITIES; j++)
			ants[i][j] = -1;

	for (i = 0; i < NCITIES; i++)
		ants[i][0] = 0;

	for (i = 0; i < NCITIES; i++)
		for (int j = 0; j < NCITIES; j++)
		{
			delta[i][j] = 0.0;

			double d = Dst(i, j);

			if (d == 0.0)
				d = -1;

			visibility[i][j] = 1 / d;
		}

	return TRUE;
}

double CSADlg::Dst(int i, int j)
{
	double xi = loc[i][0];
	double yi = loc[i][1];
	double xj = loc[j][0];
	double yj = loc[j][1];

	double res = sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));

	return res;
}

bool CSADlg::IsCityInAnt(int city, int ant)
{
	bool bRes = false;

	for (int i = 0; i < NCITIES; i++)
	{
		if (ants[ant][i] == city)
		{
			bRes = true;
			break;
		}
	}

	return bRes;
}


/**************************************************************************}
{DRAWING THE CURRENT PATH, DISPLAYING ITS LENGTH                           }
{**************************************************************************/
void CSADlg::Draw()
{

	int i, x, y, x1, y1;
	char s[10];
	CString sEn = "Route length: ";

	CRect r;
	m_CityFrame.GetClientRect(&r);
	m_memDC.Rectangle(&r);

	if (distance == 0.0)
	{
		sEn = "Calculate the length of the current route!";
	}
	else
	{
		_gcvt_s(s, sizeof(s), distance, 5);
		sEn += CString(s);
	}

	m_memDC.TextOut(10, r.Height() - 20, sEn);

	for (i = 0; i < NCITIES; i++)
	{
		x = 10 + (int)((r.Width() - 20) * loc[i][0]);
		y = 10 + (int)((r.Height() - 20) * loc[i][1]);
		m_memDC.Ellipse(x - 5, y - 5, x + 5, y + 5);
	}

	for (i = 0; i < NCITIES - 1; i++)
	{
		x = 10 + (int)((r.Width() - 20) * loc[route[i]][0]);
		y = 10 + (int)((r.Height() - 20) * loc[route[i]][1]);
		x1 = 10 + (int)((r.Width() - 20) * loc[route[i + 1]][0]);
		y1 = 10 + (int)((r.Height() - 20) * loc[route[i + 1]][1]);
		m_memDC.MoveTo(x, y);
		m_memDC.LineTo(x1, y1);
	};

	x = 10 + (int)((r.Width() - 20) * loc[route[NCITIES - 1]][0]);
	y = 10 + (int)((r.Height() - 20) * loc[route[NCITIES - 1]][1]);
	x1 = 10 + (int)((r.Width() - 20) * loc[route[0]][0]);
	y1 = 10 + (int)((r.Height() - 20) * loc[route[0]][1]);
	m_memDC.MoveTo(x, y);
	m_memDC.LineTo(x1, y1);

	CDC* pDC = GetDC();
	m_CityFrame.GetWindowRect(&r);
	ScreenToClient(r);

	pDC->BitBlt(r.left, r.top, r.right - r.left, r.bottom - r.top, &m_memDC, 0, 0, SRCCOPY);

	ReleaseDC(pDC);

}

void CSADlg::OnStart()
{

	if (!InitializeMap())
		return;

	Draw();

	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!}
	{INSERT THE ANT COLONY ALGORITHM HERE                                      }
	{!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!}

	The map has been read from a file. NCITIES indicates the number of cities.
	The coordinates of the cities are stored in the table loc[NCITIES][2].
	Each row of this table contains the coordinates (x,y) of one city.

	The number of ants is given by NANTS. Each ant is represented by one row in table ants[NANTS][NCITIES].
	Each row is supposed to contain indices of cities visited by the given ant.
	Each ant starts from city no. 0, therefore the first value in each row is 0.
	All the other values are initially set to -1.

	The table intens[NCITIES][NCITIES] stores the "pheromone" intensity.
	intens[i][j] denotes the "scent" of the connection between cities i and j.
	Initially all the connections share the same weight.

	The table visibility[NCITIES][NCITIES] stores the values describing how well one can "see" city j from city i.
	visibility[NCITIES][NCITIES] stores value 1/d(i,j), where d(i,j) denotes the distance between cities i and j.

	THe table delta[NCITIES][NCITIES], initially filled with zeros, will be used to store the movements of ants between cities.

	THe values of alpha, beta and rho used by the algorithm are already set.

	'Draw' procedure displays the route stored in 'route' variable and the value of 'distance' (when calculated).

	Dst(i,j) calculates the distance between cities i and j.

	IsCityInAnt(city, ant) returns 'true', when 'city' has been already visited by the 'ant'.

	The variables are defined in SADlg.h file.


	---ANT COLONY ALGORITHM---

	-------------------------------------------------------------------------------------
	IMPORTANT! The attractiveness of connection between i-th and j-th city is defined as
	(intens[i][j]^alpha * visibility[i][j]^beta)
	-------------------------------------------------------------------------------------

	Repeat the following steps 100 times.

	1. Repeat the following actions (NCITIES-1) times.
		For each ant select the next city:
		- calculate the sum of S of atractivenesses of all POSSIBLE connections
		- choose the next city with the probability equal to
			0 if the ant has already visited the city
			(connection_attractiveness / S) otherwise

	2. For each ant calculate the length of its route dist(k).
	3. Copy the shortest route to 'route' and its length to 'distance', then call 'Draw'.
	4. Fill table 'delta':
	   for each (k-th) ant, if it contains the connection between cities 'i' and 'j' set
		 delta[i][j] += a/dist(k);
	   where a = 10 for the best ant, a = 1 otherwise,
	5. Update 'intens' table (for all i,j):
		intens[i][j] = rho*intens[i][j] + delta[i][j];
	6. Fill 'delta' with zeros.
	7. Reset 'ants' table (set all the elements except the first column to -1).

   {!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

   // Repeat the following steps 100 times.
	for (int step = 0; step < 100; step++) {
		// 1. Repeat the following actions (NCITIES-1) times.
		for (int ant = 0; ant < NANTS; ant++) {
			for (int q = 1; q < NCITIES; q++) {
				// For each ant select the next city:
				double S = 0;
				int current_city = ants[ant][q - 1];
				// - calculate the sum of S of atractivenesses of all POSSIBLE connections
				for (int j = 0; j < NCITIES; j++) {
					if (!IsCityInAnt(j, ant)) {
						S += pow(intens[current_city][j], alpha) * pow(visibility[current_city][j], beta);
					}
				}
				// - choose the next city with the probability equal to
				// 0 if the ant has already visited the city
				// (connection_attractiveness / S) otherwise
				double city_prob = 0;
				bool inserted = false;
				while (!inserted) {
					int cand_city = rand() % NCITIES;
					double prob = (double)rand() / RAND_MAX;
					if (IsCityInAnt(cand_city, ant)) {
						city_prob = 0;
					}
					else {
						city_prob = pow(intens[current_city][cand_city], alpha) * pow(visibility[current_city][cand_city], beta) / S;
					}
					if (city_prob > prob) {
						ants[ant][q] = cand_city;
						inserted = true;
					}
				}
			}
		}
		// 2. For each ant calculate the length of its route dist(k).
		double dist[NANTS];
		int best_ant = 0;
		double best_length = 10000000;
		for (int ant = 0; ant < NANTS; ant++) {
			// link first and last on the route
			dist[ant] = Dst(ants[ant][NCITIES - 1], ants[ant][0]);
			for (int city = 1; city < NCITIES; city++) {
				int city_a = ants[ant][city - 1];
				int city_b = ants[ant][city];
				dist[ant] += Dst(city_a, city_b);
			}
			if (dist[ant] < best_length) {
				best_ant = ant;
				best_length = dist[ant];
			}
		}
		// 3. Copy the shortest route to 'route' and its length to 'distance', then call 'Draw'.
		distance = best_length;
		for (int city = 0; city < NCITIES; city++) {
			route[city] = ants[best_ant][city];
		}
		Draw();
		// 4. Fill table 'delta':
		// for each (k - th) ant
		for (int ant = 0; ant < NANTS; ant++) {
			// where a = 10 for the best ant, a = 1 otherwise,
			int a = 1;
			if (ant == best_ant) {
				a = 10;
			}
			for (int i = 0; i < NCITIES; i++) {
				for (int j = 0; j < NCITIES; j++) {
					// if it contains the connection between cities 'i' and 'j'
					bool contains_connection = false;
					// check from the last to the start
					if (ants[ant][NCITIES - 1] == i && ants[ant][0] == j) {
						contains_connection = true;
					}
					if (!contains_connection) {
						for (int ii = 0; ii < NCITIES - 1; ii++) {
							if (ants[ant][ii] == i && ants[ant][ii + 1] == j) {
								contains_connection = true;
								break;
							}
						}
					}
					if (contains_connection) {
						delta[i][j] += a / dist[ant];
					}
				}
			}
		}
		// 5. Update 'intens' table (for all i,j):
		for (int i = 0; i < NCITIES; i++) {
			for (int j = 0; j < NCITIES; j++) {
				intens[i][j] = rho * intens[i][j] + delta[i][j];
			}
		}
		// 6. Fill 'delta' with zeros.
		for (int i = 0; i < NCITIES; i++) {
			for (int j = 0; j < NCITIES; j++) {
				delta[i][j] = 0;
			}
		}
		// 7. Reset 'ants' table (set all the elements except the first column to -1).
		for (int ant = 0; ant < NANTS; ant++) {
			for (int city = 1; city < NCITIES; city++) {
				ants[ant][city] = -1;
			}
		}
	}
}
