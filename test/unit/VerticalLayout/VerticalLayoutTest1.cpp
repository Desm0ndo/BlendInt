#include "VerticalLayoutTest1.hpp"
#include <BlendInt/Interface.hpp>
#include <BlendInt/VerticalLayout.hpp>

using namespace BlendInt;

VerticalLayoutTest1::VerticalLayoutTest1()
: testing::Test()
{
	// TODO: add constructor code
}

VerticalLayoutTest1::~VerticalLayoutTest1()
{
	// TODO: add destructor code
}

/**
 * Test the default behavior
 *
 * Expected result: the 2 widgets should fill the layout and resize with the preferred size
 */
TEST_F(VerticalLayoutTest1, Add1)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add1");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
	widget1->SetExpand(false);
	Widget* widget2 = new Widget;
	widget1->SetExpand(false);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if HLayout geometry is locked
 *
 * Expected result: the 2 widgets should fill the layout
 *
 * Note: this test is only valid in DEBUG build
 */
TEST_F(VerticalLayoutTest1, Add2)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add2");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);
	vlayout->Resize(200, 400);

#ifdef DEBUG
	vlayout->lock(true);
#endif

	Widget* widget1 = new Widget;
	widget1->SetExpand(true);
	Widget* widget2 = new Widget;
	widget2->SetExpand(true);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if HLayout geometry is locked and 1 expandable + 1 fixed size
 *
 * Expected result: the 2 widgets should fill the layout
 *
 * Note: this test is only valid in DEBUG build
 */
TEST_F(VerticalLayoutTest1, Add3)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add3");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);
	vlayout->Resize(200, 400);

#ifdef DEBUG
	vlayout->lock(true);
#endif

	Widget* widget1 = new Widget;
	widget1->SetExpandX(true);
	widget1->SetExpandY(false);
	Widget* widget2 = new Widget;
	widget2->SetExpand(true);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if more than 1 expandable child with max width 
 *
 * Expected result: the widgets should fill the layout
 */
TEST_F(VerticalLayoutTest1, Add4)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add4");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);
	vlayout->Resize(200, 400);

	Widget* widget1 = new Widget;
	widget1->SetExpandY(true);
    widget1->SetPreferredSize(widget1->preferred_size().width(), 40);
    widget1->SetMaximalSize(widget1->maximal_size().width(), 80);
	Widget* widget2 = new Widget;
	widget2->SetExpandY(true);
    widget2->SetPreferredSize(widget2->preferred_size().width(), 40);
    widget2->SetMaximalSize(widget2->maximal_size().width(), 80);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if more than 1 expandable child with max width and 1 more fixed child
 *
 * Expected result: the widgets should fill the layout
 */
TEST_F(VerticalLayoutTest1, Add5)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add5");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);
	vlayout->Resize(200, 600);

	Widget* widget1 = new Widget;
	widget1->SetExpandY(true);
    //widget1->SetPreferredSize(widget1->preferred_size().width(), 40);
    //widget1->SetMaximalSize(widget1->preferred_size().width(), 80);
	Widget* widget2 = new Widget;
	widget2->SetExpandY(true);
    widget2->SetPreferredSize(widget1->preferred_size().width(), 40);
    widget2->SetMaximalSize(widget1->preferred_size().width(), 80);
	Widget* widget3 = new Widget;
	widget3->SetExpandY(true);
    widget3->SetPreferredSize(widget1->preferred_size().width(), 40);
    widget3->SetMaximalSize(widget1->preferred_size().width(), 60);
	Widget* widget4 = new Widget;
	//widget4->SetExpandY(true);
    //widget4->SetPreferredSize(widget1->preferred_size().width(), 40);
    //widget4->SetMaximalSize(widget1->preferred_size().width(), 70);
	widget4->Resize(widget4->size().width(), 40);
	
    vlayout->Add(widget1);
	vlayout->Add(widget2);
	vlayout->Add(widget3);
	vlayout->Add(widget4);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if more than 1 expandable child and 1 more fixed child in small size
 *
 * Expected result: the widgets should fill the layout
 */
TEST_F(VerticalLayoutTest1, Add6)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add6");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);
	vlayout->Resize(100, 50);

#ifdef DEBUG
	vlayout->lock(true);
#endif

	Widget* widget1 = new Widget;
	widget1->SetExpandY(true);
    //widget1->SetPreferredSize(widget1->preferred_size().height(), 40);
    //widget1->SetMaximalSize(widget1->maximal_size().height(), 80);
	Widget* widget2 = new Widget;
    widget2->SetMinimalSize(widget2->minimal_size().height(), 10);
	Widget* widget3 = new Widget;
    widget3->SetMinimalSize(widget3->minimal_size().height(), 20);
	Widget* widget4 = new Widget;
	widget4->SetExpandY(true);
	
    vlayout->Add(widget1);
	vlayout->Add(widget2);
	vlayout->Add(widget3);
	vlayout->Add(widget4);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test behavior if more than 1 expandable child and 1 more fixed child in small size
 *
 * Expected result: the widgets should fill the layout
 */
TEST_F(VerticalLayoutTest1, Add7)
{
	Init ();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Add7");

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
	//widget1->SetExpandX(true);
	//widget1->SetMinimalSize(10, widget1->minimal_size().height());
	Widget* widget2 = new Widget;
    widget2->SetMinimalSize(30, widget2->minimal_size().height());
	Widget* widget3 = new Widget;
    widget3->SetMinimalSize(20, widget3->minimal_size().height());
	Widget* widget4 = new Widget;

	vlayout->Add(widget1);
	vlayout->Add(widget2);
	vlayout->Add(widget3);
	vlayout->Add(widget4);

	vlayout->Resize(100, 80);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}


/**
 * Test resize
 *
 * Expected result: the children in the HLayout resize too according to the layout size
 */
TEST_F(VerticalLayoutTest1, Resize1)
{
	Init();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Resize1");

	// add test code here

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
	widget1->SetExpand(true);
	Widget* widget2 = new Widget;
	widget2->SetExpand(true);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	vlayout->Resize(200, 400);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test resize, no child is expandable
 *
 * Expected result: the children in the HLayout resize too according to the layout size
 */
TEST_F(VerticalLayoutTest1, Resize2)
{
	Init();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Resize2");

	// add test code here

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
	Widget* widget2 = new Widget;

	vlayout->Add(widget1);
	vlayout->Add(widget2);

	vlayout->Resize(200, 400);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test resize to small size
 *
 * Expected result: the children in the HLayout resize too according to the layout size
 */
TEST_F(VerticalLayoutTest1, Resize3)
{
	Init();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - Resize3");

	// add test code here

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
	Widget* widget2 = new Widget;
    widget2->SetMinimalSize(30, widget2->minimal_size().height());
	Widget* widget3 = new Widget;
    widget3->SetMinimalSize(20, widget3->minimal_size().height());
	Widget* widget4 = new Widget;

	vlayout->Add(widget1);
	vlayout->Add(widget2);
	vlayout->Add(widget3);
	vlayout->Add(widget4);

	vlayout->Resize(100, 80);

	Interface::Instance()->Bind(vlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test SetMargin
 *
 * Expected result: the children in the HLayout resize too according to the new margin
 */
TEST_F(VerticalLayoutTest1, SetMargin1)
{
	Init();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - SetMargin1");

	// add test code here

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
    widget1->SetExpand(true);
    widget1->Resize(100, 80);
	Widget* widget2 = new Widget;
    widget2->SetExpand(true);
    widget2->Resize(100, 80);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

    vlayout->Resize(200, 400);
	Interface::Instance()->Bind(vlayout);

    vlayout->SetMargin(20, 20, 20, 20);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}

/**
 * Test SetSpace
 *
 * Expected result: the children in the HLayout resize too according to the new space
 */
TEST_F(VerticalLayoutTest1, SetSpace1)
{
	Init();
	GLFWwindow* window = CreateWindow("VerticalLayoutTest1 - SetSpace1");

	// add test code here

	VerticalLayout* vlayout = new VerticalLayout;
	vlayout->SetPosition(200, 200);

	Widget* widget1 = new Widget;
    widget1->SetExpand(true);
    widget1->Resize(100, 80);
	Widget* widget2 = new Widget;
    widget2->SetExpand(true);
    widget2->Resize(100, 80);

	vlayout->Add(widget1);
	vlayout->Add(widget2);

    vlayout->Resize(200, 400);
	Interface::Instance()->Bind(vlayout);

    vlayout->SetSpace(10);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}