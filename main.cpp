#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <algorithm>

#include "mouseKeyboardShorts.h"
#include "button.h"
#include "textBox.h"
#include "scrollBar.h"

using namespace sf;
using namespace std;


// apparently I have to keep this shit, because pair<int, int> is sorted in a bamboozled way
struct myPair {
	int first, second;
	
	myPair (int a, int b)
	: first(a), second(b) {
	}
};


// ---=== All variables ===---

VideoMode mode = VideoMode::getDesktopMode ();
RenderWindow okno (mode, "Modular Origami Creator MOC alpha_01", Style::Fullscreen);
Event event;

Sprite piece, screenshot, colorPiece = piece;
Texture pieceT [4], screenshotT;
RectangleShape gridCell, redCell, screenShade, sideBar, bottomBar, popupWindow, textSpace, RGBTextSpace[3], pieceListBg;
Shader guassianBlur, rainbow;
Font Helvetica, Ebrima;
Text popupWindowText;
Vector2i baseOffset = {200, 0}, mouseOffset;

Button optionBtns [8], colorDropdownBtn;
vector <Button> colorBtns;
vector <Color> colors;
vector <string> colorNames;
vector <myPair> colorCount;
Button newColorBtn;
scrollBar colorScrollBar;

textBox fileTextBox, colorTextBox, RGBTextBox[3];

short widthS, widthE = 42, heightS, heightE = 30;
short pieceWidth = 90, pieceHeight = 250, pieceXSpace = 90, pieceYSpace = 60, pickedColor = 0;
float pieceScale = 0.5;
bool drawGrid, updateWindow = false, clearWasChanged = false, paused = false, fileNamePopupWindow = false, colorPopupWindow = false, updateList = false;
string filename = "saves/save1.txt";
string optionNames [8] = {"reverse", "delete", "point", "brush", "load new", "save", "clear board", "close"};
int prevColorOffset = 0, colorOffset;

// this here only uses 4,768 MB of memory
bool isPlaced [1000][1000];
bool isFront [1000][1000];
bool hasShade [1000][1000];
short gridColor [1000][1000];
bool wasChanged [1000][1000]; // for reversing set-selected areas




// ---=== all function prototypes ===---

void drawAll ();
void reacting ();
void preprocessing ();
void drawBlur ();
void popupText (string a);
void fileLoading ();
void fileSaving ();
void loadProperties ();
void saveProperties ();
void shadeUpdate ();
void resizeLimits ();
void updatingList ();

Vector2i mouseCell ();
int stringToInt (string a);
bool sortColorsFunc (myPair a, myPair b);



// ---=== int main ===---
int main () {
	
	// preprocessing funcs
	loadProperties ();
	preprocessing ();
	
		
	
	// programe loop 
	while (okno.isOpen ()) {
		
		
		// event loop 
		while (okno.pollEvent (event)) {
			
			if (event.type == event.Closed || optionBtns[7].isButtonPressed()) {
				fileSaving ();
				saveProperties();
				okno.close();
			}
			
			if (KBumped (Keyboard::Escape)) {
				if (fileNamePopupWindow) fileNamePopupWindow = false;
				else if (colorPopupWindow) colorPopupWindow = false;
				else {
					fileSaving ();
					saveProperties();
					okno.close();
				}
				updateWindow = true;
			}
			
			reacting ();
			
			if (drawGrid && !paused) updateWindow = true;
		}
		
		
		// --== all the drawing ==--
		if (updateWindow) {
			okno.clear (Color (30, 30, 30));
			drawAll ();
			okno.display ();
		}
		
		
		updateWindow = false;
	}
}


// ---=== all functions ===---


void drawAll () {
	
	// --== drawing grid ==--
	if (drawGrid) {
		for (int i = heightS; i < heightE; i++) {
			for (int j = widthS; j < widthE; j++) {
				gridCell.setPosition (j * pieceXSpace * pieceScale + baseOffset.x, i * pieceYSpace * pieceScale + 95 * pieceScale * 2 + baseOffset.y);
				if (i % 2 == 1) gridCell.move (pieceXSpace * pieceScale / 2, 0);
				okno.draw (gridCell);
			}
		}
	}
			
	
	
	// --== drawing all the pieces on the board ==--
	for (int i = heightE - 1; i >= heightS; i--) {
		for (int j = widthS; j < widthE; j++) {
			if (isPlaced[j][i]) {
				piece.setPosition (j * pieceXSpace * pieceScale + baseOffset.x, i * pieceYSpace * pieceScale + baseOffset.y);
				if (i % 2 == 1) piece.move (pieceXSpace * pieceScale / 2, 0);
				
				short pieceI = 0;
				
				if (!isFront[j][i]) pieceI += 2;
				if (!hasShade[j][i]) pieceI++;
				
				piece.setTexture (pieceT[pieceI]);
				piece.setColor (colors [gridColor[j][i]]);
				
				okno.draw (piece);
			}
		}
	}
	
	
	
	// --== drawing red cell if grid ==--
	if (drawGrid) {
		Vector2i drawCell = mouseCell();
		
		if (drawCell.x < 0) drawCell.x = 0;
		if (drawCell.y < 0) drawCell.y = 0;
		
		drawCell.x = pieceXSpace * pieceScale * drawCell.x + baseOffset.x;
		drawCell.y = pieceYSpace * pieceScale * drawCell.y + baseOffset.y;
		
		if (mouseCell().y % 2 == 1) drawCell.x += (int)pieceXSpace * pieceScale / 2;
		drawCell.y = drawCell.y + (pieceHeight * pieceScale) - (pieceYSpace * pieceScale);
		
		redCell.setPosition (drawCell.x, drawCell.y);
		okno.draw (redCell);
	}
	
	
	// --== drawing piece counter boxes ==--
	colorDropdownBtn.drawTo (okno);
	updatingList();
	
	if (colorDropdownBtn.isButtonPressed()) {
		okno.draw (pieceListBg);
	}
	
	
	// --== drawing all the sidebar parts ==--
	okno.draw (sideBar);
	
	for (int i = 0; i < 8; i++) {
		optionBtns[i].drawTo (okno); 
	}
	
	
	// --== drawing all the bottom bar parts ==--	
	okno.draw (bottomBar);
	
	for (int i = 0; i < colors.size(); i++) {
		colorBtns[i].setTextPosition ((90 - colorBtns[i].getTextGlobalBounds().width) / 2 + colorBtns[i].getPosition().x, okno.getSize().y - 190);
		colorBtns[i].drawTo (okno);
		
		colorPiece.setColor (colors[i]);
		
		colorPiece.setTexture (pieceT[1]);
		colorPiece.setPosition (colorBtns[i].getPosition().x + 12, okno.getSize().y - 155 + 12);
		okno.draw (colorPiece);
		
		colorPiece.setTexture (pieceT[3]);
		colorPiece.setPosition (colorBtns[i].getPosition().x + 90 - colorPiece.getGlobalBounds().width - 12, okno.getSize().y - 155 + 12);
		okno.draw (colorPiece);
	}
	
	newColorBtn.setCharacterSize (20);
	newColorBtn.setPosition (20 + colorOffset, newColorBtn.getPosition().y); 
	newColorBtn.setTextPosition ((90 - newColorBtn.getTextGlobalBounds().width) / 2 + 20 + colorOffset, okno.getSize().y - 190);  
	newColorBtn.drawTo (okno);
	
	colorPiece.setTexture (pieceT[1]);
	colorPiece.setPosition (20 + 12 + colorOffset, okno.getSize().y - 155 + 12);
	okno.draw (colorPiece, &rainbow);
	
	colorPiece.setTexture (pieceT[3]);
	colorPiece.setPosition (110 - colorPiece.getGlobalBounds().width - 12 + colorOffset, okno.getSize().y - 155 + 12);
	okno.draw (colorPiece, &rainbow);
	
	
	colorScrollBar.drawTo(okno); 
	
	
	
	
	// --== drawing the popup window ==--
	if (fileNamePopupWindow || colorPopupWindow) {
		drawBlur();
		okno.draw (popupWindow);
		
		// when loading file
		if (fileNamePopupWindow) {
			textSpace.setSize (Vector2f (600, 60));
			textSpace.setPosition (popupWindow.getPosition().x + 20, popupWindow.getPosition().y + 100);
			okno.draw (textSpace);
		
			fileTextBox.setPosition ({popupWindow.getPosition().x + 20 + 10, popupWindow.getPosition().y + 100 + 10});
			fileTextBox.drawTo(okno);
			
			popupWindowText.setString ("Type in save file name.");
		}
		
		// when making new color
		else if (colorPopupWindow) {
			textSpace.setSize (Vector2f (600, 50));
			textSpace.setPosition (popupWindow.getPosition().x + 20, popupWindow.getPosition().y + 75);
			okno.draw (textSpace);
			
			colorTextBox.setPosition ({popupWindow.getPosition().x + 20 + 10, popupWindow.getPosition().y + 75 + 10});
			colorTextBox.drawTo (okno);
			
			for (int i = 0; i < 3; i++) {
				okno.draw (RGBTextSpace[i]);
				RGBTextBox[i].drawTo (okno);
			}
			
			popupWindowText.setString ("Type in new color name and RGB values");
		}
		
		okno.draw (popupWindowText);
	}
}


void fileLoading () {
	okno.clear (Color (30, 30, 30));
	
	drawAll ();
	drawBlur ();
	popupText ("Loading...");
	
	okno.display ();
	
	struct stat buffer;   
  	if (!(stat (filename.c_str(), &buffer) == 0)) {
  		ofstream file (filename);
	
		for (int i = 0; i < 1000; i++) {
			for (int j = 0; j < 1000; j++) {
				file << "0 1 1 0\n";
			}
		}
		
		file.close();
	}
	
	
	ifstream file (filename);
	
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			file >> isPlaced[j][i] >> isFront[j][i] >> hasShade[j][i] >> gridColor[j][i];
		}
	}
	
	file.close();
	
	updateList = true;
}


void fileSaving () {
	okno.clear (Color (30, 30, 30));
	
	drawAll ();
	drawBlur ();
	popupText ("Saving...");
	
	okno.display ();
	
	ofstream file (filename);
	
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			file << isPlaced[j][i] << " " << isFront[j][i] << " " << hasShade[j][i] << " " << gridColor[j][i] << "\n";
		}
	}
	
	file.close();
}


void reacting () {
	
	// ---=== paused screen ===---
	if (KBumped (Keyboard::Space) && !fileNamePopupWindow && !colorPopupWindow) {
		paused = !paused;
		
		if (paused) {
			okno.clear (Color (30, 30, 30));
			drawAll ();
			drawBlur ();
			popupText ("Paused");
			
			okno.display ();
		}
		
		else updateWindow = true;
	}
	
	
	// ---=== popup window screen ===---
	if (fileNamePopupWindow) {
		if (MBumped (Mouse::Left)) {
			if (popupWindow.getPosition().x <= MPos(okno).x && MPos(okno).x <= popupWindow.getPosition().x + popupWindow.getSize().x && 
				popupWindow.getPosition().y <= MPos(okno).y && MPos(okno).y <= popupWindow.getPosition().y + popupWindow.getSize().y) {
				
				if (textSpace.getPosition().x <= MPos(okno).x && MPos(okno).x <= textSpace.getPosition().x + textSpace.getSize().x &&
					textSpace.getPosition().y <= MPos(okno).y && MPos(okno).y <= textSpace.getPosition().y + textSpace.getSize().y) {
					fileTextBox.setSelected (true);
					textSpace.setFillColor (Color (25, 25, 25));
				}
				
				else {
					fileTextBox.setSelected (false);
					textSpace.setFillColor (Color (20, 20, 20));
				}
			}
			
			else {
				fileNamePopupWindow = false;
			}
			
			updateWindow = true;
		}
		
		if (KBumped (Keyboard::Return)) {
			if (fileTextBox.getText().size() > 0 && fileTextBox.getSelected()) {
				fileNamePopupWindow = false;
				filename = "saves/" + fileTextBox.getText() + ".txt";
				fileLoading ();
				
				updateWindow = true;
			}
		}
		
		else if (event.type == Event::TextEntered) {
			fileTextBox.typing (event);
			updateWindow = true;
		}
	}
	
	
	
	// --== color popup window screen ==--
	if (colorPopupWindow) {
		if (MBumped (Mouse::Left)) {
			if (popupWindow.getPosition().x <= MPos(okno).x && MPos(okno).x <= popupWindow.getPosition().x + popupWindow.getSize().x && 
				popupWindow.getPosition().y <= MPos(okno).y && MPos(okno).y <= popupWindow.getPosition().y + popupWindow.getSize().y) {
				
				// checking name field
				if (textSpace.getPosition().x <= MPos(okno).x && MPos(okno).x <= textSpace.getPosition().x + textSpace.getSize().x &&
					textSpace.getPosition().y <= MPos(okno).y && MPos(okno).y <= textSpace.getPosition().y + textSpace.getSize().y) {
					colorTextBox.setSelected (true);
					textSpace.setFillColor (Color (25, 25, 25));
				}
				
				else {
					colorTextBox.setSelected (false);
					textSpace.setFillColor (Color (20, 20, 20));
				}
				
				
				// checking RGB fields 
				for (int i = 0; i < 3; i++) {
					if (RGBTextSpace[i].getPosition().x <= MPos(okno).x && MPos(okno).x <= RGBTextSpace[i].getPosition().x + RGBTextSpace[i].getSize().x &&
						RGBTextSpace[i].getPosition().y <= MPos(okno).y && MPos(okno).y <= RGBTextSpace[i].getPosition().y + RGBTextSpace[i].getSize().y) {
						RGBTextBox[i].setSelected (true);
						RGBTextSpace[i].setFillColor (Color (25, 25, 25));
					}
					
					else {
						RGBTextBox[i].setSelected (false);
						RGBTextSpace[i].setFillColor (Color (20, 20, 20));
					}
				}
			}
			
			else {
				colorPopupWindow = false;
			}
			
			updateWindow = true;
		}
		
		else if (KBumped (Keyboard::Return)) {
			if (colorTextBox.getText().size() > 0 && (colorTextBox.getSelected() || RGBTextBox[0].getSelected() || RGBTextBox[1].getSelected() || RGBTextBox[2].getSelected())) {
				colorPopupWindow = false;
				
				int red, green, blue;
				red = stringToInt (RGBTextBox[0].getText());
				green = stringToInt (RGBTextBox[1].getText());
				blue = stringToInt (RGBTextBox[2].getText());
				
				colors.push_back (Color (red, green, blue));
				colorNames.push_back (colorTextBox.getText());
				colorBtns.push_back (Button ());
				
				int s = colorBtns.size() - 1;
				colorBtns[s].setFont (Helvetica);
				colorBtns[s].setString (colorNames[s]);
				colorBtns[s].setSize ({90, 90});
				colorBtns[s].setBackgroundColor (Color (35, 35, 35));
				colorBtns[s].setCharacterSize (20);
				colorBtns[s].setTextColor (Color (220, 220, 220));
				colorBtns[s].setPosition ((90 + 20) * (s + 2) - 90, okno.getSize().y - 155);
				colorBtns[s].setOutlineColor (Color (50, 50, 50));
				colorBtns[s].setOutlineThickness (1);
				colorBtns[s].moveText (0, -70);
				colorBtns[s].setPressed (false, 0, 0);
			}
			
			updateWindow = true;
		}
		
		else if (KPR (Keyboard::Up)) {
			for (int i = 0; i < 3; i++) {
				if (RGBTextBox[i].getSelected()) {
					RGBTextBox[i].setSelected (false); 
					RGBTextSpace[i].setFillColor (Color (20, 20, 20));
					
					colorTextBox.setSelected (true);
					textSpace.setFillColor (Color (25, 25, 25));
					
					updateWindow = true;
					break;
				}
			}
		}
		
		else if (KPR (Keyboard::Down)) {
			if (colorTextBox.getSelected()) {
				colorTextBox.setSelected (false);
				textSpace.setFillColor (Color (20, 20, 20));
				
				RGBTextBox[0].setSelected (true);
				RGBTextSpace[0].setFillColor (Color (25, 25, 25));
				
				updateWindow = true;
			}
		}
		
		else if (KPR (Keyboard::Left)) {
			for (int i = 1; i < 3; i++) {
				if (RGBTextBox[i].getSelected()) {
					RGBTextBox[i].setSelected (false); 
					RGBTextSpace[i].setFillColor (Color (20, 20, 20));
					
					RGBTextBox[i - 1].setSelected (true);
					RGBTextSpace[i - 1].setFillColor (Color (25, 25, 25));
					
					updateWindow = true;
					break;
				}
			}
		}
		
		else if (KPR (Keyboard::Right)) {
			for (int i = 0; i < 2; i++) {
				if (RGBTextBox[i].getSelected()) {
					RGBTextBox[i].setSelected (false); 
					RGBTextSpace[i].setFillColor (Color (20, 20, 20));
					
					RGBTextBox[i + 1].setSelected (true);
					RGBTextSpace[i + 1].setFillColor (Color (25, 25, 25));
					
					updateWindow = true;
					break;
				}
			}
		}
		
		else if (event.type == Event::TextEntered) {
			colorTextBox.typing (event);
			RGBTextBox[0].typing (event);
			RGBTextBox[1].typing (event);
			RGBTextBox[2].typing (event);
			
			updateWindow = true;
		}
	}
	
	
	
	// ---=== normal screen ===---
	if (!paused && !fileNamePopupWindow && !colorPopupWindow) {
		
		if (clearWasChanged && !(MPR (Mouse::Left))) {
			clearWasChanged = false;
			
			for (int i = 0; i < 1000; i++) {
				for (int j = 0; j < 1000; j++) {
					wasChanged[j][i] = false;
				}
			}
		}
		
		if (KBumped (Keyboard::G)) {
			drawGrid = !(drawGrid);
			updateWindow = true;
		}
		
		
		
		// ---=== bottom bar ===---
		if (okno.getSize().y - bottomBar.getSize().y <= MPos(okno).y && MPos(okno).y <= okno.getSize().y) {
			
			int a = colors.size() + 1 - 16;
			if (a < 1) a = 1;
			colorOffset = colorScrollBar.getPointSection (a);
			if (colorOffset > a - 1) colorOffset = a - 1;
			colorOffset *= -110;
			
			if (colorOffset != prevColorOffset) {
				prevColorOffset = colorOffset;
				
				for (int i = 0; i < colors.size(); i++) {
					colorBtns[i].setPosition ((90 + 20) * (i + 2) - 90 + colorOffset, colorBtns[i].getPosition().y);
				}
			}
			
			// color buttons
			for (int i = 0; i < colors.size(); i++) {				
				if (colorBtns[i].isMouseOver(okno) && MBumped (Mouse::Left) && !(colorBtns[i].isButtonPressed())) {
					
					colorBtns[i].setPressed (true, 0, 0);
					colorBtns[pickedColor].setPressed (false, 0, 0);
					pickedColor = i;
				}
				
				if (colorBtns[i].isButtonPressed()) {
					colorBtns[i].setBackgroundColor (Color (120, 120, 120));
					colorBtns[i].setOutlineColor (Color (30, 30, 30));
				}
				
				else {
					if (!(colorBtns[i].isMouseOver(okno))) {
						colorBtns[i].setBackgroundColor (Color (35, 35, 35));
						colorBtns[i].setOutlineColor (Color (50, 50, 50)); 
					}
					
					else {					
						colorBtns[i].setBackgroundColor (Color (45, 45, 45));
						colorBtns[i].setOutlineColor (Color (50, 50, 50)); 
					}
				}
			}
			
			
			
			
			// new color button
			if (newColorBtn.isMouseOver(okno) && MBumped (Mouse::Left)) {
				colorPopupWindow = true;
			}
			
			
			// bottom scroll bar
			if (colorScrollBar.isMouseOver(okno)) {
				colorScrollBar.setPointFillColor(Color (70, 70, 70));
				
				if (MBumped (Mouse::Left)) colorScrollBar.setPressed (true);
			}
			
			else colorScrollBar.setPointFillColor(Color (50, 50, 50));
			
			if (colorScrollBar.getPressed() && !(MPR (Mouse::Left))) colorScrollBar.setPressed (false);
			if (colorScrollBar.getPressed()) colorScrollBar.setPointPosition (MPos(okno).x - colorScrollBar.getPointRadius());
			
			
			
			updateWindow = true;
		}
		
		
		
		// ---=== side bar ===---
		else if (0 <= MPos(okno).x && MPos(okno).x <= sideBar.getSize().x) {
			for (int i = 0; i < 8; i++) {
				if (optionBtns[i].isMouseOver(okno) && MBumped (Mouse::Left)) {
					if (optionBtns[i].isButtonPressed()) optionBtns[i].setPressed (false, 12, 4);
					else optionBtns[i].setPressed (true, -12, -4);
					
					
					// switching buttons
					switch (i) {
						case 0:
							optionBtns[1].setPressed (false, 12, 4);
							break;
						
						case 1:
							optionBtns[0].setPressed (false, 12, 4);
							break;
						
						case 2:
							optionBtns[3].setPressed (false, 12, 4);
							break;
						
						case 3:
							optionBtns[2].setPressed (false, 12, 4);
							break;
						
						case 4: {
							fileSaving ();
							optionBtns[4].setPressed (false, 12, 4);
							
							fileNamePopupWindow = true;
							break;
						}
						
						case 5: {
							fileSaving ();
							optionBtns[5].setPressed (false, 12, 4);
							break;
						}
						
						case 6: {
							for (int i = 0; i < 1000; i++) {
								for (int j = 0; j < 1000; j++) {
									isPlaced[j][i] = false;
								}
							}
							
							updateList = true;
							optionBtns[6].setPressed (false, 12, 4);
							
							break;
						}
						
					}
				}
				
				if (optionBtns[i].isButtonPressed()) {
					optionBtns[i].setBackgroundColor (Color (220, 220, 220));
					optionBtns[i].setTextColor (Color (35, 35, 35));
					optionBtns[i].setOutlineColor (Color (30, 30, 30));
				}
				
				else {
					if (!(optionBtns[i].isMouseOver(okno))) {
						optionBtns[i].setBackgroundColor (Color (35, 35, 35));
						optionBtns[i].setTextColor (Color (220, 220, 220));
						optionBtns[i].setOutlineColor (Color (50, 50, 50)); 
					}
					
					else {					
						optionBtns[i].setBackgroundColor (Color (45, 45, 45));
						optionBtns[i].setTextColor (Color (220, 220, 220));
						optionBtns[i].setOutlineColor (Color (50, 50, 50)); 
					}
				}
			}
			
			
			updateWindow = true;
		}
		
		
		
		// --== piece counter dropdown ==--
		else if (colorDropdownBtn.isMouseOver (okno)) {
			
			if (!colorDropdownBtn.isButtonPressed()) {
				colorDropdownBtn.setBackgroundColor (Color (50, 50, 50));
				colorDropdownBtn.setOutlineColor (Color (70, 70, 70)); 	
			}
			
			if (MBumped (Mouse::Left)) {
				if (!colorDropdownBtn.isButtonPressed()) {
					colorDropdownBtn.setPressed (true, 0, 0);
					colorDropdownBtn.setBackgroundColor(Color (80, 80, 80));
					colorDropdownBtn.setBackgroundColor(Color (100, 100, 100));
				}
				
				else {
					colorDropdownBtn.setPressed (false, 0, 0);
					colorDropdownBtn.setBackgroundColor(Color (50, 50, 50));
					colorDropdownBtn.setBackgroundColor(Color (70, 70, 70)); 
				}
			}
			
			updateWindow = true;
		}
		
		// for no actions while mouse if over the piece list
		else if (pieceListBg.getGlobalBounds().contains (MPos(okno).x, MPos(okno).y) && colorDropdownBtn.isButtonPressed()) {
			;
		}
		
		
		
		// --== board ==--
		else {
			
			if (colorDropdownBtn.getBackgroundColor() == Color (50, 50, 50) && !colorDropdownBtn.isButtonPressed()) {
				colorDropdownBtn.setBackgroundColor (Color (40, 40, 40));
				colorDropdownBtn.setOutlineColor (Color (60, 60, 60));
				updateWindow = true;
			}
			
			
			if ((optionBtns[2].isButtonPressed() && MBumped (Mouse::Left)) || (optionBtns[3].isButtonPressed() && MPR (Mouse::Left))) {
				Vector2i cellCords = mouseCell ();
				
				if (0 <= cellCords.x && cellCords.x < 1000 && 0 <= cellCords.y && cellCords.y < 1000) {
						if (isPlaced[cellCords.x][cellCords.y]) {
						
						if (optionBtns[0].isButtonPressed()) {
							clearWasChanged = true;
							
							if (!(wasChanged[cellCords.x][cellCords.y])) {
								isFront[cellCords.x][cellCords.y] = !(isFront[cellCords.x][cellCords.y]);
								wasChanged[cellCords.x][cellCords.y] = true;
							}
						}
						
						else if (optionBtns[1].isButtonPressed()) {
							isPlaced[cellCords.x][cellCords.y] = false;
							shadeUpdate();
						}
					}
					
					if (!(optionBtns[0].isButtonPressed()) && !(optionBtns[1].isButtonPressed())) {
						isPlaced [cellCords.x][cellCords.y] = true;
						isFront[cellCords.x][cellCords.y] = true;
						gridColor[cellCords.x][cellCords.y] = pickedColor;
						shadeUpdate();
					}
					
					updateWindow = true;
					updateList = true;
				}
				
			}
			
			
			
			// moving the board
			if (MBumped (Mouse::Right)) {
				mouseOffset = {MPos(okno).x, MPos(okno).y};
			}
			
			else if (MPR (Mouse::Right)) {
				baseOffset.x += MPos(okno).x - mouseOffset.x;
				baseOffset.y += MPos(okno).y - mouseOffset.y;
				
				mouseOffset = {MPos(okno).x, MPos(okno).y};
				
				resizeLimits ();
				
				updateWindow = true;
			}
			
			
			
			// setting the scale (and also changing the offset to keep the position)
			if (event.type == Event::MouseWheelScrolled) {
				Vector2i currCell = mouseCell();
				
				if (event.mouseWheelScroll.delta < 0 && pieceScale > 0.125) {
					pieceScale /= 2;
					
					baseOffset.x += currCell.x * pieceXSpace * pieceScale;
					baseOffset.y += currCell.y * pieceYSpace * pieceScale;
				}
				
				else if (event.mouseWheelScroll.delta > 0 && pieceScale < 1) {
					pieceScale *= 2;
					
					baseOffset.x -= currCell.x * pieceXSpace * pieceScale / 2;
					baseOffset.y -= currCell.y * pieceYSpace * pieceScale / 2;
				}
				
				piece.setScale (pieceScale, pieceScale);
				redCell.setScale (pieceScale * 2, pieceScale * 2);
				gridCell.setScale (pieceScale * 2, pieceScale * 2);
				
				resizeLimits ();
				
				updateWindow = true;
			}
		}
	}
	
		
	
}


void preprocessing () {
	
	okno.setFramerateLimit (60);
	okno.setVerticalSyncEnabled (true);
	
	Helvetica.loadFromFile ("HelveticaRoundedLTStd-Bd.otf");
	Ebrima.loadFromFile ("ebrima.ttf");
	
	pieceT[0].loadFromFile ("fold1.png");
	pieceT[1].loadFromFile ("fold2.png");
	pieceT[2].loadFromFile ("fold3.png");
	pieceT[3].loadFromFile ("fold4.png");
	
	piece.setTexture (pieceT[1]);
	piece.setScale (Vector2f (pieceScale, pieceScale));
	
	gridCell.setSize (Vector2f ((pieceXSpace * pieceScale) - 4, (pieceYSpace * pieceScale) - 4));
	gridCell.setOutlineThickness (4);
	gridCell.setFillColor (Color::Transparent);
	gridCell.setOutlineColor (Color (50, 50, 50));
	gridCell.setOrigin (-2, -2);
	
	redCell.setFillColor (Color::Transparent);
	redCell.setOutlineColor (Color::Red);
	redCell.setSize (Vector2f ((pieceXSpace * pieceScale) - 4, (pieceYSpace * pieceScale) - 4));
	redCell.setOutlineThickness (2);
	redCell.setOrigin (-2, -2);
	
	screenShade.setSize (Vector2f (okno.getSize().x, okno.getSize().y));
	screenShade.setFillColor (Color (20, 20, 20, 180));
	screenShade.setPosition (0, 0);
	
	sideBar.setPosition (0, 0);
	sideBar.setSize (Vector2f (200, okno.getSize().y));
	sideBar.setFillColor (Color (25, 25, 25));
	sideBar.setOutlineThickness (1);
	sideBar.setOutlineColor (Color (40, 40, 40));
	
	bottomBar.setSize (Vector2f (okno.getSize().x, 200));
	bottomBar.setPosition (0, okno.getSize().y - 200);
	bottomBar.setFillColor (Color (25, 25, 25));
	bottomBar.setOutlineThickness (1);
	bottomBar.setOutlineColor (Color (40, 40, 40));
	
	popupWindow.setSize (Vector2f (640, 200));
	popupWindow.setPosition ((okno.getSize().x - 640) / 2, (okno.getSize().y - 200) / 2);
	popupWindow.setFillColor (Color (25, 25, 25));
	popupWindow.setOutlineThickness (1);
	popupWindow.setOutlineColor (Color (40, 40, 40));
	
	textSpace.setFillColor (Color (20, 20, 20));
	
	for (int i = 0; i < 3; i++) {
		RGBTextSpace[i].setFillColor (Color (20, 20, 20));
		RGBTextSpace[i].setSize (Vector2f (180, 50));
		RGBTextSpace[i].setPosition (popupWindow.getPosition().x + 20 + (i * 210), popupWindow.getPosition().y + 135);
		
		RGBTextBox[i].setPosition ({popupWindow.getPosition().x + 20 + (i * 210) + 10, popupWindow.getPosition().y + 135 + 10});
		RGBTextBox[i].setFont (Ebrima);
		RGBTextBox[i].setCharacterSize (30);
		RGBTextBox[i].setColor (Color (220, 220, 220));
		RGBTextBox[i].setLimit (3);
	}
	
	popupWindowText.setFont (Ebrima);
	popupWindowText.setString ("Type in save file name.");
	popupWindowText.setPosition (popupWindow.getPosition().x + 20, popupWindow.getPosition().y + 20);
	popupWindowText.setColor (Color (220, 220, 220));
	popupWindowText.setCharacterSize (30);
	
	for (int i = 0; i < 8; i++) {
		optionBtns[i].setFont (Helvetica);
		optionBtns[i].setString (optionNames[i]);
		optionBtns[i].setSize ({150, 50});
		optionBtns[i].setBackgroundColor (Color (35, 35, 35));
		optionBtns[i].setCharacterSize (20);
		optionBtns[i].setTextColor (Color (220, 220, 220));
		optionBtns[i].setPosition (25, (50 + 30) * (i + 1) - 50);
		optionBtns[i].setOutlineColor (Color (50, 50, 50));
		optionBtns[i].setOutlineThickness (1);
	}
	
	for (int i = 0; i < colors.size(); i++) {
		colorBtns[i].setFont (Helvetica);
		colorBtns[i].setString (colorNames[i]);
		colorBtns[i].setSize ({90, 90});
		colorBtns[i].setBackgroundColor (Color (35, 35, 35));
		colorBtns[i].setCharacterSize (20);
		colorBtns[i].setTextColor (Color (220, 220, 220));
		colorBtns[i].setPosition ((90 + 20) * (i + 2) - 90, okno.getSize().y - 155);
		colorBtns[i].setOutlineColor (Color (50, 50, 50));
		colorBtns[i].setOutlineThickness (1);
		colorBtns[i].moveText (0, -70);
		colorBtns[i].setPressed (false, 0, 0);
	}
	
	newColorBtn.setFont (Helvetica);
	newColorBtn.setString ("New Color");
	newColorBtn.setSize ({90, 90});
	newColorBtn.setBackgroundColor (Color (35, 35, 35));
	newColorBtn.setCharacterSize (20);
	newColorBtn.setTextColor (Color (220, 220, 220));
	newColorBtn.setPosition (20, okno.getSize().y - 155);
	newColorBtn.setOutlineColor (Color (50, 50, 50));
	newColorBtn.setOutlineThickness (1);
	newColorBtn.moveText (0, -70);
	newColorBtn.setPressed (false, 0, 0);
	
	colorDropdownBtn.setFont (Helvetica);
	colorDropdownBtn.setString ("Piece List");
	colorDropdownBtn.setSize ({150, 50});
	colorDropdownBtn.setBackgroundColor (Color (40, 40, 40));
	colorDropdownBtn.setCharacterSize (20);
	colorDropdownBtn.setTextColor (Color (220, 220, 220));
	colorDropdownBtn.setPosition (okno.getSize().x - colorDropdownBtn.getRectangleShape().getSize().x - 30, 30);
	colorDropdownBtn.setOutlineColor (Color (60, 60, 60));
	colorDropdownBtn.setOutlineThickness (1);
	colorDropdownBtn.setPressed (false, 0, 0);
	
	pieceListBg.setSize (Vector2f (300, 100));
	pieceListBg.setPosition (Vector2f (colorDropdownBtn.getPosition().x + colorDropdownBtn.getRectangleShape().getSize().x - pieceListBg.getSize().x, colorDropdownBtn.getPosition().y + colorDropdownBtn.getRectangleShape().getSize().y));
	pieceListBg.setFillColor (Color (100, 100, 100));
	pieceListBg.setOutlineThickness (1);
	pieceListBg.setOutlineColor (Color (100, 100, 100));
	
	for (int i = 2; i < 4; i++) optionBtns[i].move (0, 25);
	for (int i = 4; i < 6; i++) optionBtns[i].move (0, 50);
	for (int i = 6; i < 8; i++) optionBtns[i].move (0, 75);
	
	for (int i = 0; i < 8; i++) {
		optionBtns[i].moveText (0, -3);
	}
	
	colorBtns[0].setPressed (true, 0, 0);
	colorBtns[0].setBackgroundColor (Color (120, 120, 120));
	colorBtns[0].setOutlineColor (Color (30, 30, 30));
	
	colorPiece = piece;
	colorPiece.setScale (0.26, 0.26);
	
	fileTextBox.setFont (Ebrima);
	fileTextBox.setCharacterSize (40);
	fileTextBox.setColor (Color (220, 220, 220));
	fileTextBox.setLimit (true);
	fileTextBox.setLimit (17);
	
	colorTextBox.setFont (Ebrima);
	colorTextBox.setCharacterSize (30);
	colorTextBox.setColor (Color (220, 220, 220));
	colorTextBox.setLimit (true);
	colorTextBox.setLimit (16);
	
	for (int i = 0; i < 3; i++) {
		colorTextBox.setFont (Ebrima);
		colorTextBox.setCharacterSize (30);
		colorTextBox.setColor (Color (220, 220, 220));
		colorTextBox.setLimit (16);
	}
	
	colorScrollBar.setBarAxis (axis::Horizontal);
	colorScrollBar.setPointOutlineThickness (1);
	colorScrollBar.setPointOutlineColor (Color (70, 70, 70));
	colorScrollBar.setBarFillColor (Color (10, 10, 10));
	colorScrollBar.setPointFillColor (Color (50, 50, 50));
	colorScrollBar.setBarSize ({okno.getSize().x - 100, 15});
	colorScrollBar.setPointRadius (15);
	colorScrollBar.setBarPosition (50, okno.getSize().y - 32);
	colorScrollBar.setPressed (false);
	colorScrollBar.setPointPosition (colorScrollBar.getBarPosition().x -(colorScrollBar.getPointRadius()), 
									 colorScrollBar.getBarPosition().y + colorScrollBar.getBarSize().y / 2 - colorScrollBar.getPointRadius());
	
	
	
	// the first frame
	
	if (!Shader::isAvailable()) okno.close();
	
	rainbow.loadFromFile ("rainbow.frag", sf::Shader::Fragment);
	
	guassianBlur.loadFromFile("GuassianBlur.frag", sf::Shader::Fragment);
	guassianBlur.setParameter ("offsetFactor", 0.0013, 0.0013);
	
	fileLoading ();
	
	updateWindow = true;
}


Vector2i mouseCell () {
	short xCell, yCell, xMove = 0;
	
	yCell = (MPos(okno).y - baseOffset.y - (pieceHeight * pieceScale) + (pieceYSpace * pieceScale)) / (pieceYSpace * pieceScale);
	if (yCell % 2 == 1) xMove = pieceXSpace * pieceScale / 2;
	xCell = (MPos(okno).x - baseOffset.x - xMove) / (pieceXSpace * pieceScale);
	
	Vector2i cords (xCell, yCell);
	return cords;
}


void drawBlur () {
	screenshotT.create (okno.getSize().x, okno.getSize().y);
	screenshotT.update (okno);
	screenshot.setTexture (screenshotT);
	screenshot.setPosition (0, 0);
	okno.draw (screenshot, &guassianBlur);
	okno.draw (screenShade);
}


void popupText (string a) {
	Text popup;
	
	popup.setFont (Helvetica);
	popup.setString (a);
	popup.setCharacterSize (100);
	popup.setFillColor (Color (220, 220, 220));
	popup.setPosition ((okno.getSize().x - popup.getGlobalBounds().width) / 2, (okno.getSize().y - popup.getGlobalBounds().height) / 2);
	
	okno.draw (popup);
}


void loadProperties () {
	ifstream file ("properties.txt");
	int n, r, g, b;
	string a;
	
	file >> filename;
	getline (file, filename);
	
	file >> a;
	file >> n;
	
	for (int i = 0; i < n; i++) {
		getline (file, a);
		getline (file, a);
		file >> r >> g >> b;
		
		colorNames.push_back (a);
		colors.push_back (Color (r, g, b));
		
		colorBtns.push_back (Button ());
	}
	
	file.close();
}


void saveProperties () {
	ofstream file ("properties.txt");
	
	file << "filename= " << filename << "\n";
	file << "Colors= " << colors.size() << "\n";
	
	for (int i = 0; i < colors.size(); i++) {
		file << colorNames[i] << "\n" << (int)colors[i].r << " " << (int)colors[i].g << " " << (int)colors[i].b << "\n";
	}
	
	file.close();
}


void shadeUpdate () {
	for (int i = heightS; i < heightE; i++) {
		if (i == 0) continue;
		
		for (int j = widthS; j < widthE; j++) {
			if (i % 2 == 0) {
				if (j == 0) continue;
				
				if (isPlaced[j][i - 1] && isPlaced[j - 1][i - 1]) hasShade[j][i] = true;
				else hasShade[j][i] = false;
			}
			
			else {
				if (j == 999) continue;
				
				if (isPlaced[j][i - 1] && isPlaced[j + 1][i - 1]) hasShade[j][i] = true;
				else hasShade[j][i] = false;
			}
		}
	}
}


void resizeLimits () {
	if (baseOffset.x > 200) baseOffset.x = 200;
	if (baseOffset.x < -(1000 * pieceXSpace * pieceScale - 200)) baseOffset.x = -(1000 * pieceXSpace * pieceScale - 200);
	if (baseOffset.y > 0) baseOffset.y = 0;
	if (baseOffset.y < -(1000 * pieceYSpace * pieceScale - 95)) baseOffset.y = -(1000 * pieceYSpace * pieceScale - 95);
	
	widthS = ((-baseOffset.x + 200) / (pieceXSpace * pieceScale)) - 1;
	widthE = ((-baseOffset.x + 200 + okno.getSize().x) / (pieceXSpace * pieceScale)) + 1;
	heightS = ((-baseOffset.y) / (pieceYSpace * pieceScale)) - (3 / pieceScale);
	heightE = ((-baseOffset.y + okno.getSize().y) / (pieceYSpace * pieceScale)) + 1;
	
	if (widthS > widthE) widthS = widthE;
	if (heightS > heightE) heightS = heightE;
	if (widthS < 0) widthS = 0;
	if (widthE > 1000) widthE = 1000;
	if (heightS < 0) heightS = 0;
	if (heightE > 1000) heightE = 1000;
}


int stringToInt (string a) {
	int b = 0, multi = 1;
	
	if (a.size() == 0) return 0;
	
	for (int i = a.size() - 1; i >= 0; i--) {
		if (a[i] < '0' || '9' < a[i]) b += 0;
		else b += (a[i] - (int)'0') * multi;
		
		multi *= 10;
	}
	
	return b;
}





void updatingList () {
	if (!updateList) return;
	
	// making a list and sorting it
	colorCount.clear();
	
	for (int i = 0; i < colors.size() + 2; i++) colorCount.emplace_back (i, 0);
	
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			if (isPlaced[j][i]) colorCount[gridColor[j][i]].second++;
		}
	}
	
	sort (colorCount.begin(), colorCount.end(), sortColorsFunc);
	
	// checking up to what point to count
	int pieceNum = 0;
	for (int i = 0; i < colorCount.size(); i++) {
		if (colorCount[i].second == 0) break;
		pieceNum++; 
	}
	
//	pieceListBg.setSize(Vector2f (pieceNum * 100))
	
	ofstream nibba ("checktest.txt");
	nibba << pieceNum;
	nibba.close();
	
	updateList = false;
}


bool sortColorsFunc (myPair a, myPair b) {
	return a.second > b.second;
}


