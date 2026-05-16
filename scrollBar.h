// thanks to github user called Jaclav and his "Fractals" project which i based this class on
// Fractals by Jaclav: 

#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <iostream>
#include <SFML/Graphics.hpp>

enum axis: bool {
	Horizontal, Vertical
};

class scrollBar {
public:
	scrollBar () {}
	scrollBar (sf::Vector2f position, axis bAxis, sf::Vector2f barSize, int pointRadius, sf::Color barColor, sf::Color pointColor) {
		bar.setPosition (position);
		barAxis = bAxis;
		bar.setSize (barSize);
		point.setRadius (pointRadius);
		bar.setFillColor (barColor);
		point.setFillColor (pointColor);
		
		if (barAxis == axis::Horizontal) point.setPosition (bar.getPosition().x, bar.getPosition().y + (bar.getSize().y / 2));
		else point.setPosition (bar.getPosition().x + (bar.getSize().x / 2), bar.getPosition().y);
	}
	
	void setBarOutlineThickness (int a) {
		bar.setOutlineThickness (a);
	}
	
	int getBarOutlineThickness () {
		return bar.getOutlineThickness();
	}
	
	void setPointOutlineThickness (int a) {
		point.setOutlineThickness (a);
	}
	
	int getPointOutlineThickness () {
	 	return point.getOutlineThickness();
	}
	
	void setBarOutlineColor (sf::Color color) {
		bar.setOutlineColor (color);
	}
	
	sf::Color getBarOutlineColor () {
		return bar.getOutlineColor();
	}
	
	void setPointOutlineColor (sf::Color color) {
		point.setOutlineColor (color);
	}
	
	sf::Color getPointOutlineColor () {
		return point.getOutlineColor();
	}
	
	sf::RectangleShape getBar () {
		return bar;
	}
	
	sf::CircleShape getPoint () {
		return point;
	}
	
	void setBarFillColor (sf::Color color) {
		bar.setFillColor (color);
	}
	
	sf::Color getBarFillColor () {
		return bar.getFillColor(); 
	}
	
	void setPointFillColor (sf::Color color) {
		point.setFillColor (color);
	}
	
	sf::Color getPointFillColor () {
		return point.getFillColor();
	}
	
	void setBarPosition (int x, int y) {
		x -= bar.getPosition().x;
		y -= bar.getPosition().y;
		
		bar.move (x, y);
		point.move (x, y);
	}
	
	sf::Vector2f getBarPosition () {
		return bar.getPosition ();
	}
	
	void setPointPosition (int x) {
		if (barAxis == axis::Horizontal) {
			if (x < bar.getPosition().x) x = bar.getPosition().x;
			if (x > bar.getPosition().x + bar.getSize().x) x = bar.getPosition().x + bar.getSize().x;
			
			point.setPosition (x, point.getPosition().y);
		}
		
		else {
			if (x < bar.getPosition().y) x = bar.getPosition().y;
			if (x > bar.getPosition().y + bar.getSize().y) x = bar.getPosition().y + bar.getSize().y;
			
			point.setPosition (point.getPosition().x, x);
		}
	}
	
	void setPointPosition (int x, int y) {
		point.setPosition (x, y);
	}
	
	int getPointSection (int sectionAmount) {
		int x;
		
		if (barAxis == axis::Horizontal) {
			x = point.getPosition().x + point.getRadius() - bar.getPosition().x;
			x /= (bar.getSize().x / sectionAmount);
		}
			
		else {
			x = point.getPosition().y - bar.getPosition().y;
			x /= (bar.getSize().y / sectionAmount);
		}
		
		return x;
	}
	
	sf::Vector2f getPointPosition () {
		return point.getPosition();
	}
	
	void movePoint (int x, int y) {
		x += point.getPosition().x;
		y += point.getPosition().y;
		point.setPosition (x, y);
	}
	
	void drawTo (sf::RenderWindow &window) {
		window.draw (bar);
		window.draw (point);
	}
	
	bool isMouseOver (sf::RenderWindow &window) {
		return (bar.getGlobalBounds().contains (sf::Vector2f (sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) || 
				point.getGlobalBounds().contains (sf::Vector2f (sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)));
	}
	
	void setBarSize (sf::Vector2f size) {
		bar.setSize (size);
	}
	
	sf::Vector2f getBarSize () {
		return bar.getSize ();
	}
	
	void setPointRadius (int radius) {
		point.setRadius (radius);
	}
	
	int getPointRadius () {
		return point.getRadius ();
	}
	
	void setBarAxis (axis Axis) {
		barAxis = Axis;
	}
	
	axis getBarAxis () {
		return barAxis;
	}
	
	void setPressed (bool pressed) {
		isPressed = pressed;
	}
	
	bool getPressed () {
		return isPressed;
	}
	
private:
	sf::RectangleShape bar;
	sf::CircleShape point;
	axis barAxis;
	bool isPressed;
};

#endif
