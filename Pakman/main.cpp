#include "SFML/Window.hpp" // Dodane biblioteki SFML
#include "SFML/Graphics.hpp"
#include <ctime>
#include <string>

using namespace std; 
using namespace sf;


#define SIZE_WINDOW_X		400		// Szeroko�� okna
#define SIZE_WINDOW_Y		440		// Wysoko�� okna
#define SIZE_GRID			40.0f	// Szeroko�� i wysoko�� "siatki" z kt�rej jest utworzone pole grania
#define GRID_X				10		// Ilo�� "siatek" w osi x
#define GRID_Y				11		// Ilo�� "siatek" w osi y
#define RIGHT_WALL			0x08	// Warto�� �ciany prawej
#define LEFT_WALL			0x04	// Warto�� �ciany lewej
#define BOTTOM_WALL			0x02	// Warto�� �ciany dolnej
#define TOP_WALL			0x01	// Warto�� �ciany g�rnej
#define GHOST_NUMBER		4		// Ilo�� duszk�w kt�re zostan� stworzone w grze
#define POINTS_TO_WIN		82		// Ilo�� punkt�w kt�re trzeba zebra� by wygra� (82)
#define TIME_PER_TICK		250		// 1 jednostka czasu w grze

RenderWindow mainWindow(VideoMode( SIZE_WINDOW_X, SIZE_WINDOW_Y, 32), "Pakman"); // Tworzymy okno programu (rozmiary, skala kolor�w, nazwa) 
Sprite sprite_pakman, sprite_point, sprite_ghost[GHOST_NUMBER]; // Tworzenie obiekt�w Sprite
Texture pakman_prawo, pakman_lewo;								// Tworzymy obiekty tekstur
Texture pelny, pusty, gora, dol, lewo, prawo, lewo_prawo, gora_dol, gora_lewo, gora_prawo, dol_lewo, dol_prawo;
Texture blue_ghost, red_ghost, orange_ghost, green_ghost;
Texture point_texture; 

int points = 0; // Ilo�� punkt�w zdobytych przez gracza
int directionPakman = 0;	// Kierunek ruchu Pakmana
int lastDirectionPakman;	// Ostatni kierunek Pakmana
int lastDirectionGhost[GHOST_NUMBER];	// Ostatnio kierunek duszka

// Plansza ze �cianami
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

// Tabela z punktami
bool points_table[GRID_Y][GRID_X] =
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

// Funkcja movePakman s�u�y do poruszania Pakmana.
// Funkcja sprawdza czy z aktualnej pozycji Pakmana da si� poruszy� w zadanym kierunku i zmienia pozycj� Pakmana je�eli to mo�liwe.
// Je�eli Pakman jest w miejscu w kt�rym znajduj� si� punkt to ten punkt jest usuwany z planszy, a dodany do tych zdobytych przez Pakmana.
void movePakman()
{
	float position_x = (sprite_pakman.getPosition().x / SIZE_GRID); 
	float position_y = (sprite_pakman.getPosition().y / SIZE_GRID); 
	uint8_t current_grid = gameArea[int(position_y)][int(position_x)]; // Pobranie warto�ci �cian z aktualnego miejsca Pakmana

	switch (directionPakman)
	{
	case 1:
		if (!(current_grid & RIGHT_WALL))
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 2:
		if (!(current_grid & BOTTOM_WALL)) 
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 3: 
		if (!(current_grid & LEFT_WALL))
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 4: 
		if (!(current_grid & TOP_WALL)) 
		{
			lastDirectionPakman = directionPakman;
		}
		break;
	default:
		break;
	}

	switch (lastDirectionPakman)
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
			sprite_pakman.move( -SIZE_GRID, 0); 
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

	position_x = (sprite_pakman.getPosition().x / SIZE_GRID);
	position_y = (sprite_pakman.getPosition().y / SIZE_GRID);

	
	if (position_x < 0) 
	{
		sprite_pakman.move(GRID_X * SIZE_GRID,0);
	}
	else if (position_x >= GRID_X)
	{
		sprite_pakman.move(GRID_X * SIZE_GRID * (-1), 0);
	}
	else if (points_table[int(position_y)][int(position_x)] == true) 
	{
		points_table[int(position_y)][int(position_x)] = false; 
		points++; 
	}
}

// Funkcja moveGhosts s�u�y do poruszania duszkami
// Funkcja sprawdza czy z aktualnej pozycji duszka da si� poruszy� w obranym kierunku i zmienia pozycj� duszka je�eli to mo�liwe.
void moveGhosts()
{
	float ghost_x, ghost_y;
	uint8_t ghost_grid;
	int directionGhost;

	for (int current_ghost = 0; current_ghost < GHOST_NUMBER; current_ghost++)
	{
		ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID); 
		ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID); 
		ghost_grid = gameArea[int(ghost_y)][int(ghost_x)]; 
		bool moved = false;

		while (moved == false)
		{
			directionGhost = ((rand() % 12) + 1);
			if (directionGhost < 5) 
			{
				switch (directionGhost)
				{
				case 1:
					if (!(ghost_grid & RIGHT_WALL)) 
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 2:
					if (!(ghost_grid & BOTTOM_WALL))
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 3:
					if (!(ghost_grid & LEFT_WALL))
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 4:
					if (!(ghost_grid & TOP_WALL))
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				default:
					break;
				}
			}

			switch (lastDirectionGhost[current_ghost]) 
			{
			case 1:
				if (!(ghost_grid & RIGHT_WALL)) 
				{
					sprite_ghost[current_ghost].move(SIZE_GRID, 0);
					moved = true;
				}
				break;
			case 2:
				if (!(ghost_grid & BOTTOM_WALL)) 
				{
					sprite_ghost[current_ghost].move(0, SIZE_GRID);
					moved = true;
				}
				break;
			case 3:
				if (!(ghost_grid & LEFT_WALL)) 
				{
					sprite_ghost[current_ghost].move(SIZE_GRID * (-1), 0);
					moved = true;
				}
				break;
			case 4:
				if (!(ghost_grid & TOP_WALL)) 
				{
					sprite_ghost[current_ghost].move(0, SIZE_GRID * (-1));
					moved = true;
				}
				break;
			default:
				break;
			}

			ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID); 
			ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID); 

			
			if (ghost_x < 0)
			{
				sprite_ghost[current_ghost].move(GRID_X * SIZE_GRID, 0); 
			}
			else if (ghost_x >= GRID_X) 
			{
				sprite_ghost[current_ghost].move(GRID_X * SIZE_GRID * (-1), 0); 
			}
		}
	}
}

// Sprawdzamy czy wyst�pi�a kolizja Pakmana z duszkiem
bool checkForCollision()
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

int main()
{
	Clock clock;
	Time last, now;
	last = clock.getElapsedTime();
	now = last;
	RenderWindow secondWindow;
	srand(time(NULL));
	Font arial;
	arial.loadFromFile("fonts/arial.ttf");
	Event mainEvent;
	Event secondaryEvent;

	pakman_prawo.loadFromFile("images/pakman.png");
	pakman_lewo.loadFromFile("images/pakman_left.png");
	blue_ghost.loadFromFile("images/ghost_blue.png");
	red_ghost.loadFromFile("images/ghost_red.png");
	orange_ghost.loadFromFile("images/ghost_orange.png");
	green_ghost.loadFromFile("images/ghost_green.png");

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

	Sprite sprite_tlo; // Tworzymy t�o

	// Ustawienia na pocz�tku rozgrywki
	sprite_pakman.setTexture(pakman_prawo); 
	sprite_pakman.setOrigin(SIZE_GRID/2, SIZE_GRID/2); 
	sprite_pakman.setPosition(SIZE_GRID * 4 + SIZE_GRID / 2, SIZE_GRID * 6 + SIZE_GRID / 2); // Pozycja startowa Pakmana
	sprite_pakman.setScale(1, 1);
	sprite_tlo.setScale(1, 1);
	sprite_point.setScale(1, 1);
	sprite_point.setTexture(point_texture);

	sprite_ghost[0].setTexture(blue_ghost);
	sprite_ghost[1].setTexture(green_ghost);
	sprite_ghost[2].setTexture(orange_ghost);
	sprite_ghost[3].setTexture(red_ghost);
	sprite_ghost[0].setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
	sprite_ghost[1].setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
	sprite_ghost[2].setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
	sprite_ghost[3].setOrigin(SIZE_GRID / 2, SIZE_GRID / 2);
	sprite_ghost[0].setScale(1, 1);
	sprite_ghost[1].setScale(1, 1);
	sprite_ghost[2].setScale(1, 1);
	sprite_ghost[3].setScale(1, 1);
	sprite_ghost[0].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 0 + SIZE_GRID / 2); // Pozycje startowe duszk�w
	sprite_ghost[1].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 1 + SIZE_GRID / 2);
	sprite_ghost[2].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2);
	sprite_ghost[3].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2);

	
	while(mainWindow.isOpen())
	{
		now = clock.getElapsedTime();

		while (mainWindow.pollEvent(mainEvent))
		{
			if (mainEvent.type == Event::Closed || (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Escape)) // Je�eli podczas gry naci�niemy Escape albo Krzy�yk to okno zostanie zamkni�te.
			{
				mainWindow.close();
			}

			if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Right) // Je�eli naci�niety klawisz to strza�ka w prawo
			{
				directionPakman = 1;
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Down) // Je�eli naci�niety klawisz to strza�ka w d�
			{
				directionPakman = 2;
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Left) // Je�eli naci�niety klawisz to strza�ka w lewo
			{
				directionPakman = 3;
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Up) // Je�eli naci�niety klawisz to strza�ka w g�r�
			{
				directionPakman = 4;
			}
		}

		if (checkForCollision() == true) //Sprawdzenie kolizjii
		{
			mainWindow.close(); 
			secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "PRZEGRALES!"); 
		}
		else if (points == POINTS_TO_WIN) //Sprawdzenie wygranej
		{
			mainWindow.close(); 
			secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "WYGRALES!"); 
		}

		if ((now.asMilliseconds() - last.asMilliseconds()) > TIME_PER_TICK)
		{
			movePakman(); 

			last = now;

			if (checkForCollision() == true) 
			{
				mainWindow.close();
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "PRZEGRALES!"); 
			}

			moveGhosts(); 
		}

		//"Rysowanie" okna
		mainWindow.clear(); 
		for (int i = 0; i < GRID_Y; i++) 
		{
			for (int j = 0; j < GRID_X; j++)
			{
				switch (gameArea[i][j]) 
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
				sprite_tlo.setPosition(j*SIZE_GRID, i*SIZE_GRID); 
				mainWindow.draw(sprite_tlo);

				if (points_table[i][j] == true) 
				{
					sprite_point.setPosition(j*SIZE_GRID, i*SIZE_GRID); 
					mainWindow.draw(sprite_point); 
				}
			}
		}
		mainWindow.draw(sprite_ghost[0]);
		mainWindow.draw(sprite_ghost[1]);
		mainWindow.draw(sprite_ghost[2]);
		mainWindow.draw(sprite_ghost[3]);
		mainWindow.draw(sprite_pakman); 
		mainWindow.display(); 
	}
	while (secondWindow.isOpen())
	{
		while (secondWindow.pollEvent(secondaryEvent))
		{
			if (secondaryEvent.type == Event::Closed || (secondaryEvent.type == Event::KeyPressed && secondaryEvent.key.code == Keyboard::Escape)) // Je�eli podczas gry naci�niemy Escape albo Krzy�yk to okno zostanie zamkni�te.
			{
				secondWindow.close();
			}
		}

		string pointsDisplay;
		pointsDisplay = "Punkty: " + to_string(points);
		Text pointsNow(pointsDisplay, arial, 30);
		pointsNow.setPosition(SIZE_GRID, SIZE_GRID);

		secondWindow.draw(pointsNow);
		secondWindow.display();
	}
	return 0;
}