#include "SFML/Window.hpp" // Dodane biblioteki SFML
#include "SFML/Graphics.hpp"
#include <ctime>

using namespace std; // ¯eby nie u¿ywaæ ci¹gle std:: i sf::
using namespace sf;

 // Definicje. Sta³e. Nie da siê ich zmieniæ i zawsze znacz¹ to co tutaj zostanie im przypisane przy kompilacji.
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
#define POINTS_TO_WIN		82		// Iloœæ punktów które trzeba zebraæ by wygraæ (82)

RenderWindow mainWindow(VideoMode( SIZE_WINDOW_X, SIZE_WINDOW_Y, 32), "Pakman"); // Tworzymy okno programu. Obiekt mainWindow jest tworzony przez konstruktor, w nim podajemy rozmiary okna, skalê kolorów, a potem tytu³ okna.
Sprite sprite_pakman, sprite_point, sprite_ghost[GHOST_NUMBER]; // Tworzenie obiektów Sprite. S¹ to obiekty które mog¹ siê pojawiaæ na oknie programu.
Texture pakman_prawo, pakman_prawo_closed, pakman_lewo, pakman_lewo_closed; // Tworzymy obiekty tekstur. S¹ to obiekty którym póŸniej przypisane s¹ obrazy z komputera.
Texture pelny, pusty, gora, dol, lewo, prawo, lewo_prawo, gora_dol, gora_lewo, gora_prawo, dol_lewo, dol_prawo;
Texture blue_ghost, red_ghost, orange_ghost, green_ghost;
Texture point_texture; 

int points = 0; // Iloœæ punktów zdobytych przez gracza. Na pocz¹tku jest równa zero.
int directionPakman = 0;	// Kierunek ruchu Pakmana
int lastDirectionPakman;	// Ostatnio Pakman rusza³ siê w kierunku...
int lastDirectionGhost[GHOST_NUMBER];	// Ostatnio duszek porusza³ siê w stronê...


// uint8_t to jeden bajt, czyli 8 zer/jedynek.
// Tutaj gdy bit ustawiony jest na 1 to oznacza ¿e jest tam œciana. Gdy jest tam zero to oznacza ¿e nie ma tam œciany.
// Oznaczenia œcian s¹ nastêpuj¹ce: [x,x,x,x, prawo, lewo, dó³, góra], gdzie x to nie u¿ywane bity.
// Poni¿ej mamy tabelê tych wartoœci przedstawionych w formie szestnastkowej.
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

// Tabela z punktami, jest to tabela typu bool, prawda/fa³sz.
// Je¿eli w danym miejscu jest jedynka to oznacza ¿e jest w tym miejscu punkt, je¿eli zero to ¿e tego punktu tam nie ma.
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

// Funkcja movePakman s³u¿y do poruszania Pakmana. Pobiera argument dir który jest kierunkiem w którym u¿ytkownik chcia³by poruszyæ Pakmanem.
// Funkcja sprawdza czy z aktualnej pozycji Pakmana da siê poruszyæ w zadanym kierunku i zmienia pozycjê Pakmana je¿eli to mo¿liwe.
// Je¿eli Pakman jest w miejscu w którym znajdujê siê punkt to ten punkt jest usuwany z planszy, a dodany do tych zdobytych przez Pakmana.
void movePakman()
{
	float position_x = (sprite_pakman.getPosition().x / SIZE_GRID); // Pobranie pozycji X podzielonej przez szerokoœæ pojedyñczej "siatki"
	float position_y = (sprite_pakman.getPosition().y / SIZE_GRID); // Pobranie pozycji Y podzielonej przez szerokoœæ pojedyñczej "siatki"
	uint8_t current_grid = gameArea[int(position_y)][int(position_x)]; // Pobranie wartoœci œcian z aktualnego miejsca Pakmana

	switch (directionPakman)
	{
	case 1:
		if (!(current_grid & RIGHT_WALL))
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 2:
		if (!(current_grid & BOTTOM_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 3: // Kierunek 3 - lewo
		if (!(current_grid & LEFT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			lastDirectionPakman = directionPakman;
		}
		break;

	case 4: // Kierunek 4 - prawo
		if (!(current_grid & TOP_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			lastDirectionPakman = directionPakman;
		}
		break;
	default: // Kierunek ró¿ny od pozosta³ych czterech, akcja "domyœlna" gdy nie pasuje do ¿adnej z opcji
		break;
	}


	// Sprawdzamy w którym kierunku u¿ytkownik chce siê poruszaæ i dzia³amy zale¿nie od tego kierunku
	switch (lastDirectionPakman)
	{
	case 1: // Kierunek 1 - prawo
		if (!(current_grid & RIGHT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			sprite_pakman.setRotation(0); // Zmieniamy obrót na 0*
			sprite_pakman.setTexture(pakman_prawo); // Ustawiamy texturê przypisan¹ do Pakmana na praw¹
			sprite_pakman.move(SIZE_GRID, 0); // Poruszamy Pakmana o jedn¹ szerokoœæ siatki dalej w osi X.
		}
		break;

	case 2: // Kierunek 2 - dó³
		if (!(current_grid & BOTTOM_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			sprite_pakman.setRotation(90); // Zmieniamy obrót na 90*
			sprite_pakman.setTexture(pakman_prawo); // Ustawiamy texturê na praw¹
			sprite_pakman.move(0, SIZE_GRID); // Poruszamy Pakmanem o jedn¹ szerokoœæ siatki w dó³, czyli dodajemy szerokoœæ siatki do osi Y
		}
		break;

	case 3: // Kierunek 3 - lewo
		if (!(current_grid & LEFT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			sprite_pakman.setRotation(0); // Zmieniamy obrót na 0*
			sprite_pakman.setTexture(pakman_lewo); // Ustawiamy teksturê na lew¹
			sprite_pakman.move( -SIZE_GRID, 0); // Poruszamy Pakmanem o szerokoœæ siatki w lewo odejmuj¹c szerokoœæ siatki od osi X
		}
		break;

	case 4: // Kierunek 4 - prawo
		if (!(current_grid & TOP_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Pakmana w tym kierunku
		{
			sprite_pakman.setRotation(270); // Zmieniamy obrót na 270* (lub -90*)
			sprite_pakman.setTexture(pakman_prawo); // Ustawiamy teksturê na praw¹
			sprite_pakman.move(0, -SIZE_GRID); // Poruszamy Pakmana w górê odejmuj¹c szerokoœæ siatki od osi Y.
		}
		break;
	default: // Kierunek ró¿ny od pozosta³ych czterech, akcja "domyœlna" gdy nie pasuje do ¿adnej z opcji
		break;
	}

	position_x = (sprite_pakman.getPosition().x / SIZE_GRID); // Pobranie pozycji X podzielonej przez szerokoœæ pojedyñczej "siatki"
	position_y = (sprite_pakman.getPosition().y / SIZE_GRID); // Pobranie pozycji Y podzielonej przez szerokoœæ pojedyñczej "siatki"

	// Po poruszeniu trzeba sprawdziæ parê rzeczy
	if (position_x < 0) // Je¿eli Pakman poruszy³ siê poza planszê z lewej strony, to przesuwamy go na praw¹ stronê...
	{
		sprite_pakman.move(GRID_X * SIZE_GRID,0); // ...przesuwamy Pakmana o szerokoœæ siatki pomno¿on¹ przez jej iloœæ w osi X
	}
	else if (position_x >= GRID_X) // Je¿eli Pakman poruszy³ siê poza planszê z prawej strony...
	{
		sprite_pakman.move(GRID_X * SIZE_GRID * (-1), 0); // ...przesuwamy Pakmana o szerokoœæ siatki pomno¿on¹ przez jej iloœæ w osi X oraz przemno¿one przez -1
	}
	else if (points_table[int(position_y)][int(position_x)] == true) // Je¿eli w miejscu pakmana jest ustawiony punkt...
	{
		points_table[int(position_y)][int(position_x)] = false; // Usuwamy ten punkt z tabeli...
		points++; // ...i dodajemy go do tych zdobytych przez Pakmana
	}
}

// Funkcja moveGhosts s³u¿y do poruszania duszkami, wszystkimi.
// Funkcja sprawdza czy z aktualnej pozycji duszka da siê poruszyæ w obranym kierunku i zmienia pozycjê duszka je¿eli to mo¿liwe.
void moveGhosts()
{
	// Wartoœæ X i Y Pakmana jest taka sama dla ka¿dego duszka wiêc pobieramy j¹ przed pêtl¹.
	float pakman_x = (sprite_pakman.getPosition().x / SIZE_GRID); // Pobranie pozycji X Pakmana
	float pakman_y = (sprite_pakman.getPosition().y / SIZE_GRID); // Pobranie pozycji Y Pakmana
	float ghost_x, ghost_y;
	uint8_t ghost_grid;
	int directionGhost;

	// W tej pêtli przechodzimy po wszystkich duszkach i ruszamy ka¿dym z nich je¿eli to mo¿liwe
	for (int current_ghost = 0; current_ghost < GHOST_NUMBER; current_ghost++)
	{
		ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID); // Pobieramy X'ow¹ wartoœæ duszka
		ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID); // Pobieramy Y'ow¹ wartoœæ duszka
		ghost_grid = gameArea[int(ghost_y)][int(ghost_x)]; // Pobieramy wartoœæ pola w którym jest Duszek 
		bool moved = false;

		while (moved == false)
		{
			directionGhost = ((rand() % 12) + 1); // Ustawiamy randomow¹ wartoœæ od 1 do 8
			if (directionGhost < 5) // Je¿eli wartoœæ jest mniejsza od 4 to zmieniamy kierunek, inaczej, zostawiamy taki jak poprzedni
			{
				switch (directionGhost) // Tutaj tak jak w Pakmanie, wed³ug obranego kierunku przesuwamy Duszka
				{
				case 1:
					if (!(ghost_grid & RIGHT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 2:
					if (!(ghost_grid & BOTTOM_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 3:
					if (!(ghost_grid & LEFT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				case 4:
					if (!(ghost_grid & TOP_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
					{
						lastDirectionGhost[current_ghost] = directionGhost;
					}
					break;
				default:
					break;
				}
			}

			switch (lastDirectionGhost[current_ghost]) // Tutaj tak jak w Pakmanie, wed³ug obranego kierunku przesuwamy Duszka
			{
			case 1:
				if (!(ghost_grid & RIGHT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
				{
					sprite_ghost[current_ghost].move(SIZE_GRID, 0);
					moved = true;
				}
				break;
			case 2:
				if (!(ghost_grid & BOTTOM_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
				{
					sprite_ghost[current_ghost].move(0, SIZE_GRID);
					moved = true;
				}
				break;
			case 3:
				if (!(ghost_grid & LEFT_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
				{
					sprite_ghost[current_ghost].move(SIZE_GRID * (-1), 0);
					moved = true;
				}
				break;
			case 4:
				if (!(ghost_grid & TOP_WALL)) // Je¿eli nie ma œciany to mo¿emy ruszyæ Duszka w tym kierunku
				{
					sprite_ghost[current_ghost].move(0, SIZE_GRID * (-1));
					moved = true;
				}
				break;
			default:
				break;
			}

			ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID); // Pobieramy X'ow¹ wartoœæ duszka
			ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID); // Pobieramy Y'ow¹ wartoœæ duszka

			// Po poruszeniu trzeba sprawdziæ parê rzeczy
			if (ghost_x < 0) // Je¿eli Duszek poruszy³ siê poza planszê z lewej strony, to przesuwamy go na praw¹ stronê...
			{
				sprite_ghost[current_ghost].move(GRID_X * SIZE_GRID, 0); // ...przesuwamy Duszek o szerokoœæ siatki pomno¿on¹ przez jej iloœæ w osi X
			}
			else if (ghost_x >= GRID_X) // Je¿eli Duszek poruszy³ siê poza planszê z prawej strony...
			{
				sprite_ghost[current_ghost].move(GRID_X * SIZE_GRID * (-1), 0); // ...przesuwamy Duszek o szerokoœæ siatki pomno¿on¹ przez jej iloœæ w osi X oraz przemno¿one przez -1
			}
		}
	}
}

// Sprawdzamy czy wyst¹pi³a kolizja Pakmana z jakimkolwiek z duszków. Je¿eli tak to zwracamy prawdê, je¿eli ¿aden duszek nie jest w tym samym miejscu co Pakman to zwracamy fa³sz.
bool checkForCollision()
{
	float pakman_x = (sprite_pakman.getPosition().x / SIZE_GRID); // Pobranie pozycji X Pakmana
	float pakman_y = (sprite_pakman.getPosition().y / SIZE_GRID); // Pobranie pozycji Y Pakmana

	// W tej pêtli przechodzimy po wszystkich duszach i sprawdzamy czy s¹ w tej samej pozycji co Pakman
	for (int current_ghost = 0; current_ghost < GHOST_NUMBER; current_ghost++)
	{
		float ghost_x = (sprite_ghost[current_ghost].getPosition().x / SIZE_GRID); // Pobranie pozycji X Duszka
		float ghost_y = (sprite_ghost[current_ghost].getPosition().y / SIZE_GRID); // Pobranie pozycji Y Duszka

		int difference_x = (int)pakman_x - (int)ghost_x; // Liczymy ró¿nicê po osi X
		int difference_y = (int)pakman_y - (int)ghost_y; // Liczymy ró¿nicê po osi Y

		if ((difference_x == 0) && (difference_y == 0)) // Je¿eli obydwie ró¿nice s¹ równe zeru to zwracamy prawdê, Pakman zetkn¹³ siê z duszkiem.
		{
			return true;
		}
	}
	return false; // Je¿eli przejdziemy przez wszystkie duszki i nie zwrócimy wczeœniej prawdy to zwracamy fa³sz, Pakman nie zetkn¹³ siê z duszkiem.
}


// G³ówna funkcja, ustawiamy startowe zmienne i obs³ugujemy okna.
int main()
{
	Clock clock;
	Time last, now;
	last = clock.getElapsedTime();
	now = last;
	RenderWindow secondWindow;	// Definiujemy drugie okno, nie rusuj¹c go.
	srand(time(NULL));
	Font arial;
	arial.loadFromFile("fonts/arial.ttf");

	// Do wszystkich poprzednio zdefiniowanych tekstur wczytujemy obrazki z plików
	pakman_prawo.loadFromFile("images/pakman.png");
	pakman_lewo.loadFromFile("images/pakman_left.png");
	pakman_prawo_closed.loadFromFile("images/pakman_closed.png");
	pakman_lewo_closed.loadFromFile("images/pakman_left_closed.png");
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

	Sprite sprite_tlo; // Tworzymy obiekt tla.

	// Ustawienia na pocz¹tku rozgrywki
	sprite_pakman.setTexture(pakman_prawo); // Ustawiamy teksturê Pakmana na praw¹
	sprite_pakman.setOrigin(SIZE_GRID/2, SIZE_GRID/2); // Ustawiamy punkt œrodka Pakmana (wobec tego punktu potem obracamy Pakmanem)
	sprite_pakman.setPosition(SIZE_GRID * 4 + SIZE_GRID / 2, SIZE_GRID * 6 + SIZE_GRID / 2); // Ustawiamy Pakmana na pozycji startowej
	sprite_pakman.setScale(1, 1);	// Ustawiamy skalê sprite
	sprite_tlo.setScale(1, 1);
	sprite_point.setScale(1, 1);
	sprite_point.setTexture(point_texture); // Ustawiamy teksturê dla Sprite'a punktu

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
	sprite_ghost[0].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 0 + SIZE_GRID / 2);
	sprite_ghost[1].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 1 + SIZE_GRID / 2);
	sprite_ghost[2].setPosition(SIZE_GRID * 0 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2);
	sprite_ghost[3].setPosition(SIZE_GRID * 9 + SIZE_GRID / 2, SIZE_GRID * 10 + SIZE_GRID / 2);



	// Tutaj jest g³ówna pêtla programu, jest wykonywana zawsze gdy jakieœ okno programu jest otwarte, g³ówne, lub dodatkowe.
	while(mainWindow.isOpen())
	{
		now = clock.getElapsedTime();

		if ((now.asMilliseconds() - last.asMilliseconds()) > 200)
		{
			moveGhosts(); // Poruszamy duszkami
			last = now;

			if (checkForCollision() == true) // Je¿eli kolizja wyst¹pi³a, gracz przegra³ wiêc...
			{
				mainWindow.close(); // Zamykamy g³ówne okno
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "Pakman"); // Otwieramy drugie okno
				secondWindow.setTitle("PRZEGRALES!"); // Ustawiamy tytu³ mówi¹cy o przegranej
			}
			else if (points == POINTS_TO_WIN) // Je¿eli gracz nie przegra³, a punkty uzyskane s¹ równe z iloœci¹ potrzebn¹ do wygrania...
			{
				mainWindow.close(); // Zamykamy g³ówne okno
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "Pakman"); // Otwieramy drugie okno
				secondWindow.setTitle("WYGRALES!"); // Ustawiamy tytu³ mówi¹cy o wygranej
			}

			movePakman(); // Poruszamy Pakmanem u¿ywaj¹c kierunku ustawionego przez klawisze

			if (checkForCollision() == true) // Je¿eli kolizja wyst¹pi³a, gracz przegra³ wiêc...
			{
				mainWindow.close(); // Zamykamy g³ówne okno
				secondWindow.create(VideoMode(SIZE_WINDOW_X, SIZE_WINDOW_Y / 4, 32), "Pakman"); // Otwieramy drugie okno
				secondWindow.setTitle("PRZEGRALES!"); // Ustawiamy tytu³ mówi¹cy o przegranej
			}
		}

		Event mainEvent; // Tworzymy obiekt klasy Event. Do niego przypisane s¹ wydarzenia które odczytujemy.

		// Pêtla wykonuj¹ca siê gdy mamy otwarte okno g³ówne, sprawdza ka¿dorazowo czy wyst¹pi³y jakieœ wydarzenia zwi¹zane z tym oknem
		while (mainWindow.pollEvent(mainEvent))
		{
			if (mainEvent.type == Event::Closed || (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Escape)) // Je¿eli podczas gry naciœniemy Escape albo Krzy¿yk to okno zostanie zamkniête.
			{
				mainWindow.close(); // Zamkniêcie okna
			}

			if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Right) // Je¿eli naciœniety klawisz to strza³ka w prawo
			{
				directionPakman = 1; // Ustawiamy kierunek na prawo
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Down) // Je¿eli naciœniety klawisz to strza³ka w dó³
			{
				directionPakman = 2; // Ustawiamy kierunek na dó³
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Left) // Je¿eli naciœniety klawisz to strza³ka w lewo
			{
				directionPakman = 3; // Ustawiamy kierunek na lewo
			}
			else if (mainEvent.type == Event::KeyPressed && mainEvent.key.code == Keyboard::Up) // Je¿eli naciœniety klawisz to strza³ka w górê
			{
				directionPakman = 4; // Ustawiamy kierunek na górê
			}
		}

		mainWindow.clear(); // Czyszczenie okna
		for (int i = 0; i < GRID_Y; i++) // Pêtla po osi Y
		{
			for (int j = 0; j < GRID_X; j++) // Pêtla po osi X
			{
				switch (gameArea[i][j]) // Sprawdzamy ka¿de miejsce i ustawiamy dan¹ teksturê z danymi œcianami
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
				sprite_tlo.setPosition(j*SIZE_GRID, i*SIZE_GRID); // Ustawiamy tego sprite'a na pozycji XY
				mainWindow.draw(sprite_tlo); // I rysujemy tego sprite'a na oknie

				if (points_table[i][j] == true) // Je¿eli w tym miejscu jest ustawiony punkt
				{
					sprite_point.setPosition(j*SIZE_GRID, i*SIZE_GRID); // Ustawiamy go w tej pozycji na oknie
					mainWindow.draw(sprite_point); // I rysujemy go
				}
			}
		}
		mainWindow.draw(sprite_ghost[0]);
		mainWindow.draw(sprite_ghost[1]);
		mainWindow.draw(sprite_ghost[2]);
		mainWindow.draw(sprite_ghost[3]);
		mainWindow.draw(sprite_pakman); // Na koñcu rysujemy Pakmana
		mainWindow.display(); // I wyœwietlamy okno ze wszystkim ju¿ "narysowanym" :)
	}
	while (secondWindow.isOpen())
	{
		Event secondaryEvent;
		// Pêtla dla drugiego okna
		while (secondWindow.pollEvent(secondaryEvent))
		{
			if (secondaryEvent.type == Event::Closed || (secondaryEvent.type == Event::KeyPressed && secondaryEvent.key.code == Keyboard::Escape)) // Je¿eli podczas gry naciœniemy Escape albo Krzy¿yk to okno zostanie zamkniête.
			{
				secondWindow.close(); // Zamykamy okno
			}
		}

		Text pointsNow("Punkty: ", arial, 30);
		pointsNow.setPosition(SIZE_GRID, SIZE_GRID);

		secondWindow.draw(pointsNow);
		secondWindow.display();
	}
	return 0;
}