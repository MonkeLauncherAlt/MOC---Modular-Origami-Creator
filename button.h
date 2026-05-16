#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>
#include <SFML/Graphics.hpp>


class Button {
public:
	Button () {}
	Button (std::string t, sf::Vector2f size, sf::Color bgColor, sf::Color textColor, int charSize) {
		text.setString (t);
		button.setSize (size);
		button.setFillColor (bgColor);
		text.setColor (textColor);
		text.setCharacterSize (charSize);
	}
	
	void setOutlineThickness (int a) {
		button.setOutlineThickness (a);
	}
	
	void setOutlineColor (sf::Color a) {
		button.setOutlineColor (a);
	}
	
	void setFont (sf::Font &font) {
		text.setFont (font);
	}
	
	void setPressed (bool pressed, int sizeChangeX, int sizeChangeY) {		
		if (pressed != isPressed) {			
			button.setSize({button.getSize().x + sizeChangeX, button.getSize().y + sizeChangeY});
			text.setCharacterSize (text.getCharacterSize() + sizeChangeY);
			button.move (-sizeChangeX / 2, -sizeChangeY / 2);
			text.move (-sizeChangeX / 2, -sizeChangeY / 2);
		}
		
		isPressed = pressed;
	}
	
	bool isButtonPressed () {
		return isPressed;
	}
	
	sf::RectangleShape getRectangleShape () {
		return button;
	}
	
	void setBackgroundColor (sf::Color color) {
		button.setFillColor (color);
	}
	
	void setTextColor (sf::Color color) {
		text.setColor (color);
	}
	
	void setPosition (int x, int y) {
		button.setPosition (x, y);
		text.setPosition ((button.getSize().x - text.getGlobalBounds().width) / 2 + x, (button.getSize().y - text.getCharacterSize()) / 2 + y);
	}
	
	void move (int x, int y) {
		x += button.getPosition().x;
		y += button.getPosition().y;
		
		button.setPosition (x, y);
		text.setPosition ((button.getSize().x - text.getGlobalBounds().width) / 2 + x, (button.getSize().y - text.getGlobalBounds().height) / 2 + y);
	}
	
	void drawTo (sf::RenderWindow &window) {
		window.draw (button);
		window.draw (text);
	}
	
	bool isMouseOver (sf::RenderWindow &window) {
		return (button.getPosition().x - button.getOutlineThickness() <= sf::Mouse::getPosition(window).x && sf::Mouse::getPosition(window).x <= button.getPosition().x + button.getSize().x + button.getOutlineThickness() && 
				button.getPosition().y - button.getOutlineThickness() <= sf::Mouse::getPosition(window).y && sf::Mouse::getPosition(window).y <= button.getPosition().y + button.getSize().y + button.getOutlineThickness());
	}
	
	void setString (std::string a) {
		text.setString (a);
	}
	
	void setSize (sf::Vector2f size) {
		button.setSize(size);
	}
	
	void setCharacterSize (int a) {
		text.setCharacterSize (a);
	}
	
	void setTextPosition (int x, int y) {
		text.setPosition (x, y);
	}
	
	void moveText (int x, int y) {
		x += text.getPosition().x;
		y += text.getPosition().y;
		text.setPosition (x, y);
	}
	
	int getOutlineThickness () {
		return button.getOutlineThickness ();
	}
	
	sf::Color getOutlineColor () {
		return button.getOutlineColor ();
	}
	
	sf::Color getBackgroundColor () {
		return button.getFillColor ();
	}
	
	sf::Color getTextColor () {
		return text.getColor ();
	}
	
	sf::Vector2i getPosition () {
		sf::Vector2i v (button.getPosition().x, button.getPosition().y);
		return v;
	}
	
	sf::FloatRect getTextGlobalBounds () {
		return text.getGlobalBounds ();
	}
	
private:
	sf::RectangleShape button;
	sf::Text text;
	bool isPressed;

};

#endif
