//OpenGL 3.0 - Loads Proteins from Files on the Protein Data Bank

//Author: Stephen Laycock and Volodymyr Nazarenko
//Copyright (C) 2011 - Stephen Laycock, University of East Anglia
//Source Code For Educational Purposes

using namespace std;
#include <windows.h>		// Header File For Windows
#include "console.h"
#include <iostream>

#include "GLSL/Shader.h"	//Include shader header file, this is not part of OpenGL
							//Shader.h has been created using common glsl structure

#include "PDBLoading\FileHandle.h"
#include "ProteinRendering\Atoms.h"

#include "CartoonModel.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"


AtomList theList;
Atom** myListPtr;
int myListSize = 0;
bool drawHydrogens = false;


Shader moleculeShader;  ///shader object 

float mTransparency = 0.2f;	//Molecule transparency
float cTransparency = 1.0f;	//Cartoon model transparency
bool isCartoon = true;
bool isCartoonSpin = false;
bool isMolecule = false;
bool isTransparent = false;

//ConsoleWindow console;

int screenWidth = 640, screenHeight = 480;
bool keys[256];
double spin = 0;
double mouse_x, mouse_y;
bool LeftPressed = false;
float ZOOM = 0.0f;
float SPIN = 0.0f;
glm::vec2 xyMove = glm::vec2(0.0f, 0.0f);

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape();				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void update();				//called in winmain to update variables
void updateCartoonSpin();

void drawCube();			//draws a cube of unit size to the screen, with centre at the origin.

void drawMolecule();
void renderProtein();

__int64 startTime, now;
__int64 ticksPerSecond;
bool first = false;
int counter = 0;

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing
GLfloat position[] = {0.0f, 0.0f, 0.0f, 1.0f};

CartoonModel CM;

/*************    START OF OPENGL FUNCTIONS   ****************/
void display() {
	if (first) {
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isCartoon) {
		if (isCartoonSpin) {
			updateCartoonSpin();
		}
		//CM.initCartoonModel(theList);
		CM.drawCartoonModel(ProjectionMatrix, xyMove, ZOOM, SPIN, isTransparent);
	}
	if (isMolecule) {
		drawMolecule();
	}


	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	if(((float(now - startTime) / float(ticksPerSecond))) > 1)
	{
		cout << "Frames per second: " << counter << endl;
		counter = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	}
	counter++;

	glFlush();
}


void drawMolecule() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(moleculeShader.handle());  // use the shaderc

	//All of our geometry will have the same projection matrix.
	//we only need to set it once, since we are using the same shader.
	GLuint matLocation = glGetUniformLocation(moleculeShader.handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	ModelViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-xyMove, -ZOOM));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, SPIN, glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(moleculeShader.handle(), "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ModelViewMatrix));

	//normal matrix isn't effected by translation
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(moleculeShader.handle(), "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glUniform1f(glGetUniformLocation(moleculeShader.handle(), "constantAttenuation"), 0.5);
	glUniform1f(glGetUniformLocation(moleculeShader.handle(), "linearAttenuation"), 0.05);
	glUniform1f(glGetUniformLocation(moleculeShader.handle(), "quadraticAttenuation"), 0.005);

	float Light_Ambient[4] = { 0.8, 0.8, 0.8, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "light_ambient"), 1, Light_Ambient);
	float Light_Diffuse[4] = { 0.9, 0.9, 0.9, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "light_diffuse"), 1, Light_Diffuse);
	float Light_Specular[4] = { 0.9, 0.9, 0.9, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "light_specular"), 1, Light_Specular);

	float Material_Ambient[4] = { 0.3, 0.3, 0.3, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "material_ambient"), 1, Material_Ambient);
	float Material_Diffuse[4] = { 0.6, 0.6, 0.6, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "material_diffuse"), 1, Material_Diffuse);
	float Material_Specular[4] = { 0.7, 0.7, 0.7, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "material_specular"), 1, Material_Specular);
	float Material_Shininess = 100.0;
	glUniform1f(glGetUniformLocation(moleculeShader.handle(), "material_shininess"), Material_Shininess);

	float LightPos[4] = { -15.0, 0.0, 2500.0, 1.0 };
	glUniform4fv(glGetUniformLocation(moleculeShader.handle(), "LightPos"), 1, LightPos);

	glUniform1f(glGetUniformLocation(moleculeShader.handle(), "transparency"), mTransparency);


	//ModelViewMatrix = glm::rotate(viewingMatrix,(float)spin, glm::vec3(0,1,0));

	//glUniformMatrix4fv(glGetUniformLocation(myShader.handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//glTranslatef(0,0,-ZOOM);

	//set light positions for scene
	//glPushMatrix();
	//glTranslatef(0,0,ZOOM);
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	//glPopMatrix();

	//spin the molecule
	//glRotated(spin,0,1,0);

	//render the protein
	renderProtein();

	//glFlush();	
}

void renderProtein() {
	SuperAtom::bindbuffers(moleculeShader.handle());
	for (int i = 0; i < myListSize; i++)
	{
		Atom* temp = myListPtr[i];
		//Vector3d v = temp->getMyLocalPosition();
		//float radius = temp->getRadius();

		if ((!temp->isWater()) && (temp->getEle().compare("H") != 0)) //not water or hydrogen
		{
			//temp->renderSimple();
			temp->renderUsingVBO(moleculeShader.handle(), ModelViewMatrix);
		}
		else if ((temp->getEle().compare("H") == 0) && drawHydrogens) //can add hydrogen here!
		{
			//temp->renderSimple();
			temp->renderUsingVBO(moleculeShader.handle(), ModelViewMatrix);

		}
	}
	SuperAtom::Unbind(moleculeShader.handle());
}




void reshape(int width, int height) {		// Resize the OpenGL window
	screenWidth = width; screenHeight = height;           // to ensure the mouse coordinates match 
	// we will use these values to set the coordinate system

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	//glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	//glLoadIdentity();									// Reset The Projection Matrix

	//set up a perspective view (fov, aspect ratio, near, far)
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,4000.0f);
	ProjectionMatrix = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 8000.0f);

	//glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	//glLoadIdentity();									// Reset The Modelview Matrix
}


void init() {
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	
	//glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to black
	//glEnable(GL_DEPTH_TEST);				//Enables depth testing - z-buffer is used to store depths of pixels.

	SuperAtom::hapticRadius = 0;

	cout << "Loading files..." << endl;
	FileHandle file_handle;
	
	//file_handle.openFile("PDB Test Files\\1CRN(327).pdb", false);
	//file_handle.openFile("PDB Test Files\\1CRN(327).dssp", true);		//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\5ADH(510).pdb", false);
	//file_handle.openFile("PDB Test Files\\5ADH(510).dssp", true);		//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\1OGZ(1057).pdb", false);
	//file_handle.openFile("PDB Test Files\\1OGZ(1057).dssp", true);	//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\1ANF(2860).pdb", false);
	//file_handle.openFile("PDB Test Files\\1ANF(2860).dssp", true);		//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\5ADH(3127).pdb", false);
	//file_handle.openFile("PDB Test Files\\5ADH(3127).dssp", true);	//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\1ADG(3601).pdb", false);
	//file_handle.openFile("PDB Test Files\\1ADG(3601).dssp", true);	//Loads DSSP file

	file_handle.openFile("PDB Test Files\\7BJW(4820).pdb", false);
	file_handle.openFile("PDB Test Files\\7BJW(4820).dssp", true);		//Loads DSSP file

	//file_handle.openFile("PDB Test Files\\1BKS(5231).pdb", false);
	//file_handle.openFile("PDB Test Files\\1BKS(5231).dssp", true);	//Loads DSSP file
	

	//Obtain the list of atoms from the protein
	theList = file_handle.getAtomList();
	myListPtr = &theList[0];
	myListSize = theList.size();

	cout << "This file contains " << myListSize << " atoms. " << endl;

	//PrintOutPLY("3E76.ply");

	ZOOM = file_handle.getFurthestDistanceToMidPoint();
	//cout << "RADIUS OF BOUNDING CIRCLE " << ZOOM << endl;

	//find radius of largest atom
	float radius = 0;
	for (int i = 0; i < myListSize; i++)
	{
		Atom* temp = myListPtr[i];
		float r = temp->getRadius();
		if (r > radius)
			radius = r;
	}

	//ZOOM the molecule back 
	ZOOM = (ZOOM / tan((45 * 0.5)*(3.141 / 180.0))) + radius;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glShadeModel(GL_SMOOTH);							// Enable smooth shading
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);				// White background
	glEnable(GL_DEPTH_TEST);							// Enables depth testing
	//glDepthFunc(GL_LEQUAL);


	//Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//Molecular model initialization
	//if(!myShader.load("BasicView", "shaders/basicTransformations.vert", "shaders/basicTransformations.frag"))
	if (!moleculeShader.load("BasicView", "shaders/basicSpecular.vert", "shaders/basicSpecular.frag"))
	{
		cout << "failed to load shader" << endl;
	}
	glUseProgram(moleculeShader.handle());
	SuperAtom::constructUnitSphere(16, moleculeShader.handle());


	//Cartoon shader/model initialisation
	const char* vertPath = "shaders/cartoonModelShader.vert";
	const char* fragPath = "shaders/cartoonModelShader.frag";
	CM.loadShader(vertPath, fragPath);
	CM.initCartoonModel(theList);
}


void processKeys() {
	if (keys[VK_ADD]) {
		ZOOM -= 5.0f;
	}
	else if (keys[VK_SUBTRACT]) {
		ZOOM += 5.0f;
	}

	if (keys[VK_UP]) {
		xyMove.y += 3.0f;
	}
	else if (keys[VK_DOWN]) {
		xyMove.y -= 3.0f;
	}

	if (keys[VK_LEFT]) {
		xyMove.x -= 3.0f;
	}
	else if (keys[VK_RIGHT]) {
		xyMove.x += 3.0f;
	}

	if (keys['O']) {
		SPIN -= 1.5f;
	}
	else if (keys['P']) {
		SPIN += 1.5f;
	}

	if (keys['C']) {
		if (isCartoon) {
			isCartoon = false;
			mTransparency = 1.0f;
			glUniform1f(glGetUniformLocation(moleculeShader.handle(), "transparency"), mTransparency);
		}
		else {
			isCartoon = true;
			mTransparency = 0.2f;
			glUniform1f(glGetUniformLocation(moleculeShader.handle(), "transparency"), mTransparency);
		}
		keys['C'] = false;
	}

	if (keys['V']) {
		if (isCartoonSpin) {
			isCartoonSpin = false;
		}
		else {
			isCartoonSpin = true;
		}
		keys['V'] = false;
	}

	if (keys['T']) {
		if (isTransparent) {
			isTransparent = false;
		}
		else {
			isTransparent = true;
		}
		keys['T'] = false;
	}

	if (keys['M']) {
		if (isMolecule) {
			isMolecule = false;
		}
		else {
			isMolecule = true;
		}
		keys['M'] = false;
	}
}


void update()
{
	spin += 0.05;
	if (spin > 360)
		spin = 0;
}

void updateCartoonSpin() {
	SPIN += 0.05f;
	if (SPIN > 360.0f)
		SPIN = 0.0f;
}


/**************** END OPENGL FUNCTIONS *************************/
//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

//win32 global variabless
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application


/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	//console.Open();

	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	MSG		msg;									// Windows Message Structure
	bool	done = false;								// Bool Variable To Exit Loop

	// Create Our OpenGL Window
	if (!CreateGLWindow("OpenGL Win32 Example", screenWidth, screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if (keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard

			display();					// Draw The Scene
			update();					// update variables
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
	}

	//console.Close();

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}
	break;

	case WM_SIZE:								// Resize The OpenGL Window
	{
		reshape(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	break;

	case WM_LBUTTONDOWN:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
		LeftPressed = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		LeftPressed = false;
	}
	break;

	case WM_MOUSEMOVE:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
	}
	break;
	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = true;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}
	break;
	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = false;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*/

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;

	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
		cout << " not possible to make context " << endl;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	cout << "OpenGL version: " << GLVersionString << endl;

	//We can check the version in OpenGL 
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	cout << "OpenGL Version: " << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window

	init();

	reshape(width, height);     					// Set Up Our Perspective GL Screen

	return true;		// Success
}








//if (p == 0) {
//	lastFrontNorm = frontNorm;
//	lastBackNorm = backNorm;
//	lastLeftNorm = leftNorm;
//	lastRightNorm = rightNorm;
//	lastTopNorm = topNorm;
//	lastBottomNorm = bottomNorm;
//}
////else {
//	//Face Normal interpolation
//	/*glm::vec3 frontTopRightNormal = glm::normalize(rightNorm + topNorm + lastRightNorm + lastTopNorm);
//	glm::vec3 frontTopLeftNormal = glm::normalize(leftNorm + topNorm + lastLeftNorm + lastTopNorm);
//	glm::vec3 frontBottomLeftNormal = glm::normalize(leftNorm + bottomNorm + lastLeftNorm + lastBottomNorm);
//	glm::vec3 frontBottomRightNormal = glm::normalize(rightNorm + bottomNorm + lastRightNorm + lastBottomNorm);
//
//	glm::vec3 backTopRightNormal = glm::normalize(rightNorm + topNorm + lastRightNorm + lastTopNorm);
//	glm::vec3 backTopLeftNormal = glm::normalize(leftNorm + topNorm + lastLeftNorm + lastTopNorm);
//	glm::vec3 backBottomLeftNormal = glm::normalize(leftNorm + bottomNorm + lastLeftNorm + lastBottomNorm);
//	glm::vec3 backBottomRightNormal = glm::normalize(rightNorm + bottomNorm + lastRightNorm + lastBottomNorm);*/
//
//glm::vec3 frontTopNormal = glm::normalize(lastTopNorm);
//glm::vec3 backTopNormal = glm::normalize(topNorm);
//glm::vec3 frontBottomNormal = glm::normalize(lastBottomNorm);
//glm::vec3 backBottomNormal = glm::normalize(bottomNorm);
//glm::vec3 frontLeftNormal = glm::normalize(lastLeftNorm);
//glm::vec3 backLeftNormal = glm::normalize(leftNorm);
//glm::vec3 frontRightNormal = glm::normalize(lastRightNorm);
//glm::vec3 backRightNormal = glm::normalize(rightNorm);
//glm::vec3 frontNormal = glm::normalize(frontNorm);
//glm::vec3 backNormal = glm::normalize(backNorm);
//
//if (isFrontFace) {
//	if (isFrontAndBackFlipped) {
//		//Flip Front Normals
//		vertexNormals[normIndex] = frontTopRightNormal;			//2
//		vertexNormals[normIndex + 1] = frontBottomRightNormal;	//1
//		vertexNormals[normIndex + 2] = frontBottomLeftNormal;	//0
//
//		vertexNormals[normIndex + 3] = frontBottomLeftNormal;	//0
//		vertexNormals[normIndex + 4] = frontTopLeftNormal;		//3
//		vertexNormals[normIndex + 5] = frontTopRightNormal;		//2
//	}
//	else {
//		//Keep Front Normals
//		vertexNormals[normIndex] = frontBottomLeftNormal;		//0
//		vertexNormals[normIndex + 1] = frontBottomRightNormal;	//1
//		vertexNormals[normIndex + 2] = frontTopRightNormal;		//2
//
//		vertexNormals[normIndex + 3] = frontTopRightNormal;		//2
//		vertexNormals[normIndex + 4] = frontTopLeftNormal;		//3
//		vertexNormals[normIndex + 5] = frontBottomLeftNormal;	//0
//	}
//}
//else {
//	normIndex -= 6;
//}
//
//if (isBackFace) {
//	if (isFrontAndBackFlipped) {
//		//Flip Back Normals
//		vertexNormals[normIndex + 6] = backTopLeftNormal;		//6
//		vertexNormals[normIndex + 7] = backBottomLeftNormal;	//5
//		vertexNormals[normIndex + 8] = backBottomRightNormal;	//4
//
//		vertexNormals[normIndex + 9] = backBottomRightNormal;	//4
//		vertexNormals[normIndex + 10] = backTopRightNormal;		//7
//		vertexNormals[normIndex + 11] = backTopLeftNormal;		//6
//	}
//	else {
//		//Keep Back Normals
//		vertexNormals[normIndex + 6] = backBottomRightNormal;	//4
//		vertexNormals[normIndex + 7] = backBottomLeftNormal;	//5
//		vertexNormals[normIndex + 8] = backTopLeftNormal;		//6
//
//		vertexNormals[normIndex + 9] = backTopLeftNormal;		//6
//		vertexNormals[normIndex + 10] = backTopRightNormal;		//7
//		vertexNormals[normIndex + 11] = backBottomRightNormal;	//4
//	}
//}
//else {
//	normIndex -= 6;
//}
//
//if (isLeftAndRightFlipped) {
//	//Flip Left Normals
//	vertexNormals[normIndex + 12] = frontTopLeftNormal;		//3
//	vertexNormals[normIndex + 13] = frontBottomLeftNormal;	//0
//	vertexNormals[normIndex + 14] = backBottomLeftNormal;	//5
//
//	vertexNormals[normIndex + 15] = backBottomLeftNormal;	//5
//	vertexNormals[normIndex + 16] = backTopLeftNormal;		//6
//	vertexNormals[normIndex + 17] = frontTopLeftNormal;		//3
//
//	//Flip Right Normals
//	vertexNormals[normIndex + 18] = backTopRightNormal;		//7
//	vertexNormals[normIndex + 19] = backBottomRightNormal;	//4
//	vertexNormals[normIndex + 20] = frontBottomRightNormal;	//1
//
//	vertexNormals[normIndex + 21] = frontBottomRightNormal;	//1
//	vertexNormals[normIndex + 22] = frontTopRightNormal;	//2
//	vertexNormals[normIndex + 23] = backTopRightNormal;		//7
//}
//else {
//	//Keep Left Normals
//	vertexNormals[normIndex + 12] = backBottomLeftNormal;	//5
//	vertexNormals[normIndex + 13] = frontBottomLeftNormal;	//0
//	vertexNormals[normIndex + 14] = frontTopLeftNormal;		//3
//
//	vertexNormals[normIndex + 15] = frontTopLeftNormal;		//3
//	vertexNormals[normIndex + 16] = backTopLeftNormal;		//6
//	vertexNormals[normIndex + 17] = backBottomLeftNormal;	//5
//
//	//Keep Right Normals
//	vertexNormals[normIndex + 18] = frontBottomRightNormal;	//1
//	vertexNormals[normIndex + 19] = backBottomRightNormal;	//4
//	vertexNormals[normIndex + 20] = backTopRightNormal;		//7
//
//	vertexNormals[normIndex + 21] = backTopRightNormal;		//7
//	vertexNormals[normIndex + 22] = frontTopRightNormal;	//2
//	vertexNormals[normIndex + 23] = frontBottomRightNormal;	//1
//}
//
//if (isTopAndBottomFlipped) {
//	//Flip Top Normals
//	vertexNormals[normIndex + 24] = backTopRightNormal;		//7
//	vertexNormals[normIndex + 25] = frontTopRightNormal;	//2 
//	vertexNormals[normIndex + 26] = frontTopLeftNormal;		//3
//
//	vertexNormals[normIndex + 27] = frontTopLeftNormal;		//3
//	vertexNormals[normIndex + 28] = backTopLeftNormal;		//6
//	vertexNormals[normIndex + 29] = backTopRightNormal;		//7
//
//	//Flip Bottom Normals
//	vertexNormals[normIndex + 30] = frontBottomRightNormal;	//1
//	vertexNormals[normIndex + 31] = backBottomRightNormal;	//4
//	vertexNormals[normIndex + 32] = backBottomLeftNormal;	//5
//
//	vertexNormals[normIndex + 33] = backBottomLeftNormal;	//5
//	vertexNormals[normIndex + 34] = frontBottomLeftNormal;	//0
//	vertexNormals[normIndex + 35] = frontBottomRightNormal;	//1
//}
//else {
//	//Keep Top Normals
//	vertexNormals[normIndex + 24] = frontTopLeftNormal;		//3
//	vertexNormals[normIndex + 25] = frontTopRightNormal;	//2 
//	vertexNormals[normIndex + 26] = backTopRightNormal;		//7
//
//	vertexNormals[normIndex + 27] = backTopRightNormal;		//7
//	vertexNormals[normIndex + 28] = backTopLeftNormal;		//6
//	vertexNormals[normIndex + 29] = frontTopLeftNormal;		//3
//
//	//Keep Bottom Normals
//	vertexNormals[normIndex + 30] = backBottomLeftNormal;	//5
//	vertexNormals[normIndex + 31] = backBottomRightNormal;	//4
//	vertexNormals[normIndex + 32] = frontBottomRightNormal;	//1
//
//	vertexNormals[normIndex + 33] = frontBottomRightNormal;	//1
//	vertexNormals[normIndex + 34] = frontBottomLeftNormal;	//0
//	vertexNormals[normIndex + 35] = backBottomLeftNormal;	//5
//}


///ERROR
	///file_handle.openFile("PDB Test Files\\2EB2(2459).pdb", false);
	///file_handle.openFile("PDB Test Files\\2EB2(2459).dssp", true);	//Loads DSSP file
