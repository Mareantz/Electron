//sfml library
#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

//Interface_Object.h class
#include "Interface_Object.h"

#define nullptr NULL
#define MAX_COMPONENTS 100
#define SCREEN_DIVIDER 4
#define MAXNR 64

using namespace std;
using namespace sf;

#pragma region variables
//window related variables
int windowHeight, windowWidth;
RenderWindow window;
RenderWindow edit;
RenderWindow helpWindow;
//two-dimensional pair of float elements
Vector2f centerScreen;
Vector2f previousMouseCoords;

//self-explinatory booleans
bool isHoldingClick = false;
bool isDraggingComponent = false;
//curent state of project
enum State {
	MainMenu,
	Project,
	Info,
}states;

//static-buttons
Button* mainMenu_buttons[MAX_COMPONENTS];
Button* project_buttons[MAX_COMPONENTS];

//electrical components related
ElectricalComponent* project_components[MAX_COMPONENTS];
ElectricalComponent* draggedComponent;

//counters
int buttonCount = 0;
int projectbuttonCount = 0;
int projectComponentCount = 0;
int editCount = 0;

//selected components
int selectedComponentIndex = -1;
FloatRect selectedComponentBounds;

//mouse variable
Mouse myMouse;

//line draw
Vector2f pointA, pointB;

//view-port shapes
RectangleShape use_space;
RectangleShape Edit;
pair<Vector2f, Vector2f> connectionLines[MAXNR];
int connectionLinesCount = 0;
//view-port sliders
//Slider rotationSlider(Vector2f(centerScreen.x + 810, centerScreen.y + 470));


//font
Font font;

Text backgroundText;
//BACKG. TEXT


bool canComponentsBeSelected = false;

#pragma endregion

bool isInDrawableArea(Vector2f mouseCoords, FloatRect shapeBounds) {
	if (mouseCoords.x > shapeBounds.left &&
		mouseCoords.x < shapeBounds.left + shapeBounds.width &&
		mouseCoords.y > shapeBounds.top &&
		mouseCoords.y < shapeBounds.top + shapeBounds.height)
		return true;

	return false;
}
//connection lines
list<VertexArray> line;
void computeLines(Vector2f pointA, Vector2f pointB) {
	auto vectorPair = make_pair(pointA, pointB);
	connectionLines[++connectionLinesCount] = vectorPair;
	VertexArray lines = VertexArray(sf::Lines, 6);
	auto distance = abs(pointA.x - pointB.x);
	lines[0].position = pointA;
	lines[1].position = Vector2f(pointA.x + distance / 2, pointA.y);
	lines[2].position = Vector2f(pointA.x + distance / 2, pointA.y);
	lines[3].position = Vector2f(pointA.x + distance / 2, pointB.y);
	lines[4].position = Vector2f(pointA.x + distance / 2, pointB.y);
	lines[5].position = pointB;

	for (int i = 0; i < lines.getVertexCount(); i++)
		lines[i].color = Color::Black;

	line.push_back(lines);
}

bool anySelectedButton() {
	for (int i = 1; i <= projectbuttonCount; i++) {
		if (project_buttons[i]->buttonState == Button::SELECTED)
			return true;
	}
	return false;
}

void deleteComponent(int index)
{
	int i = index;

	project_components[i] = nullptr;
	for (i; i < projectComponentCount; i++)
		project_components[i] = project_components[i + 1];
	project_components[i] = nullptr;
	projectComponentCount--;

}

//bounds for components
void drawLinesToBounds(FloatRect bounds, float offset = 5)
{
	bounds.left -= offset;
	bounds.height += offset * 2.125;
	bounds.top -= offset;
	bounds.width += offset * 2.125;

	VertexArray lines(Lines, 8);
	lines[0].position = Vector2f(bounds.left, bounds.top);
	lines[0].color = Color::Red;

	lines[1].position = Vector2f(bounds.left + bounds.width, bounds.top);
	lines[1].color = Color::Red;

	lines[2].position = Vector2f(bounds.left + bounds.width, bounds.top);
	lines[2].color = Color::Red;

	lines[3].position = Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
	lines[3].color = Color::Red;

	lines[4].position = Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
	lines[4].color = Color::Red;

	lines[5].position = Vector2f(bounds.left, bounds.top + bounds.height);
	lines[5].color = Color::Red;

	lines[6].position = Vector2f(bounds.left, bounds.top + bounds.height);
	lines[6].color = Color::Red;

	lines[7].position = Vector2f(bounds.left, bounds.top);
	lines[7].color = Color::Red;

	window.draw(lines);
}

void loadFont()
{
	if (!font.loadFromFile("Arialn.ttf")) {
		cout << "Couldn't load font. You must've install the above font in the same folder as the project to continue. Returning -1.";
		exit(0);
	}
}

bool areCoordsInBounds(Vector2f coords, FloatRect bounds)
{
	if (coords.x > bounds.left && coords.x < bounds.left + bounds.width && coords.y > bounds.top && coords.y < bounds.top + bounds.height)
		return true;
	return false;
}
//save button
void saveToFile()
{
	auto fout = ofstream("output.txt");
	fout << projectComponentCount << '\n';

	for (int i = 1; i <= projectComponentCount; i++)
	{
		fout << project_components[i]->type << " ";
		fout << project_components[i]->position.x << " " << project_components[i]->position.y << " ";
		fout << project_components[i]->rotation << " ";
		fout << project_components[i]->nume.str() << " " << project_components[i]->rezistenta.str() << " " << project_components[i]->voltaj.str();
		fout << '\n';
	}



	fout << connectionLinesCount << endl;
	cout << connectionLinesCount;
	for (int i = 1; i <= connectionLinesCount; i++)
	{
		fout << connectionLines[i].first.x << " " << connectionLines[i].first.y << " " << connectionLines[i].second.x << " " << connectionLines[i].second.y << '\n';
	}




	fout.close();
}

void resetProject()
{
	memset(project_components, 0, sizeof(project_components));
	memset(connectionLines, 0, sizeof(connectionLines));
	connectionLinesCount = 0;
	projectComponentCount = 0;
	line.clear();
	selectedComponentIndex = -1;
}

Button::Type returnType(int input)
{
	switch (input)
	{
	case 0:
		return Button::Null;
	case 1:
		return Button::Cell;
	case 2:
		return Button::Resistor;
	case 3:
		return Button::Bulb;
	case 4:
		return Button::Ammeter;
	case 5:
		return Button::Voltmeter;
	case 6:
		return Button::Capacitor;
	case 7:
		return Button::Transistor;
	case 8:
		return Button::Fuse;
	case 9:
		return Button::Diode;
	default:
		cout << "an error happened, couldn't load file";
		return Button::Null;
	}

}

//open button
void loadFromFile()
{

	auto fin = ifstream("output.txt");
	resetProject();
	int count;
	fin >> count;
	int type; Vector2f position; float rotation;
	projectComponentCount = 0;

	for (int i = 1; i <= count; i++)
	{
		fin >> type; fin >> position.x; fin >> position.y; fin >> rotation;
		project_buttons[1]->InstantiateComponent(returnType(type), project_components, projectComponentCount, position, Vector2f(1, 1));
		string nume, rezistenta, voltaj, line;
		getline(fin,line);
		istringstream ss(line);
		ss >> nume;
		ss >> rezistenta;
		ss >> voltaj;
		project_components[i]->nume << nume;
		project_components[i]->rezistenta << rezistenta;
		project_components[i]->voltaj << voltaj;
		
	}
	connectionLinesCount = 0;
	fin >> connectionLinesCount;
	Vector2f pointA, pointB;
	auto allConnectionLines = connectionLinesCount;
	for (int i = 1; i <= allConnectionLines; i++)
	{
		fin >> pointA.x >> pointA.y >> pointB.x >> pointB.y;
		computeLines(pointA, pointB);
	}

	fin.close();

}

void init()
{
	loadFont();
	backgroundText.setFont(font);
	backgroundText.setCharacterSize(200);
	backgroundText.setScale(Vector2f(.2, .2));
	backgroundText.setString("ELECTRON");
	backgroundText.setFillColor(Color::White);
	backgroundText.setPosition(centerScreen.x - backgroundText.getGlobalBounds().width / 2, centerScreen.y / 4);
}

int main() {
	//initialize loaded-objects (ex: font)


	windowHeight = VideoMode::getDesktopMode().height;
	windowWidth = VideoMode::getDesktopMode().width;
	window.create(VideoMode(windowWidth / SCREEN_DIVIDER * 2, windowHeight / SCREEN_DIVIDER * 2), "SFML Electron");

	states = MainMenu;

	window.setVerticalSyncEnabled(true);
	centerScreen.x = window.getPosition().x;
	centerScreen.y = window.getPosition().y;
	init();

	//declaration of static elements within the project
	Vector2f defaultComponentButtonSize = Vector2f(100, 30);
	Vector2f defaultProjectButtonSize = Vector2f(155, 30);

	Texture roTexture;
	roTexture.loadFromFile("romana.png");
	Sprite roSprite;
	roSprite.setTexture(roTexture, true);
	roSprite.setScale(.04, .04);
	roSprite.setPosition(10, 10);

	Texture enTexture;
	enTexture.loadFromFile("engleza.png");
	Sprite enSprite;
	enSprite.setTexture(enTexture, true);
	enSprite.setScale(.04, .04);
	enSprite.setPosition(2*centerScreen.x-88, 10);
	
	bool Lang = false;

#pragma region static-declarated buttons
	Button button_mainMenu_newProject(Vector2f(centerScreen.x, centerScreen.y - 100), Vector2f(150, 63), 0, Button::Null, "New Project");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_newProject;
	Button button_mainMenu_openProject(Vector2f(centerScreen.x, centerScreen.y - 20), Vector2f(150, 63), 0, Button::Null, "Open Project");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_openProject;
	Button button_mainMenu_info(Vector2f(centerScreen.x, centerScreen.y + 60), Vector2f(150, 63), 0, Button::Null, "Info");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_info;
	Button button_mainMenu_exit(Vector2f(centerScreen.x, centerScreen.y + 140), Vector2f(150, 63), 0, Button::Null, "Exit");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_exit;
	Button button_mainMenu_ro(Vector2f(48, 35), Vector2f(74, 48), 0, Button::Null, "");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_ro;
	Button button_mainMenu_en(Vector2f(2*centerScreen.x-49,33), Vector2f(74,45), 0, Button::Null, "");
	mainMenu_buttons[++buttonCount] = &button_mainMenu_en;
	Button button_project_cell(Vector2f(centerScreen.x - 425, centerScreen.y - 200), defaultComponentButtonSize, 0, Button::Cell, "Cell");
	project_buttons[++projectbuttonCount] = &button_project_cell;
	Button button_project_resistor(Vector2f(centerScreen.x - 425, centerScreen.y - 150), defaultComponentButtonSize, 0, Button::Resistor, "Resistor");
	project_buttons[++projectbuttonCount] = &button_project_resistor;
	Button button_project_bulb(Vector2f(centerScreen.x - 425, centerScreen.y - 100), defaultComponentButtonSize, 0, Button::Bulb, "Bulb");
	project_buttons[++projectbuttonCount] = &button_project_bulb;
	Button button_project_diode(Vector2f(centerScreen.x - 425, centerScreen.y - 50), defaultComponentButtonSize, 0, Button::Diode, "Diode");
	project_buttons[++projectbuttonCount] = &button_project_diode;
	Button button_project_ammeter(Vector2f(centerScreen.x - 425, centerScreen.y), defaultComponentButtonSize, 0, Button::Ammeter, "Ammeter");
	project_buttons[++projectbuttonCount] = &button_project_ammeter;
	Button button_project_voltmeter(Vector2f(centerScreen.x - 425, centerScreen.y + 50), defaultComponentButtonSize, 0, Button::Voltmeter, "Voltmeter");
	project_buttons[++projectbuttonCount] = &button_project_voltmeter;
	Button button_project_capacitor(Vector2f(centerScreen.x - 425, centerScreen.y + 100), defaultComponentButtonSize, 0, Button::Capacitor, "Capacitor");
	project_buttons[++projectbuttonCount] = &button_project_capacitor;
	Button button_project_transistor(Vector2f(centerScreen.x - 425, centerScreen.y + 150), defaultComponentButtonSize, 0, Button::Transistor, "Transistor");
	project_buttons[++projectbuttonCount] = &button_project_transistor;
	Button button_project_fuse(Vector2f(centerScreen.x - 425, centerScreen.y + 200), defaultComponentButtonSize, 0, Button::Fuse, "Fuse");
	project_buttons[++projectbuttonCount] = &button_project_fuse;
	Button button_project_back(Vector2f(centerScreen.x - 400, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Back");
	project_buttons[++projectbuttonCount] = &button_project_back;
	Button button_project_save(Vector2f(centerScreen.x - 240, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Save");
	project_buttons[++projectbuttonCount] = &button_project_save;
	Button button_project_saveas(Vector2f(centerScreen.x - 80, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Save As");
	project_buttons[++projectbuttonCount] = &button_project_saveas;
	Button button_project_open(Vector2f(centerScreen.x + 80, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Open");
	project_buttons[++projectbuttonCount] = &button_project_open;
	Button button_project_reset(Vector2f(centerScreen.x + 240, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Reset");
	project_buttons[++projectbuttonCount] = &button_project_reset;
	Button button_project_help(Vector2f(centerScreen.x + 400, centerScreen.y - 250), defaultProjectButtonSize, 0, Button::Null, "Help");
	project_buttons[++projectbuttonCount] = &button_project_help;
	Button button_project_undo(Vector2f(centerScreen.x - 415, centerScreen.y + 250), Vector2f(125, 30), 0, Button::Null, "Undo");
	project_buttons[++projectbuttonCount] = &button_project_undo;
	Button button_project_redo(Vector2f(centerScreen.x - 280, centerScreen.y + 250), Vector2f(125, 30), 0, Button::Null, "Redo");
	project_buttons[++projectbuttonCount] = &button_project_redo;
	Button button_project_edit(Vector2f(centerScreen.x - 145, centerScreen.y + 250), Vector2f(125, 30), 0, Button::Null, "Edit");
	project_buttons[++projectbuttonCount] = &button_project_edit;

	Button button_info_back(Vector2f(centerScreen.x - 425, centerScreen.y - 250), Vector2f(100, 30), 0, Button::Null, "Back");
	Button* BackButton = &button_info_back;

	//edit button ON and OFF text
	Text edit_button_text = Text("undefined", font, 100);
	edit_button_text.setPosition(centerScreen.x - 113, centerScreen.y + 233);
	edit_button_text.setScale(.13, .13);

	Button button_project_delete(Vector2f(centerScreen.x - 10, centerScreen.y + 250), Vector2f(125, 30), 0, Button::Null, "Delete");
	project_buttons[++projectbuttonCount] = &button_project_delete;



#pragma endregion

#pragma region view-port elements init
	use_space.setSize(Vector2f(820, 450));
	use_space.setFillColor(Color(255, 255, 255, 255));
	use_space.setOutlineColor(Color::Red);
	use_space.setOutlineThickness(1);
	use_space.setPosition(Vector2f(centerScreen.x - 360, centerScreen.y - 225));
#pragma endregion

	while (window.isOpen()) {
		centerScreen.x = window.getPosition().x;
		centerScreen.y = window.getPosition().y;

		//if componenets can be selected it'll be ON and green, otherwise OFF and red
		if (canComponentsBeSelected)
		{
			edit_button_text.setString("ON");
			edit_button_text.setFillColor(Color::Green);
		}
		else
		{
			edit_button_text.setString("OFF");
			edit_button_text.setFillColor(Color::Red);
		}

		Event event;
		while (window.pollEvent(event)) {
			window.clear(Color::Black);

			if (event.type == Event::Closed) {
				window.close();
			}

			auto mouseCoords = window.mapPixelToCoords(myMouse.getPosition(window));
			switch (states) {

				//logica de a apasa butoanele din meniu
			case MainMenu:
				for (int i = 1; i <= buttonCount; i++) {
					auto buttonBounds = mainMenu_buttons[i]->buttonShape.getGlobalBounds();
					if (areCoordsInBounds(mouseCoords, buttonBounds))
						mainMenu_buttons[i]->buttonState = (event.type == Event::MouseButtonPressed) ? Button::CLICKED : Button::HOVERED;
					else
						mainMenu_buttons[i]->buttonState = Button::INACTIVE;
				}
				break;
				//logica de a apasa butoanele din fereastra proiect
			case Project:
				for (int i = 1; i <= projectbuttonCount; i++) {
					auto buttonBounds = project_buttons[i]->buttonShape.getGlobalBounds();
					if (!(areCoordsInBounds(mouseCoords, buttonBounds)) && project_buttons[i]->buttonState == Button::SELECTED && event.type == Event::MouseButtonPressed) {
						if (isInDrawableArea(mouseCoords, use_space.getGlobalBounds()))
							project_buttons[i]->InstantiateComponent(project_buttons[i]->typeToCast, project_components, projectComponentCount, mouseCoords + Vector2f(-35, -27), Vector2f(.2, .2));
						project_buttons[i]->buttonState = Button::INACTIVE;

					}
					if (areCoordsInBounds(mouseCoords, buttonBounds)) {
						if (project_buttons[i]->buttonState != Button::SELECTED)
							project_buttons[i]->buttonState = (event.type == Event::MouseButtonPressed) ? Button::CLICKED : Button::HOVERED;
					}
					else
						if (project_buttons[i]->buttonState != Button::SELECTED)
							project_buttons[i]->buttonState = Button::INACTIVE;

				}
				if (!anySelectedButton()) {
					if (isInDrawableArea(mouseCoords, use_space.getGlobalBounds()) && event.type == Event::MouseButtonPressed && !isHoldingClick) {
						isHoldingClick = true;
						pointA = Vector2f(mouseCoords.x, mouseCoords.y);
					}
					else if (isInDrawableArea(mouseCoords, use_space.getGlobalBounds()) && event.type == Event::MouseButtonReleased && isHoldingClick) {
						//if (sqrt(mouseCoords.x - pointA.x) * (mouseCoords.x - pointA.x) + (mouseCoords.y - pointA.y) * (mouseCoords.y - pointA.y) > 5)
						isHoldingClick = false;
						if (!isDraggingComponent)
						{
							/*bool ok;
							for (int j = 1; j <= projectComponentCount; j++)
							{
								FloatRect bounds = project_components[j]->shape.getBounds();
								ok=bounds.contains(pointA);
							}
							if(!ok)*/
								computeLines(pointA, mouseCoords);
						}
					}
				}


				break;
			case Info:
					auto buttonBounds = BackButton->buttonShape.getGlobalBounds();
					if (areCoordsInBounds(mouseCoords, buttonBounds))
						BackButton->buttonState = (event.type == Event::MouseButtonPressed) ? Button::CLICKED : Button::HOVERED;
					else
						BackButton->buttonState = Button::INACTIVE;
				break;
			}
		}



		//iterate all buttons and check for state changes
		switch (states) {
		case MainMenu:
			for (int i = 1; i <= buttonCount; i++) {
				switch (mainMenu_buttons[i]->buttonState) {
				case(Button::INACTIVE):
					mainMenu_buttons[i]->buttonShape.setFillColor(Color::Black);
					break;
				case(Button::HOVERED):
					mainMenu_buttons[i]->buttonShape.setFillColor(Color::Cyan);
					break;
				case(Button::CLICKED):
					cout << i << " has been clicked." << endl;
					if (i == 1)
					{
						states = Project;
						resetProject();
					}
					if (i == 3)
						states = Info;
					if (i == 2)
						states = Project;
					if (i == 4)
						return 0;
					//language changes
					if (i == 5)
					{
						Lang = true;
						mainMenu_buttons[1]->buttonText.setString("Proiect nou");
						mainMenu_buttons[2]->buttonText.setString("Deschide un proiect");
						mainMenu_buttons[3]->buttonText.setString("Informatii");
						mainMenu_buttons[4]->buttonText.setString("Iesi");
						for (int j = 1; j <= 4; j++)
							mainMenu_buttons[j]->buttonText.setPosition(Vector2f(mainMenu_buttons[j]->position.x + mainMenu_buttons[j]->buttonShape.getGlobalBounds().width / 2 - mainMenu_buttons[j]->buttonText.getGlobalBounds().width / 2, mainMenu_buttons[j]->position.y + mainMenu_buttons[j]->buttonText.getGlobalBounds().height / 2));

						project_buttons[1]->buttonText.setString("Baterie");
						project_buttons[2]->buttonText.setString("Rezistor");
						project_buttons[3]->buttonText.setString("Bec");
						project_buttons[4]->buttonText.setString("Dioda");
						project_buttons[5]->buttonText.setString("Ampermetru");
						project_buttons[6]->buttonText.setString("Voltmetru");
						project_buttons[7]->buttonText.setString("Condensator");
						project_buttons[8]->buttonText.setString("Tranzistor");
						project_buttons[9]->buttonText.setString("Siguranta");
						project_buttons[10]->buttonText.setString("Inapoi");
						project_buttons[11]->buttonText.setString("Salveaza");
						project_buttons[12]->buttonText.setString("Salveaza ca");
						project_buttons[13]->buttonText.setString("Deschide");
						project_buttons[14]->buttonText.setString("Reseteaza");
						project_buttons[15]->buttonText.setString("Ajutor");
						project_buttons[16]->buttonText.setString("Anuleaza");
						project_buttons[17]->buttonText.setString("Refa");
						project_buttons[18]->buttonText.setString("Editeaza");
						project_buttons[19]->buttonText.setString("Sterge");
						for (int j = 1; j <= 19; j++)
							project_buttons[j]->buttonText.setPosition(Vector2f(project_buttons[j]->position.x + project_buttons[j]->buttonShape.getGlobalBounds().width / 2 - project_buttons[j]->buttonText.getGlobalBounds().width / 2, project_buttons[j]->position.y + project_buttons[j]->buttonText.getGlobalBounds().height / 2));
					}
					if (i == 6)
					{
						Lang = false;
						mainMenu_buttons[1]->buttonText.setString("New Project");
						mainMenu_buttons[2]->buttonText.setString("Open Project");
						mainMenu_buttons[3]->buttonText.setString("Info");
						mainMenu_buttons[4]->buttonText.setString("Exit");
						for (int j = 1; j <= 4; j++)
							mainMenu_buttons[j]->buttonText.setPosition(Vector2f(mainMenu_buttons[j]->position.x + mainMenu_buttons[j]->buttonShape.getGlobalBounds().width / 2 - mainMenu_buttons[j]->buttonText.getGlobalBounds().width / 2, mainMenu_buttons[j]->position.y + mainMenu_buttons[j]->buttonText.getGlobalBounds().height / 2));

						project_buttons[1]->buttonText.setString("Cell");
						project_buttons[2]->buttonText.setString("Resistor");
						project_buttons[3]->buttonText.setString("Bulb");
						project_buttons[4]->buttonText.setString("Diode");
						project_buttons[5]->buttonText.setString("Ammeter");
						project_buttons[6]->buttonText.setString("Voltmeter");
						project_buttons[7]->buttonText.setString("Capacitor");
						project_buttons[8]->buttonText.setString("Transistor");
						project_buttons[9]->buttonText.setString("Fuse");
						project_buttons[10]->buttonText.setString("Back");
						project_buttons[11]->buttonText.setString("Save");
						project_buttons[12]->buttonText.setString("Save as");
						project_buttons[13]->buttonText.setString("Open");
						project_buttons[14]->buttonText.setString("Reset");
						project_buttons[15]->buttonText.setString("Help");
						project_buttons[16]->buttonText.setString("Undo");
						project_buttons[17]->buttonText.setString("Redo");
						project_buttons[18]->buttonText.setString("Edit");
						project_buttons[19]->buttonText.setString("Delete");
						for (int j = 1; j <= 19; j++)
							project_buttons[j]->buttonText.setPosition(Vector2f(project_buttons[j]->position.x + project_buttons[j]->buttonShape.getGlobalBounds().width / 2 - project_buttons[j]->buttonText.getGlobalBounds().width / 2, project_buttons[j]->position.y + project_buttons[j]->buttonText.getGlobalBounds().height / 2));
					}
					mainMenu_buttons[i]->buttonState = Button::INACTIVE;
					break;
				}

			}
			break;
		case Project:
		{
			int ok = 1;
			for (int i = 1; i <= projectbuttonCount; i++) {
				if (ok == 0)
					break;
				switch (project_buttons[i]->buttonState) {
				case(Button::INACTIVE):
					project_buttons[i]->buttonShape.setFillColor(Color::Black);
					break;
				case(Button::HOVERED):
					project_buttons[i]->buttonShape.setFillColor(Color(100, 100, 100, 255));
					break;
				case(Button::CLICKED):
					if (i >= 1 && i <= 9) {
						project_buttons[i]->buttonState = Button::SELECTED;
						continue;
					}

					cout << i << " has been clicked." << endl;
					if (i == 10)
					{
						states = MainMenu;
						selectedComponentIndex = -1;
					}
					if (i == 11)
					{
						saveToFile();

					}
					if (i == 13)
					{
						loadFromFile();
					}

					if (i == 14)
					{
						resetProject();
						ok = 0;
					}
					//help button
					if(i==15)
					{
						helpWindow.create(VideoMode(500, 500), "Help", Style::Close);
						Text helpText;
						if(Lang==false)
						helpText.setString("Back     	  - return to menu page\nSave     	  - save changes made in the same project file\nOpen     	 - opens the saved project\nReset    	  - deletes every component on the screen\nComponent Button  - creates a circuit object of that type by \n		               clicking the button, then a place on screen\nUndo 		 - will negate the last operation made\nRedo 		  - will negate the last Undo operation made\nEdit 		    - activates the edit options for the component\n		            To edit parameters you need to press E while a \n		            component is selected\n		            To save a parameter you MUST press enter to save it.\nDelete 		- deletes a component\n\nA component is selected when the specific component gets \nclicked and the edit button is turned ON.\nTo make a conection between two components, click and hold \non a I/O point, drag and release on a O/I point.");
						if (Lang == true)
							helpText.setString("Inapoi     	  - te intorci in meniul principal\nSalveaza     	  - salveaza modificarile facute in proiect\nDeschide     	 - incarca proiectul salvat\nReseteaza    	  - sterge toate componentele de pe ecran\nButon componenta  - creaza un obiect circuit de acel tip apasand \n		               butonul, apoi apasand un loc pe ecran\nAnuleaza 		 - va nega ultima operatie facuta\nRefa 		  - va nega ultima operatie de tip Anuleaza\nEditeaza 		  - activeaza optiunile de editare pentru componenta\n		           Pentru a edita parametrii trebuie sa apasat E in timp ce \n		           o componenta este selectata\n		          Pentru a salva un parametru TREBUIE sa apasati enter.\nSterge 		- sterge o componenta\n\nO componenta este selectata cand acea componenta este \napasata si butonul edit arata ON.\nPentru a face o conexiune intre doua componente, apasa si tine \npe un punct de Intrare/Iesire , apoi trage si da drumul pe un punct\nde Iesire/Intrare.");
						helpText.setFont(font);
						helpText.setCharacterSize(100);
						helpText.setScale(Vector2f(.2, .2));
						helpText.setFillColor(Color::White);
						helpText.setPosition(10,10);
						while (helpWindow.isOpen())
						{
							helpWindow.clear(Color::Black);
							Event Eevent;
							while (helpWindow.pollEvent(Eevent))
							{
								switch (Eevent.type)
								{
								case Event::Closed:
									helpWindow.close();
								}
							}
							helpWindow.draw(helpText);
							helpWindow.display();
						}
					}
					if (i == 18)
					{
						canComponentsBeSelected = !canComponentsBeSelected;
						selectedComponentIndex = -1;
					}

					if (i == 19)
					{
						cout << selectedComponentIndex << '\n';
						deleteComponent(selectedComponentIndex);
						selectedComponentIndex = -1;
						selectedComponentBounds = FloatRect(0, 0, 0, 0);
					}
					project_buttons[i]->buttonState = Button::INACTIVE;
					break;
				case(Button::SELECTED):
					project_buttons[i]->buttonShape.setFillColor(Color::Magenta);
					break;
				}

			}

			for (int i = 1; i <= projectComponentCount; i++) {

				if (!canComponentsBeSelected)
					break;

				//drag-and-drop logic
				if (project_components[i]->isMouseOnComponent(window.mapPixelToCoords(myMouse.getPosition(window))) && event.type == Event::MouseButtonPressed && !isDraggingComponent) {
					isDraggingComponent = true;
					previousMouseCoords = window.mapPixelToCoords(myMouse.getPosition(window));
					draggedComponent = project_components[i];
					
				}
				if (isDraggingComponent) {
					auto currentMouseCoords = window.mapPixelToCoords(myMouse.getPosition(window));
					auto offset = Vector2f(currentMouseCoords.x - previousMouseCoords.x, currentMouseCoords.y - previousMouseCoords.y);
					selectedComponentBounds = draggedComponent->shape.getBounds();
					previousMouseCoords = currentMouseCoords;
					
					draggedComponent->circleShape.setPosition(draggedComponent->circleShape.getPosition().x + offset.x, draggedComponent->circleShape.getPosition().y + offset.y);

					draggedComponent->inputShape.setPosition(draggedComponent->inputShape.getPosition().x + offset.x, draggedComponent->inputShape.getPosition().y + offset.y);

					draggedComponent->outputShape.setPosition(draggedComponent->outputShape.getPosition().x + offset.x, draggedComponent->outputShape.getPosition().y + offset.y);
					draggedComponent->outputShape1.setPosition(draggedComponent->outputShape1.getPosition().x + offset.x, draggedComponent->outputShape1.getPosition().y + offset.y);

					draggedComponent->setPosition(offset);

				}
				if (event.type == Event::MouseButtonReleased && isDraggingComponent)
				{
					isDraggingComponent = false;
					draggedComponent = nullptr;
				}

				/*else if (isDraggingComponent && event.type == Event::MouseButtonReleased)
				{
					isDraggingComponent = false;
					cout << "dragging ended" << '\n';
					auto currentMouseCoords = window.mapPixelToCoords(myMouse.getPosition(window));
					auto offset = Vector2f(currentMouseCoords.x - previousMouseCoords.x, currentMouseCoords.y - previousMouseCoords.y);
					draggedComponent->setPosition(offset);
				}*/

			}

			for (int i = 1; i <= projectComponentCount; i++)
			{
				if (!canComponentsBeSelected)
					break;
				if (event.type == Event::MouseButtonPressed && project_components[i]->isMouseOnComponent(window.mapPixelToCoords(myMouse.getPosition(window)))) {
					selectedComponentIndex = i;
					selectedComponentBounds = project_components[i]->shape.getBounds();
					break;
				}
				else if (event.type == Event::MouseButtonPressed)
				{
					selectedComponentIndex = -1;
					selectedComponentBounds = FloatRect(0, 0, 0, 0);
				}
			}
			break;
		}
		case(Info):
				switch (BackButton->buttonState) 
				{
				case(Button::INACTIVE):
					BackButton->buttonShape.setFillColor(Color::Black);
					break;
				case(Button::HOVERED):
					BackButton->buttonShape.setFillColor(Color::Cyan);
					break;
				case(Button::CLICKED):
					states = MainMenu;
					break;
				}
			break;
		}
		//edit parameters window
		if (Keyboard::isKeyPressed(Keyboard::E) && selectedComponentIndex != -1)
		{
			edit.create(VideoMode(300, 300), "Edit", Style::Titlebar);
			Button button_edit_save(Vector2f(150, 280), Vector2f(50, 25), 0, Button::Null, "Save");
			Button* SaveButton = &button_edit_save;
			Textbox nume(15, Color::White, true);
			Textbox rezistenta(15, Color::White, false);
			Textbox voltaj(15, Color::White, false);
			if (project_components[selectedComponentIndex]->nume.tellp() != streampos(0))
				nume.text << project_components[selectedComponentIndex]->nume.str();
			if (project_components[selectedComponentIndex]->rezistenta.tellp() != streampos(0))
				rezistenta.text << project_components[selectedComponentIndex]->rezistenta.str();
			if (project_components[selectedComponentIndex]->voltaj.tellp() != streampos(0))
				voltaj.text << project_components[selectedComponentIndex]->voltaj.str();
			Text numet, rezistentat, voltajt;
			numet.setFont(font);
			numet.setCharacterSize(75);
			numet.setScale(Vector2f(.2, .2));
			numet.setString("Nume");
			numet.setFillColor(Color::White);
			numet.setPosition(80, 70);
			rezistentat.setFont(font);
			rezistentat.setCharacterSize(75);
			rezistentat.setScale(Vector2f(.2, .2));
			rezistentat.setString("Rezistenta");
			rezistentat.setFillColor(Color::White);
			rezistentat.setPosition(80, 120);
			voltajt.setFont(font);
			voltajt.setCharacterSize(75);
			voltajt.setScale(Vector2f(.2, .2));
			voltajt.setString("Voltaj");
			voltajt.setFillColor(Color::White);
			voltajt.setPosition(80, 170);
			nume.setFont(font);
			nume.setPosition(Vector2f(150, 72));
			nume.setLimit(true, 15);
			rezistenta.setFont(font);
			rezistenta.setPosition(Vector2f(150, 122));
			rezistenta.setLimit(true, 15);
			voltaj.setFont(font);
			voltaj.setPosition(Vector2f(150, 172));
			voltaj.setLimit(true, 15);
			RectangleShape Nume, Rezistenta, Voltaj;
			Nume.setFillColor(Color(0, 0, 0, 0));
			Rezistenta.setFillColor(Color(0, 0, 0, 0));
			Voltaj.setFillColor(Color(0, 0, 0, 0));
			Nume.setOutlineColor(Color::White);
			Rezistenta.setOutlineColor(Color::White);
			Voltaj.setOutlineColor(Color::White);
			Nume.setOutlineThickness(2);
			Rezistenta.setOutlineThickness(2);
			Voltaj.setOutlineThickness(2);
			Nume.setPosition(150, 75);
			Rezistenta.setPosition(150, 125);
			Voltaj.setPosition(150, 175);
			Nume.setSize(Vector2f(100, 15));
			Rezistenta.setSize(Vector2f(100, 15));
			Voltaj.setSize(Vector2f(100, 15));
			while (edit.isOpen())
			{

				edit.clear(Color::Black);
				Event Eevent;
				bool keyPressed = false;
				while (edit.pollEvent(Eevent))
				{
					switch (Eevent.type)
					{
					case Event::Closed:
						window.close();
					case Event::TextEntered:
						nume.typedOn(Eevent);
						rezistenta.typedOn(Eevent);
						voltaj.typedOn(Eevent);
					}

					auto mouseCoords = edit.mapPixelToCoords(myMouse.getPosition(edit));
					auto buttonBounds = SaveButton->buttonShape.getGlobalBounds();
					if (areCoordsInBounds(mouseCoords, buttonBounds))
						SaveButton->buttonState = (Eevent.type == Event::MouseButtonPressed) ? Button::CLICKED : Button::HOVERED;
					else
						SaveButton->buttonState = Button::INACTIVE;
					edit.setKeyRepeatEnabled(false);
					if (nume.isSelected == true && Keyboard::isKeyPressed(Keyboard::Return) && !keyPressed)
					{
						project_components[selectedComponentIndex]->nume.str("");
						keyPressed = true;
						project_components[selectedComponentIndex]->nume << nume.text.str();
						nume.isSelected = false;
						rezistenta.isSelected = true;
						voltaj.isSelected = false;
					}
					else
						if (rezistenta.isSelected == true && Keyboard::isKeyPressed(Keyboard::Return) && !keyPressed)
						{
							project_components[selectedComponentIndex]->rezistenta.str("");
							keyPressed = true;
							project_components[selectedComponentIndex]->rezistenta << rezistenta.text.str();
							nume.isSelected = false;
							rezistenta.isSelected = false;
							voltaj.isSelected = true;
						}
						else
							if (voltaj.isSelected == true && Keyboard::isKeyPressed(Keyboard::Return) && !keyPressed)
							{
								project_components[selectedComponentIndex]->voltaj.str("");
								keyPressed = true;
								project_components[selectedComponentIndex]->voltaj << voltaj.text.str();
								nume.isSelected = true;
								rezistenta.isSelected = false;
								voltaj.isSelected = false;
							}

					if (Eevent.type == Event::KeyReleased)
						keyPressed = false;

					switch (SaveButton->buttonState)
					{
					case(Button::INACTIVE):
						SaveButton->buttonShape.setFillColor(Color::Black);
						break;

					case(Button::HOVERED):
						SaveButton->buttonShape.setFillColor(Color::Cyan);
						break;

					case(Button::CLICKED):
						cout << "Save has been clicked." << endl;
						SaveButton->buttonState = Button::INACTIVE;
						edit.close();
						break;
					}
				}

				nume.textbox.setString(nume.text.str());
				edit.draw(nume.textbox);
				voltaj.textbox.setString(voltaj.text.str());
				edit.draw(voltaj.textbox);
				rezistenta.textbox.setString(rezistenta.text.str());
				edit.draw(rezistenta.textbox);
				edit.draw(Nume);
				edit.draw(Rezistenta);
				edit.draw(Voltaj);
				edit.draw(SaveButton->buttonShape);
				edit.draw(SaveButton->buttonText);
				edit.draw(numet);
				edit.draw(rezistentat);
				edit.draw(voltajt);
				edit.display();

			}
		}

		//draw all buttons
		
		if (states == MainMenu)
		{
			window.clear(Color::Black);
			for (int i = 1; i <= buttonCount; i++) {
				window.draw(mainMenu_buttons[i]->buttonShape);
				window.draw(mainMenu_buttons[i]->buttonText);
			}
			window.draw(roSprite);
			window.draw(enSprite);
			window.draw(backgroundText);
		}
		if (states == Info)
		{
			Texture texture;
			Sprite sprite;
			sprite.setScale(.35, .35);
			sprite.setPosition(130, 10);
			if(Lang==true)
				texture.loadFromFile("infoRo.png");
			if(Lang==false)
				texture.loadFromFile("infoEn.png");
			sprite.setTexture(texture, false);
			window.clear(Color::Black);
			window.draw(BackButton->buttonShape);
			window.draw(BackButton->buttonText);
			window.draw(sprite);
		}

		if (states == Project)
		{
			window.clear(Color::Black);
			window.draw(use_space);
			for (int i = 1; i <= projectbuttonCount; i++)
			{
				window.draw(project_buttons[i]->buttonShape);
				window.draw(project_buttons[i]->buttonText);
			}

			for (int i = 1; i <= projectComponentCount; i++)
			{
				if ((project_components[i]->type == ElectricalComponent::Bulb) || (project_components[i]->type == ElectricalComponent::Ammeter) || (project_components[i]->type == ElectricalComponent::Voltmeter) || (project_components[i]->type == ElectricalComponent::Transistor))
					window.draw(project_components[i]->circleShape);

				window.draw(project_components[i]->shape);
				window.draw(project_components[i]->inputShape);
				window.draw(project_components[i]->outputShape);

				if ((project_components[i]->type == ElectricalComponent::Transistor))
					window.draw(project_components[i]->outputShape1);

			}

			window.draw(edit_button_text);
			//rotationSlider.SliderLogic(window.mapPixelToCoords(myMouse.getPosition(window)), event);
			//rotationSlider.DrawSlider(window);

		}
	
		if (selectedComponentIndex != -1)
			drawLinesToBounds(selectedComponentBounds);
		for (auto x : line) {
			window.draw(x);
		}
		window.display();
	}

}