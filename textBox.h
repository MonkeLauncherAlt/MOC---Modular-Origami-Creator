#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cstdlib>
#include <iostream> 
#include <sstream>

#define DELETE_KEY 8
#define ENTER_KEY 13
#define ESCAPE_KEY 27

class textBox {
public:
	textBox () {
		textbox.setColor (sf::Color (255, 255, 255));
	}
	
	textBox (int size, sf::Color color, bool sel) {
		textbox.setCharacterSize (size);
		textbox.setColor (color);
		isSelected = sel;
		
		if (sel) {
			textbox.setString ("_");
		}
		
		else {
			textbox.setString ("");
		}
	}
	
	void setFont (sf::Font &font) {
		textbox.setFont (font);
	}
	
	void setPosition (sf::Vector2f pos) {
		position = pos;
		textbox.setPosition (position);
	}
	
	void setLimit (bool isLim) {
		hasLimit = isLim;
	}
	
	void setLimit (int lim) {
		hasLimit = true;
		limit = lim - 1;
	}
	
	void setSelected (bool isSel) {
		if (!isSel) {
			textbox.setString (text.str());
		} 
		
		else {
			textbox.setString (text.str() + '_');
		}
		
		isSelected = isSel;
	}
	
	std::string getText () {
		return text.str();
	}
	
	void drawTo (sf::RenderWindow &okno) {
		okno.draw (textbox);
	}
	
	void typing (sf::Event input) {
		if (isSelected) {
			int charTyped = input.text.unicode;
			
			if (charTyped < 128) {
				if (hasLimit) {
					if (text.str().size() <= limit) {
						inputLogic (charTyped);
					}
					
					else if (charTyped == DELETE_KEY) {
						deleteLastChar ();
					}
				}
				
				else {
					inputLogic (charTyped);
				}
			}
		}
	}
	
	void move (int x, int y) {
		position.x += x;
		position.y += y;
		textbox.setPosition (position);
	}
	
	void setCharacterSize (int size) {
		textbox.setCharacterSize (size);
	}
	
	void setColor (sf::Color color) {
		textbox.setColor (color);
	}
	
	bool getSelected () {
		return isSelected;
	}
	
private:
	sf::Text textbox;
	std::ostringstream text;
	bool isSelected = false;
	bool hasLimit = false;
	int limit;
	sf::Vector2f position;
	
	void inputLogic (int charTyped) {
		if (charTyped == DELETE_KEY) {
			if (text.str().length() > 0) {
				deleteLastChar ();
			}
		}
		
		else if (charTyped != ENTER_KEY && charTyped != ESCAPE_KEY) {
			text << static_cast <char> (charTyped);
		}
		
		textbox.setString (text.str() + "_");
	}
	
	deleteLastChar () {
		std::string t = text.str();
		std::string newT = "";
		
		for (int i = 0; i < t.size() - 1; i++) {
			newT += t[i];
		}
		
		text.str("");
		text << newT;
		
		textbox.setString (text.str());
	}
};




#endif
