#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#define defaultSizeX 100
#define defaultSizeY 100
#define defaultRadius 2
#define DELETE_KEY 8
#define ENTER_KEY 13
#define ESCAPE_KEY 27
#define TAB_KEY 9
using namespace std;
using namespace sf;
extern Font font;
class Textbox : RectangleShape
{
public:
	ostringstream text;
	Text textbox;
	bool isSelected = false;
	Textbox(int size, Color color, bool sel)
	{
		textbox.setCharacterSize(size);
		textbox.setFillColor(color);
		isSelected = sel;
		if (sel)
		{
			textbox.setString("_");
		}
		else
		{
			textbox.setString("");
		}
	}

	void setFont(Font& font)
	{
		textbox.setFont(font);
	}

	void setPosition(Vector2f pos)
	{
		textbox.setPosition(pos);
	}

	void setLimit(bool ok)
	{
		hasLimit = ok;
	}

	void setLimit(bool ok, int lim)
	{
		hasLimit = ok;
		limit = lim - 1;
	}

	void setSelected(bool sel)
	{
		isSelected = sel;
		if (!sel)
		{
			string t = text.str();
			string newT = "";
			for (int i = 0; i < t.length(); i++)
			{
				newT += t[i];
			}
			textbox.setString(newT);
		}
	}

	string getText()
	{
		return text.str();
	}

	void drawTo(RenderWindow& window)
	{
		window.draw(textbox);
	}

	void typedOn(Event input)
	{
		if (isSelected)
		{
			int charTyped = input.text.unicode;
			if (charTyped < 128)
			{
				if (hasLimit)
				{
					if (text.str().length() <= limit)
					{
						inputLogic(charTyped);
					}
					else if (text.str().length() > limit && charTyped == DELETE_KEY)
					{
						deleteLastChar();
					}
				}
				else
				{
					inputLogic(charTyped);
				}
			}
		}
	}

	bool hasLimit = false;
	int limit;

	void inputLogic(int charTyped)
	{
		if (charTyped != DELETE_KEY && charTyped != ENTER_KEY && charTyped != ESCAPE_KEY)
		{
			text << static_cast<char>(charTyped);
		}
		else if (charTyped == DELETE_KEY)
		{
			if (text.str().length() > 0)
			{
				deleteLastChar();
			}
		}
	}

	void deleteLastChar()
	{
		string t = text.str();
		string newT = "";
		for (int i = 0; i < t.length() - 1; i++)
		{
			newT += t[i];
		}
		text.str("");
		text << newT;

		textbox.setString(newT);
	}
};

class Slider
{
public:
	float value = 1;
	float maxValue = 100;
	float minValue = 1;

	Vector2f previousMouseCoords;
	Vector2f currentMouseCoords;

	float minX, maxX;

	Vector2f position;
	Vector2f buttonPosition;

	bool isDraggingButton = false;


	RectangleShape sliderShape;
	CircleShape sliderButton;
	Text sliderText;

	Slider(Vector2f _position)
	{
		buttonPosition = Vector2f(_position.x - 5, _position.y - 5);
		position = _position;
		minX = _position.x - 5;
		maxX = _position.x + 94;
		sliderShape.setPosition(_position);
		sliderShape.setSize(Vector2f(99, 5));
		sliderShape.setFillColor(Color(155, 155, 155, 255));
		sliderButton.setPosition(buttonPosition + Vector2f(0, 5));
		sliderButton.setRadius(3);
		sliderButton.setFillColor(Color::Blue);

		sliderText.setFont(font);
		sliderText.setCharacterSize(60);
		sliderText.setScale(Vector2f(.2, .2));
		sliderText.setString("rotation 0");
		sliderText.setFillColor(Color::Black);
		sliderText.setPosition(_position + Vector2f(.1, -20));

	}

	void DrawSlider(RenderWindow& window) {
		window.draw(sliderShape);
		window.draw(sliderButton);
		window.draw(sliderText);

	}

	float computeValue(float value) {
		return value - minX + 1;
	}

	void SliderLogic(Vector2f mouseCoords, Event event) {
		if (isMouseOnButtonSlider(mouseCoords) && !isDraggingButton && event.type == Event::MouseButtonPressed)
		{
			isDraggingButton = true;
			previousMouseCoords = mouseCoords;
		}
		if (isDraggingButton)
		{
			string SliderText = "rotation: ";
			sliderText.setString(SliderText.append(to_string((int)computeValue(sliderButton.getPosition().x))));
			currentMouseCoords = mouseCoords;
			auto offset = Vector2f(currentMouseCoords.x - previousMouseCoords.x, currentMouseCoords.y - previousMouseCoords.y);
			previousMouseCoords = currentMouseCoords;
			sliderButton.setPosition(sliderButton.getPosition().x + offset.x, sliderButton.getPosition().y);
		}
		if (event.type == Event::MouseButtonReleased)
		{
			isDraggingButton = false;
		}

		if (sliderButton.getPosition().x < minX)
			sliderButton.setPosition(minX, sliderButton.getPosition().y);

		if (sliderButton.getPosition().x > maxX)
			sliderButton.setPosition(maxX, sliderButton.getPosition().y);
	}

	bool isMouseOnButtonSlider(Vector2f mouseCoords) {
		auto shapeBounds = sliderButton.getGlobalBounds();
		if (mouseCoords.x > shapeBounds.left && mouseCoords.x < shapeBounds.left + shapeBounds.width && mouseCoords.y > shapeBounds.top && mouseCoords.y < shapeBounds.top + shapeBounds.height) {
			return true;
		}
		return false;
	}
};

class Components
{
public:
	Components() = default;

	pair<VertexArray, pair<CircleShape, CircleShape>> ComputeDiodeDraw(Vector2f position, Vector2f size, float rotation) {
		//file = fopen("Components\\Diode.txt", "r");
		ifstream fin("Components\\Diode.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 12);
		for (int i = 0; i <= 5; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
			case 'L':
				int values[10] = { 0 };
				char* p = strtok(readedLine, " ");
				int j = 0;

				while (p != nullptr)
				{
					values[j++] = atoi(p);
					p = strtok(NULL, " ");
				}

				shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
				shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
				shape[i * 2].color = Color::Black;
				shape[i * 2 + 1].color = Color::Black;
				break;
			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		fin.close();
		return make_pair(shape, make_pair(inputShape, outputShape));
	}
	pair<VertexArray, pair<CircleShape, CircleShape>> ComputeCapacitatorDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Capacitator.txt");
		VertexArray shape(sf::Lines, 8);
		char readedLine[256] = "undefined";

		for (int i = 0; i <= 3; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
			case 'L':
				int values[10] = { 0 };
				char* p = strtok(readedLine, " ");
				int j = 0;

				while (p != nullptr)
				{
					values[j++] = atoi(p);
					p = strtok(NULL, " ");
				}

				shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
				shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
				shape[i * 2].color = Color::Black;
				shape[i * 2 + 1].color = Color::Black;
				break;
			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		fin.close();
		return make_pair(shape, make_pair(inputShape, outputShape));
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> ComputeFuseDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Fuse.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 10);
		for (int i = 0; i <= 4; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
			case 'L':
				int values[10] = { 0 };
				char* p = strtok(readedLine, " ");
				int j = 0;

				while (p != nullptr)
				{
					values[j++] = atoi(p);
					p = strtok(NULL, " ");
				}

				shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
				shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
				shape[i * 2].color = Color::Black;
				shape[i * 2 + 1].color = Color::Black;
				break;

			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		fin.close();
		return make_pair(shape, make_pair(inputShape, outputShape));
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>>  ComputeBulbDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Bulb.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 8);
		CircleShape circleShape;
		for (int i = 0; i <= 4; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
				case 'L':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
					shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
					shape[i * 2].color = Color::Black;
					shape[i * 2 + 1].color = Color::Black;
				}
				break;
				case 'C':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					circleShape = CircleShape(values[1]);
					circleShape.setPosition(Vector2f(position.x + values[2], position.y + values[3]));
					circleShape.setFillColor(Color::White);
					circleShape.setOutlineColor(Color::Black);
					circleShape.setOutlineThickness(.7);

				}
			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);
		fin.close();
		return make_pair(shape, make_pair(circleShape, make_pair(inputShape, outputShape)));
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> ComputeCellDraw(Vector2f position, Vector2f size, float rotation) {
		ifstream fin("Components\\ElectricalCell.txt");


		char readedLine[256] = "undefined";
		VertexArray shape(sf::Lines, 8);
		for (int i = 0; i <= 3; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
			case 'L':
				int values[10] = { 0 };
				char* p = strtok(readedLine, " ");
				int j = 0;


				while (p != nullptr)
				{
					values[j++] = atoi(p);
					p = strtok(NULL, " ");
				}


				shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
				shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
				shape[i * 2].color = Color::Black;
				shape[i * 2 + 1].color = Color::Black;
				break;
			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		fin.close();
		return make_pair(shape, make_pair(inputShape, outputShape));

	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>>  ComputeAmmeterDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Ammeter.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 10);
		CircleShape circleShape;
		for (int i = 0; i <= 5; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
				case 'L':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
					shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
					shape[i * 2].color = Color::Black;
					shape[i * 2 + 1].color = Color::Black;
				}
				break;
				case 'C':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;


					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					circleShape = CircleShape(values[1]);
					circleShape.setPosition(Vector2f(position.x + values[2], position.y + values[3]));
					circleShape.setFillColor(Color::White);
					circleShape.setOutlineColor(Color::Black);
					circleShape.setOutlineThickness(.7);

				}

			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);
		fin.close();
		return make_pair(shape, make_pair(circleShape, make_pair(inputShape, outputShape)));
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>>  ComputeVoltmeterDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\\Voltmeter.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 8);
		CircleShape circleShape;
		for (int i = 0; i <= 4; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
				case 'L':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
					shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
					shape[i * 2].color = Color::Black;
					shape[i * 2 + 1].color = Color::Black;
				}
				break;
				case 'C':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					circleShape = CircleShape(values[1]);
					circleShape.setPosition(Vector2f(position.x + values[2], position.y + values[3]));
					circleShape.setFillColor(Color::White);
					circleShape.setOutlineColor(Color::Black);
					circleShape.setOutlineThickness(.7);

				}

			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);
		fin.close();
		return make_pair(shape, make_pair(circleShape, make_pair(inputShape, outputShape)));
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, pair<CircleShape, CircleShape>>>>  ComputeTransistorDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Transistor.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 12);
		CircleShape circleShape;
		for (int i = 0; i <= 6; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
				case 'L':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
					shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
					shape[i * 2].color = Color::Black;
					shape[i * 2 + 1].color = Color::Black;
				}
				break;
				case 'C':
				{
					int values[10] = { 0 };
					char* p = strtok(readedLine, " ");
					int j = 0;

					while (p != nullptr)
					{
						values[j++] = atoi(p);
						p = strtok(NULL, " ");
					}

					circleShape = CircleShape(values[1]);
					circleShape.setPosition(Vector2f(position.x + values[2], position.y + values[3]));
					circleShape.setFillColor(Color::White);
					circleShape.setOutlineColor(Color::Black);
					circleShape.setOutlineThickness(.7);

				}

			}

		}
		int inputX, inputY;
		int outputX, outputY;
		int outputX1, outputY1;

		fin >> inputX >> inputY >> outputX >> outputY >> outputX1 >> outputY1;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);
		CircleShape outputShape1 = CircleShape(defaultRadius);
		outputShape1.setPosition(outputX1, outputY1);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		outputShape1.setPosition(Vector2f(position.x + outputX1, position.y + outputY1));
		outputShape1.setFillColor(Color::Transparent);
		outputShape1.setOutlineColor(Color::Black);
		outputShape1.setOutlineThickness(.5);
		fin.close();
		return make_pair(shape, make_pair(circleShape, make_pair(inputShape, make_pair(outputShape, outputShape1))));
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> ComputeResistorDraw(Vector2f position, Vector2f size, float rotation)
	{
		ifstream fin("Components\\Resistor.txt");
		char readedLine[256] = "undefined";
		VertexArray shape(Lines, 12);
		for (int i = 0; i <= 5; i++)
		{
			fin.getline(readedLine, 256);
			switch (readedLine[0])
			{
			case 'L':
				int values[10] = { 0 };
				char* p = strtok(readedLine, " ");
				int j = 0;


				while (p != nullptr)
				{
					values[j++] = atoi(p);
					p = strtok(NULL, " ");
				}


				shape[i * 2].position = Vector2f(position.x + values[1], position.y + values[2]);
				shape[i * 2 + 1].position = Vector2f(position.x + values[3], position.y + values[4]);
				shape[i * 2].color = Color::Black;
				shape[i * 2 + 1].color = Color::Black;
				break;


			}

		}
		int inputX, inputY;
		int outputX, outputY;

		fin >> inputX >> inputY >> outputX >> outputY;
		CircleShape inputShape = CircleShape(defaultRadius);
		inputShape.setPosition(inputX, inputY);
		CircleShape outputShape = CircleShape(defaultRadius);
		outputShape.setPosition(outputX, outputY);

		inputShape.setPosition(Vector2f(position.x + inputX, position.y + inputY));
		inputShape.setFillColor(Color::Transparent);
		inputShape.setOutlineColor(Color::Black);
		inputShape.setOutlineThickness(.5);

		outputShape.setPosition(Vector2f(position.x + outputX, position.y + outputY));
		outputShape.setFillColor(Color::Transparent);
		outputShape.setOutlineColor(Color::Black);
		outputShape.setOutlineThickness(.5);

		fin.close();
		return make_pair(shape, make_pair(inputShape, outputShape));
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> SaveDiodeDraw(Vector2f position, Vector2f size, float angleRotation, VertexArray& shape)
	{
		return ComputeDiodeDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> SaveCellDraw(Vector2f position, Vector2f size, float angleRotation, VertexArray& shape)
	{
		return ComputeCellDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> SaveCapacitatorDraw(Vector2f position, Vector2f size, float angleRotation, VertexArray& shape)
	{
		return ComputeCapacitatorDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> SaveFuseDraw(Vector2f position, Vector2f size, float angleRotation, VertexArray& shape)
	{
		return ComputeFuseDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, CircleShape>> SaveResistorDraw(Vector2f position, Vector2f size, float angleRotation, VertexArray& shape)
	{
		return ComputeResistorDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> SaveBulbDraw(Vector2f position, Vector2f size, float angleRotation)
	{
		return ComputeBulbDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> SaveAmmeterDraw(Vector2f position, Vector2f size, float angleRotation)
	{
		return ComputeAmmeterDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> SaveVoltmeterDraw(Vector2f position, Vector2f size, float angleRotation)
	{
		return ComputeVoltmeterDraw(position, size, angleRotation);
	}

	pair<VertexArray, pair<CircleShape, pair<CircleShape, pair<CircleShape, CircleShape>>>> SaveTransistorDraw(Vector2f position, Vector2f size, float angleRotation)
	{
		return ComputeTransistorDraw(position, size, angleRotation);
	}
};

class ElectricalComponent : Components
{

public:
	VertexArray shape;
	CircleShape circleShape;

	CircleShape inputShape, outputShape, outputShape1;
	ostringstream nume, voltaj, rezistenta;
	Vector2f position;
	Vector2f size;
	enum Type {
		Null = 0,
		Cell = 1,
		Resistor = 2,
		Bulb = 3,
		Ammeter = 4,
		Voltmeter = 5,
		Capacitor = 6,
		Transistor = 7,
		Fuse = 8,
		Diode = 9

	}type;
	Sprite componentSprite;
	float rotation = 0;

	ElectricalComponent(Type _type, Vector2f _position, Vector2f _size, float _rotation) {
		type = _type;
		position = _position;
		size = _size;
		rotation = _rotation;

		//LoadSprite();
		LoadComponentShape();
	}

	void LoadComponentShape() {
		switch (type) {
		case Diode:
		{
			pair<VertexArray, pair<CircleShape, CircleShape>> var = SaveDiodeDraw(position, size, rotation, shape);
			shape = var.first;
			inputShape = var.second.first;
			outputShape = var.second.second;
		}
		break;
		case Transistor:
		{

			pair<VertexArray, pair<CircleShape, pair<CircleShape, pair<CircleShape, CircleShape>>>> var = ComputeTransistorDraw(position, size, rotation);
			shape = var.first;
			circleShape = var.second.first;
			inputShape = var.second.second.first;
			outputShape = var.second.second.second.first;
			outputShape1 = var.second.second.second.second;
		}
		break;
		case Capacitor:
		{
			pair<VertexArray, pair<CircleShape, CircleShape>> var = SaveCapacitatorDraw(position, size, rotation, shape);
			shape = var.first;
			inputShape = var.second.first;
			outputShape = var.second.second;
		}
		break;
		case Ammeter:
		{
			pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> var = ComputeAmmeterDraw(position, size, rotation);
			shape = var.first;
			circleShape = var.second.first;
			inputShape = var.second.second.first;
			outputShape = var.second.second.second;
		}
		break;
		case Bulb:
		{
			pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> var = ComputeBulbDraw(position, size, rotation);
			shape = var.first;
			circleShape = var.second.first;
			inputShape = var.second.second.first;
			outputShape = var.second.second.second;
		}

		break;
		case Cell:
		{
			pair<VertexArray, pair<CircleShape, CircleShape>> var = SaveCellDraw(position, size, rotation, shape);
			shape = var.first;
			inputShape = var.second.first;
			outputShape = var.second.second;
		}
		break;
		case Voltmeter:
		{
			pair<VertexArray, pair<CircleShape, pair<CircleShape, CircleShape>>> var = ComputeVoltmeterDraw(position, size, rotation);
			shape = var.first;
			circleShape = var.second.first;
			inputShape = var.second.second.first;
			outputShape = var.second.second.second;
		}
		break;
		case Resistor:
		{
			pair<VertexArray, pair<CircleShape, CircleShape>> var = SaveResistorDraw(position, size, rotation, shape);
			shape = var.first;
			inputShape = var.second.first;
			outputShape = var.second.second;
		}
		break;


		case Fuse:
		{
			pair<VertexArray, pair<CircleShape, CircleShape>> var = SaveFuseDraw(position, size, rotation, shape);
			shape = var.first;
			inputShape = var.second.first;
			outputShape = var.second.second;
		}
		break;

		}
	}

	bool isMouseOnComponent(Vector2f mouseCoords)
	{
		auto shapeBounds = shape.getBounds();
		if (mouseCoords.x > shapeBounds.left && mouseCoords.x < shapeBounds.left + shapeBounds.width && mouseCoords.y > shapeBounds.top && mouseCoords.y < shapeBounds.top + shapeBounds.height) {
			return true;
		}
		return false;
	}

	void setPosition(Vector2f offset) {
		for (int i = 0; i < shape.getVertexCount(); i++)
		{
			shape[i].position = Vector2f(shape[i].position.x + offset.x, shape[i].position.y + offset.y);

		}
		position = position + offset;
	}

};

class Button
{
public:
	enum State {
		INACTIVE,
		HOVERED,
		CLICKED,
		SELECTED,
	}buttonState;

	enum Type {
		Null = 0,
		Cell = 1,
		Resistor = 2,
		Bulb = 3,
		Ammeter = 4,
		Voltmeter = 5,
		Capacitor = 6,
		Transistor = 7,
		Fuse = 8,
		Diode = 9

	}typeToCast;
	RectangleShape buttonShape;
	Text buttonText;
	Vector2f position;
	Vector2f size;

	float rotation = 0;

	Button(Vector2f _position, Vector2f _size, float _rotation, Type _typeToCast, string _buttonText) {
		position = Vector2f(_position.x - _size.x / 2, _position.y - _size.y / 2);
		size = _size;
		rotation = _rotation;
		typeToCast = _typeToCast;
		buttonText.setString(_buttonText);

		buttonState = INACTIVE;

		DrawButton();
	}

	void DrawButton() {
		buttonShape.setSize(size);
		buttonShape.setOutlineColor(Color(100, 100, 100, 255));
		buttonShape.setOutlineThickness(1);
		buttonShape.setPosition(position);

		buttonText.setFont(font);
		buttonText.setCharacterSize(120);
		buttonText.setScale(Vector2f(.13, .13));
		buttonText.setFillColor(Color::White);

		//centreaza textu
		buttonText.setPosition(Vector2f(position.x + buttonShape.getGlobalBounds().width / 2 - buttonText.getGlobalBounds().width / 2, position.y + buttonText.getGlobalBounds().height / 2));
	}

	void InstantiateComponent(Type type, ElectricalComponent* components[], int& electricalCount, Vector2f mouseCoords, Vector2f defaultSize) {

		switch (type) {

		case Diode:
		{

			ElectricalComponent* diode = new ElectricalComponent(ElectricalComponent::Diode, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = diode;
		}
		break;
		case Fuse:
		{
			ElectricalComponent* fuse = new ElectricalComponent(ElectricalComponent::Fuse, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = fuse;

		}
		break;
		case Voltmeter:
		{
			ElectricalComponent* voltmeter = new ElectricalComponent(ElectricalComponent::Voltmeter, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = voltmeter;

		}
		break;
		case Ammeter:
		{
			ElectricalComponent* ammeter = new ElectricalComponent(ElectricalComponent::Ammeter, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = ammeter;
		}
		break;

		case Capacitor:
		{
			ElectricalComponent* capacitator = new ElectricalComponent(ElectricalComponent::Capacitor, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = capacitator;
		}
		break;
		case Transistor:
		{
			ElectricalComponent* transistor = new ElectricalComponent(ElectricalComponent::Transistor, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = transistor;
		}
		break;
		case Bulb:
		{
			ElectricalComponent* bulb = new ElectricalComponent(ElectricalComponent::Bulb, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = bulb;
		}
		break;
		case Cell:
		{
			ElectricalComponent* cell = new ElectricalComponent(ElectricalComponent::Cell, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = cell;
		}
		break;
		case Resistor:
		{
			ElectricalComponent* resistor = new ElectricalComponent(ElectricalComponent::Resistor, mouseCoords, Vector2f(defaultSize.x, defaultSize.y), 0);
			components[++electricalCount] = resistor;
		}
		break;
		}
	}
};