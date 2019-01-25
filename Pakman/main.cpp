#include "SFML/Window.hpp" // Dodane biblioteki SFML
#include "SFML/Graphics.hpp"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>

using namespace std; 
using namespace sf;


#define SIZE_WINDOW_X		400		// Szerokoœæ okna
#define SIZE_WINDOW_Y		440		// Wysokoœæ okna
#define SIZE_GRID			40.0f	// Szerokoœæ i wysokoœæ "siatki" z której jest utworzone pole grania
#define GRID_X				10		// Iloœæ "siatek" w osi x
#define GRID_Y				11		// Iloœæ "siatek" w osi y
#define RIGHT_WALL			0x08	// Wartoœæ œciany prawej
#define LEFT_WALL			0x04	// Wartoœæ œciany lewej
#define BOTTOM_WALL			0x02	// Wartoœæ œciany dolnej
#define TOP_WALL			0x01	// Wartoœæ œciany górnej
#define GHOST_NUMBER		4		// Iloœæ duszków które zostan¹ stworzone w grze
#define POINTS_TO_WIN		82	// Iloœæ punktów które trzeba zebraæ by wygraæ (82)
#define TIME_PER_TICK		400		// Iloœæ czasu pomiêdzy klatkami gry
#define TIME_PER_FRUIT		12000	// Iloœæ czasu pomiêdzy pojawieniem siê owocków

RenderWindow mainWindow(VideoMode( SIZE_WINDOW_X, SIZE_WINDOW_Y, 32), "Pakman"); // Tworzymy okno programu (rozmiary, skala kolorów, nazwa)
RenderWindow secondWindow;
Sprite sprite_pakman, sprite_point, sprite_ghost[GHOST_NUMBER], sprite_tlo, sprite_fruit; // Tworzenie obiektów Sprite
Texture pakman_prawo, pakman_lewo;								// Tworzymy obiekty tekstur
Texture pelny, pusty, gora, dol, lewo, prawo, lewo_prawo, gora_dol, gora_lewo, gora_prawo, dol_lewo, dol_prawo;
Texture blue_ghost, red_ghost, orange_ghost, green_ghost;
Texture point_texture, fruit_texture;
Time last, now, startFruit, endFruit;

class Character
{
public:
	void move();
	bool checkForCollision();
protected:
	int direction;
	int lastDirection;
};

class Pakman : public Character
{
public:
	Pakman()
	{
		pakman_prawo.loadFromFile("images/pakman.png");
		sprite_pakman.setTexture(pakman_prawo);
		sprite_pakman.setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
		sprite_pakman.setPosition(SIZE_GRID * 4 + SIZE_GRID / 2, SIZE_GRID * 6 + SIZE_GRID / 2); // Pozycja startowa Pakmana
		sprite_pakman.setScale(1, 1);
	}
	void setDirection(int dir)
	{
		direction = dir;
	}
	void move();
};
Pakman pakman;
 
class Ghost : public Character
{
public:
	void move();
	Ghost(int number)
	{
		id = number-1;
		if (id == 0)
		{
			blue_ghost.loadFromFile("images/ghost_blue.png");
			sprite_ghost[id].setTexture(blue_ghost);
			sprite_ghost[id].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 0 + SIZE_GRID / 2); // Pozycje startowe duszków
		}
		else if (id == 1)
		{
			green_ghost.loadFromFile("images/ghost_green.png");
			sprite_ghost[id].setTexture(green_ghost);
			sprite_ghost[id].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 0 + SIZE_GRID / 2); // Pozycje startowe duszków
		}
		else if (id == 2)
		{
			orange_ghost.loadFromFile("images/ghost_orange.png");
			sprite_ghost[id].setTexture(orange_ghost);
			sprite_ghost[id].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2); // Pozycje startowe duszków
		}
		else if (id == 3)
		{
			red_ghost.loadFromFile("images/ghost_red.png");
			sprite_ghost[id].setTexture(red_ghost);
			sprite_ghost[id].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2); // Pozycje startowe duszków
		}

		sprite_ghost[id].setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
		sprite_ghost[id].setScale(1, 1);
	}
private:
	int id;
};
Ghost ghost1(1);
Ghost ghost2(2);
Ghost ghost3(3);
Ghost ghost4(4);

class Area
{
public:
	uint8_t getWallsAt(int x, int y)
	{
		return gameArea[y][x];
	}
private:
	// Plansza ze œcianami
	uint8_t gameArea[GRID_Y][GRID_X] =
	{
		0x5, 0x3, 0x1, 0x3, 0x9, 0x5, 0x3, 0x1, 0x3, 0x9,
		0xC, 0xF, 0xC, 0xF, 0xC, 0xC, 0xF, 0xC, 0xF, 0xC,
		0x4, 0x3, 0x0, 0x1, 0x2, 0x2, 0x1, 0x0, 0x3, 0x8,
		0x6, 0x3, 0x8, 0x6, 0x9, 0x5, 0xA, 0x4, 0x3, 0xA,
		0xF, 0xF, 0xC, 0x5, 0x2, 0x2, 0x9, 0xC, 0xF, 0xF,
		0x3, 0x3, 0x0, 0x8, 0xF, 0xF, 0x4, 0x0, 0x3, 0x3,
		0xF, 0xF, 0xC, 0x4, 0x3, 0x3, 0x8, 0xC, 0xF, 0xF,
		0x5, 0x3, 0x0, 0x2, 0x9, 0x5, 0x2, 0x0, 0x3, 0x9,
		0x6, 0x9, 0x4, 0x1, 0x2, 0x2, 0x1, 0x8, 0x5, 0xA,
		0x5, 0x2, 0xA, 0x6, 0x9, 0x5, 0xA, 0x6, 0x2, 0x9,
		0x6, 0x3, 0x3, 0x3, 0x2, 0x2, 0x3, 0x3, 0x3, 0xA,
	};
};
Area gameArea;

class Point
{
public:
	bool isPointIn(int x, int y)
	{
		return points_table[y][x];
	}
	void pointTaken(int x, int y)
	{
		points_table[y][x] = 0;
		pointsTotal += value;
	}
	int count()
	{
		return pointsTotal;
	}
	void collectPoints();
	void placeFruit()
	{
		points_table[6][4] = 2;
	};
	void deleteFruit()
	{
		points_table[6][4] = 0;
	};
protected:
	int value = 1; // Wartoœæ punktu
	int pointsTotal = 0; // Iloœæ punktów zdobytych przez gracza
	int points_table[GRID_Y][GRID_X] = // Tabela z punktami
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 1, 0, 1, 1, 0, 1, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
};
Point point;

class Fruit
{
public:
	int getTimeLeft()
	{
		return this->timeLeft_Milliseconds;
	}
	bool isFruitset()
	{
		return this->fruitSet;
	}

	void setFruit(bool value)
	{
		this->fruitSet = value;
	}
	void setValue(int value)
	{
		this->value = value;
	}
	void setTime(int time)
	{
		this->timeLeft_Milliseconds = time;
	}
	bool fruitSet;
	int value;
	int timeLeft_Milliseconds;
};
Fruit fruit;

class Cherry : public Fruit, public Point
{
public:
	void set(bool create)
	{
		if (create == true)
		{
			fruit.setFruit(true);
			point.placeFruit();
			fruit.setValue(20);
			fruit.setTime(6000);
			fruit_texture.loadFromFile("images/cherry.png");
			sprite_fruit.setTexture(fruit_texture);
			sprite_fruit.setPosition(SIZE_GRID * 4 + SIZE_GRID / 2, SIZE_GRID * 6 + SIZE_GRID / 2); // Pozycja owocu
		}
		else
		{
			fruit.setFruit(false);
			point.deleteFruit();
			sprite_fruit.setPosition(-SIZE_GRID, -SIZE_GRID);
		}
	}
};
Cherry cherry;

class Orange : public Fruit, public Point
{
public:
	void set(bool create)
	{
		if (create == true)
		{
			fruit.setFruit(true);
			point.placeFruit();
			fruit.setValue(15);
			fruit.setTime(8000);
			fruit_texture.loadFromFile("images/orange.png");
			sprite_fruit.setTexture(fruit_texture);
			sprite_fruit.setPosition(SIZE_GRID * 4 + SIZE_GRID / 2, SIZE_GRID * 6 + SIZE_GRID / 2); // Pozycja owocu
		}
		else
		{
			fruit.setFruit(false);
			point.deleteFruit();
			sprite_fruit.setPosition(-SIZE_GRID, -SIZE_GRID);
		}
	}
};
Orange orange;


void saveScore(string score)
{
	string temporary, line;
	ifstream fileInput;
	ofstream fileOutput;
	try
	{
		fileInput.open("lastScore/score.txt");
		if (fileInput.fail())
		{
			throw -1;
		}
		if (fileInput.is_open() == true)
		{
			while (getline(fileInput, line))
			{
				temporary = temporary + line + "\n";
			}
		}
	}
	catch (int fail)
	{
		abort();
	}

	try
	{
		fileOutput.open("lastScore/score.txt");
		if (fileInput.fail())
		{
			throw - 1;
		}
		fileOutput << temporary << score;
		if (point.count() >= POINTS_TO_WIN)
		{
			fileOutput << " WINNER !!!" << endl;
		}
		else
		{
			fileOutput << " LOOSER :(" << endl;
		}
		fileOutput.close();
		fileInput.close();

	}
		catch (int fail)
	{
		abort();
	}
}

int main()
{
	// Ustawienia na pocz¹tku rozgrywki
	Clock clock;
	Time last, now, startFruit, endFruit;
	now = clock.getElapsedTime();
	last = now;
	startFruit = now;
	endFruit = now;
	srand(time(NULL));
	Font arial;
	arial.loadFromFile("fonts/arial.ttf");
	Event mainEvent, secondaryEvent;

	pakman_prawo.loadFromFile("images/pakman.png");
	pakman_lewo.loadFromFile("images/pakman_left.png");
	pelny.loadFromFile("images/0xF.png");
	pusty.loadFromFile("images/0x0.png");
	gora.loadFromFile("images/0x1.png");
	dol.loadFromFile("images/0x2.png");
	lewo.loadFromFile("images/0x4.png");
	prawo.loadFromFile("images/0x8.png");
	lewo_prawo.loadFromFile("images/0xC.png");
	gora_dol.loadFromFile("images/0x3.png");
	gora_lewo.loadFromFile("images/0x5.png");
	dol_lewo.loadFromFile("images/0x6.png");
	gora_prawo.loadFromFile("images/0x9.png");
	dol_prawo.loadFromFile("images/0xA.png");
	point_texture.loadFromFile("images/point.png");
	
	fruit_texture.loadFromFile("images/orange.png");
	fruit_texture.loadFromFile("images/cherry.png");
	sprite_fruit.setTexture(fruit_texture);
	sprite_fruit.setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
	sprite_fruit.setPosition(-SIZE_GRID, -SIZE_GRID);
	sprite_fruit.setScale(1, 1);

	sprite_tlo.setScale(1, 1);
	sprite_point.setScale(1, 1);
	sprite_point.setTexture(point_texture);
	
	while(mainWindow.isOpen())
	{
		now = clock.getElapsedTime();

		while (mainWindow.pollEvent(mainEvent))
		{
			if (mainEvent.type == Event::Closed || (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Escape)) // Je¿eli podczas gry naciœniemy Escape albo Krzy¿yk to okno zostanie zamkniête.
			{
				mainWindow.close();
			}

			if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Right) // Je¿eli naciœniety klawisz to strza³ka w prawo
			{
				pakman.setDirection(1);
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Down) // Je¿eli naciœniety klawisz to strza³ka w dó³
			{
				pakman.setDirection(2);
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Left) // Je¿eli naciœniety klawisz to strza³ka w lewo
			{
				pakman.setDirection(3);
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Up) // Je¿eli naciœniety klawisz to strza³ka w górê
			{
				pakman.setDirection(4);
			}
		}

		if ((now.asMilliseconds() - last.asMilliseconds()) > TIME_PER_TICK)
		{
			last = now;
			
			pakman.move();
			if (pakman.checkForCollision() == true) //Sprawdzenie kolizji
			{
				mainWindow.close();
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "YOU LOST!");
			}
			ghost1.move();
			ghost2.move();
			ghost3.move();
			ghost4.move();
			point.collectPoints();

			if (pakman.checkForCollision() == true) //Sprawdzenie kolizji
			{
				mainWindow.close();
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "YOU LOST!");
			}
			else if (point.count() >= POINTS_TO_WIN) //Sprawdzenie wygranej
			{
				mainWindow.close();
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "YOU WON!");
			}
		}

		if(fruit.isFruitset() == false)
		{
			if ((now.asMilliseconds() - endFruit.asMilliseconds()) > TIME_PER_FRUIT)
			{
				(rand() % 2) ? cherry.set(true) : orange.set(true);
				startFruit = now;
				endFruit = now;
			}
		}
		else
		{
			if ((now.asMilliseconds() - startFruit.asMilliseconds()) > fruit.getTimeLeft())
			{
				cherry.set(false);
				orange.set(false);
				startFruit = now;
				endFruit = now;
			}
		}

		//"Rysowanie" okna
		mainWindow.clear(); 
		for (int i = 0; i < GRID_X; i++) 
		{
			for (int j = 0; j < GRID_Y; j++)
			{
				switch (gameArea.getWallsAt(i,j))
				{
					case 0x0:
						sprite_tlo.setTexture(pusty);
						break;
					case 0x1:
						sprite_tlo.setTexture(gora);
						break;
					case 0x2:
						sprite_tlo.setTexture(dol);
						break;
					case 0x4:
						sprite_tlo.setTexture(lewo);
						break;
					case 0x8:
						sprite_tlo.setTexture(prawo);
						break;
					case 0x3:
						sprite_tlo.setTexture(gora_dol);
						break;
					case 0xC:
						sprite_tlo.setTexture(lewo_prawo);
						break;
					case 0x5:
						sprite_tlo.setTexture(gora_lewo);
						break;
					case 0x6:
						sprite_tlo.setTexture(dol_lewo);
						break;
					case 0x9:
						sprite_tlo.setTexture(gora_prawo);
						break;
					case 0xA:
						sprite_tlo.setTexture(dol_prawo);
						break;
					case 0xF:
						sprite_tlo.setTexture(pelny);
						break;
					default:
						sprite_tlo.setTexture(pusty);
						break;
				}
				sprite_tlo.setPosition(i*SIZE_GRID, j*SIZE_GRID);
				mainWindow.draw(sprite_tlo);

				if (point.isPointIn(i, j) == true)
				{
					sprite_point.setPosition(i*SIZE_GRID, j*SIZE_GRID); 
					mainWindow.draw(sprite_point); 
				}
			}
		}
		mainWindow.draw(sprite_ghost[0]);
		mainWindow.draw(sprite_ghost[1]);
		mainWindow.draw(sprite_ghost[2]);
		mainWindow.draw(sprite_ghost[3]);
		mainWindow.draw(sprite_pakman); 
		mainWindow.draw(sprite_fruit);
		mainWindow.display(); 
	}
	while (secondWindow.isOpen())
	{

		string pointsDisplay;
		pointsDisplay = "Your score: " + to_string(point.count());
		Text pointsNow(pointsDisplay, arial, 30);
		pointsNow.setPosition(SIZE_GRID, SIZE_GRID);

		while (secondWindow.pollEvent(secondaryEvent))
		{
			if (secondaryEvent.type == Event::Closed || (secondaryEvent.type == Event::KeyPressed && secondaryEvent.key.code == Keyboard::Escape)) // Je¿eli podczas gry naciœniemy Escape albo Krzy¿yk to okno zostanie zamkniête.
			{
				saveScore(pointsDisplay);
				secondWindow.close();
			}
		}

		secondWindow.draw(pointsNow);
		secondWindow.display();
	}
	return 0;
}


void Pakman::move()
{
	int position_x = (int(sprite_pakman.getPosition().x) / SIZE_GRID);
	int position_y = (int(sprite_pakman.getPosition().y) / SIZE_GRID);
	uint8_t current_grid = gameArea.getWallsAt(position_x, position_y);

	switch (direction)
	{
	case 1:
		if (!(current_grid & RIGHT_WALL))
		{
			lastDirection = direction;
		}
		break;

	case 2:
		if (!(current_grid & BOTTOM_WALL))
		{
			lastDirection = direction;
		}
		break;

	case 3:
		if (!(current_grid & LEFT_WALL))
		{
			lastDirection = direction;
		}
		break;

	case 4:
		if (!(current_grid & TOP_WALL))
		{
			lastDirection = direction;
		}
		break;
	default:
		break;
	}

	switch (lastDirection)
	{
	case 1:
		if (!(current_grid & RIGHT_WALL))
		{
			sprite_pakman.setRotation(0);
			sprite_pakman.setTexture(pakman_prawo);
			sprite_pakman.move(SIZE_GRID, 0);
		}
		break;

	case 2:
		if (!(current_grid & BOTTOM_WALL))
		{
			sprite_pakman.setRotation(90);
			sprite_pakman.setTexture(pakman_prawo);
			sprite_pakman.move(0, SIZE_GRID);
		}
		break;

	case 3:
		if (!(current_grid & LEFT_WALL))
		{
			sprite_pakman.setRotation(0);
			sprite_pakman.setTexture(pakman_lewo);
			sprite_pakman.move(-SIZE_GRID, 0);
		}
		break;

	case 4:
		if (!(current_grid & TOP_WALL))
		{
			sprite_pakman.setRotation(270);
			sprite_pakman.setTexture(pakman_prawo);
			sprite_pakman.move(0, -SIZE_GRID);
		}
		break;
	default:
		break;
	}

	if (position_x < 0)
	{
		sprite_pakman.move(GRID_X * SIZE_GRID, 0);
	}
	else if (position_x >= GRID_X)
	{
		sprite_pakman.move(GRID_X * SIZE_GRID * (-1), 0);
	}
}

void Ghost::move()
{
	int ghost_x = (int(sprite_ghost[id].getPosition().x) / SIZE_GRID);
	int ghost_y = (int(sprite_ghost[id].getPosition().y) / SIZE_GRID);
	uint8_t ghost_grid = gameArea.getWallsAt(ghost_x, ghost_y);
	bool moved = false;

	while (moved == false)
	{
		direction = ((rand() % 12) + 1);
		if (direction < 5)
		{
			switch (direction)
			{
			case 1:
				if (!(ghost_grid & RIGHT_WALL))
				{
					lastDirection = direction;
				}
				break;
			case 2:
				if (!(ghost_grid & BOTTOM_WALL))
				{
					lastDirection = direction;
				}
				break;
			case 3:
				if (!(ghost_grid & LEFT_WALL))
				{
					lastDirection = direction;
				}
				break;
			case 4:
				if (!(ghost_grid & TOP_WALL))
				{
					lastDirection = direction;
				}
				break;
			default:
				break;
			}
		}

		switch (lastDirection)
		{
		case 1:
			if (!(ghost_grid & RIGHT_WALL))
			{
				sprite_ghost[id].move(SIZE_GRID, 0);
				moved = true;
			}
			break;
		case 2:
			if (!(ghost_grid & BOTTOM_WALL))
			{
				sprite_ghost[id].move(0, SIZE_GRID);
				moved = true;
			}
			break;
		case 3:
			if (!(ghost_grid & LEFT_WALL))
			{
				sprite_ghost[id].move(SIZE_GRID * (-1), 0);
				moved = true;
			}
			break;
		case 4:
			if (!(ghost_grid & TOP_WALL))
			{
				sprite_ghost[id].move(0, SIZE_GRID * (-1));
				moved = true;
			}
			break;
		default:
			break;
		}

		ghost_x = (sprite_ghost[id].getPosition().x / SIZE_GRID);
		ghost_y = (sprite_ghost[id].getPosition().y / SIZE_GRID);


		if (ghost_x < 0)
		{
			sprite_ghost[id].move(GRID_X * SIZE_GRID, 0);
		}
		else if (ghost_x >= GRID_X)
		{
			sprite_ghost[id].move(GRID_X * SIZE_GRID * (-1), 0);
		}
	}
}

// Sprawdzamy czy wyst¹pi³a kolizja Pakmana z duszkiem
bool Character::checkForCollision()
{
	float pakman_x = (sprite_pakman.getPosition().x / SIZE_GRID);
	float pakman_y = (sprite_pakman.getPosition().y / SIZE_GRID);


	for (int current_ghost = 0; current_ghost < GHOST_NUMBER; current_ghost++)
	{
		float ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID);
		float ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID);

		int difference_x = (int)pakman_x - (int)ghost_x;
		int difference_y = (int)pakman_y - (int)ghost_y;

		if ((difference_x == 0) && (difference_y == 0))
		{
			return true;
		}
	}
	return false;
}

void Point::collectPoints()
{
	int position_x = (int(sprite_pakman.getPosition().x) / SIZE_GRID);
	int position_y = (int(sprite_pakman.getPosition().y) / SIZE_GRID);

	if (isPointIn(position_x, position_y) == true)
	{
		pointTaken(position_x, position_y);
	}
	if (fruit.isFruitset() == true && position_x == 4 && position_y == 6)
	{
		point.pointsTotal += fruit.value;
		point.deleteFruit();
		cherry.set(false);
		orange.set(false);
		startFruit = now;
		endFruit = now;
	}
}
