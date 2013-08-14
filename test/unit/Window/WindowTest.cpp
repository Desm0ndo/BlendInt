// cpp

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <stdio.h>

#include <BIL/Application.h>

#include <BIL/Window.h>
#include <BIL/Label.h>
#include <BIL/Widget.h>

#include "WindowTest.h"

using namespace BIL;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION (WindowTest);

myWindow::myWindow(int width, int height, const char *title, GLFWmonitor *monitor,
        GLFWwindow *share, Traceable* parent)
: BIL::Window( width, height, title, monitor, share, parent)
{

}

void myWindow::cursorEnterEvent(int entered)
{
	if (entered == GL_TRUE) {
		cout << "This time show message in derived class" << endl;
	}
}

myWidget::myWidget()
	: Widget (NULL)
{
	resize (200, 100);
}

myWidget::~myWidget()
{

}

void myWidget::render (void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(1.0f, 0.9f, 1.0f, 0.95f);

	drawRoundBox(pos_.coord.x,
				 pos_.coord.y,
				 pos_.coord.x + size_.vec.x,
				 pos_.coord.y + size_.vec.y,
				 5.0);
	drawShadowBox(pos_.coord.x,
				  pos_.coord.y,
				  pos_.coord.x + size_.vec.x,
				  pos_.coord.y + size_.vec.y,
				  5.0,
				  64);

	glDisable(GL_BLEND);
}

void myWidget::MousePressEvent (MouseEvent* event)
{
	cout << "Click at" << event->pos().coord.x << " "
		 << event->pos().coord.y << endl;

	cout << "and window position: " << event->windowPos().coord.x << " "
		 << event->windowPos().coord.y << endl;

	event->Accept();
}

WindowTest::WindowTest ()
{

}

WindowTest::~WindowTest ()
{

}

void WindowTest::setUp ()
{

}

void WindowTest::tearDown ()
{

}

void WindowTest::event1 ()
{
    Application app;

    Window win(640, 480, "640 x 480 WindowTest Event1", NULL, NULL);

    app.setMainWindow(&win);
    app.initialize();

    Label label(L"Test Event1");
    label.setPos (Coord2f(200.0, 200.0));
    label.setParent (&win);

    app.run();

    CPPUNIT_ASSERT(true);
}

void WindowTest::event2 ()
{
    Application app;

    myWindow win(640, 480, "640 x 480 WindowTest Event2", NULL, NULL);

    app.setMainWindow(&win);
    app.initialize();

    Label label(L"Test Event2");
    label.setPos (Coord2f(300.0, 200.0));
    label.setParent (&win);

    app.run();

    CPPUNIT_ASSERT(true);
}

void WindowTest::solorender1 ()
{
    Application app;

    myWindow win(640, 480, "640 x 480 WindowTest Event2", NULL, NULL);

    app.setMainWindow(&win);
    app.initialize();

    Label label(L"Test Event2");
    label.setPos (Coord2f(100.0, 100.0));
    label.setParent (&win);

		Label solo (L"This label should also been rendered");
		solo.setPos(400.0, 300.0, 0.0);

    app.run();

    CPPUNIT_ASSERT(true);
}

void WindowTest::check_mouse_press1 ()
{
    Application app;

    myWindow win(640, 480, "640 x 480 WindowTest Event2", NULL, NULL);

    app.setMainWindow(&win);
    app.initialize();
	
	myWidget widget;
	widget.setParent (&win);
	widget.setPos(300.0, 300.0, 0.0);

    app.run();

    CPPUNIT_ASSERT(true);
}