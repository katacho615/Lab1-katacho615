#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
 
int main()
{
	sf::RenderWindow mainWindow(sf::VideoMode(800, 600, 32), "Pakman");
	sf::RenderWindow secondWindow;
	sf::Texture texture;
	if (!texture.loadFromFile("pakman.png"))
		return EXIT_FAILURE;
	sf::Sprite pakman;
	pakman.setTexture(texture);
	pakman.setPosition(800/2, 600/2);
	pakman.setScale(0.5, 0.5);
	pakman.setOrigin(64, 64);
	while(mainWindow.isOpen())
	{
		sf::Event somethingHappened;
		while (mainWindow.pollEvent(somethingHappened))
		{
			if (somethingHappened.type == sf::Event::Closed || (somethingHappened.type ==sf::Event::KeyPressed && somethingHappened.key.code == sf::Keyboard::Escape)) 
			{
				mainWindow.close();
			}
			if (somethingHappened.type == sf::Event::MouseButtonPressed && somethingHappened.mouseButton.button == sf::Mouse::Left)
			{
				secondWindow.create(sf::VideoMode(240, 100, 32), "You sure?");
			}
			if (somethingHappened.type == sf::Event::KeyPressed && somethingHappened.key.code == sf::Keyboard::Left)
			{
				pakman.setRotation(180);
				pakman.move(-20, 0);
			}
			if (somethingHappened.type == sf::Event::KeyPressed && somethingHappened.key.code == sf::Keyboard::Right)
			{
				pakman.setRotation(0);
				pakman.move(20, 0);
			}
			if (somethingHappened.type == sf::Event::KeyPressed && somethingHappened.key.code == sf::Keyboard::Up)
			{
				pakman.setRotation(270);
				pakman.move(0, -20);
			}
			if (somethingHappened.type == sf::Event::KeyPressed && somethingHappened.key.code == sf::Keyboard::Down)
			{
				pakman.setRotation(90);
				pakman.move(0, 20);
			}
		}

		mainWindow.clear();
		mainWindow.draw(pakman);
		mainWindow.display();
	}
	return 0;
}