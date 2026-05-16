#ifndef MOUSEKEYBOARDSHORTS_H
#define MOUSEKEYBOARDSHORTS_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cstdlib>
#include <iostream> 
#include <sstream>


bool KPR (int a) {
	return sf::Keyboard::isKeyPressed (static_cast <sf::Keyboard::Key> (a));
}

bool MPR (int a) {
	return sf::Mouse::isButtonPressed (static_cast <sf::Mouse::Button> (a));
}

sf::Vector2i MPos () {
	return sf::Mouse::getPosition ();
}

sf::Vector2i MPos (sf::RenderWindow &window) {
	return sf::Mouse::getPosition (window);
}

bool KBumped (int a) {
	static bool isPressed [128];
	bool returned = false;
	
	if (KPR (a)) {
		if (!isPressed[a]) returned = true;
		isPressed[a] = true;
	}
	
	else {
		isPressed[a] = false;
	}
	
	return returned;
}

bool MBumped (int a) {
	static bool isPressed [8];
	bool returned = false;
	
	if (MPR (a)) {
		if (!isPressed[a]) returned = true;
		isPressed[a] = true;
	}
	
	else {
		isPressed[a] = false;
	}
	
	return returned;
}


#endif 
