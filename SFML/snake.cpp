#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Audio.hpp>
#include "math.h"

using namespace sf;

enum class direction { LEFT, RIGHT, UP, DOWN };
direction dir = direction::LEFT;
direction dirConfirm = direction::LEFT;

int main()
{
	/*
	RESOLUTION OPTIONS
	*/
	const int WIDTH = 800;
	const int HEIGHT = 600;
	const bool FULLSCREEN = false;

	/*
	SIZE OF THE MAP 
	INCLUDES BORDERS
	RECOMMENDED MINIMUM: X=10, Y=10
	RECOMMENDED MAXIMUM: X=40, Y=30
	*/
	const int XMAPSIZE = 24;
	const int YMAPSIZE = 18;

	/*
	SPEED OPTIONS
	*/
	float STEPTIME =.3f; // Start Speed
	const bool CHANGESPEED = true; //Allow Changing Speed
	const float FASTER = 1.05; //Speed acceleration -- 1.05 = +105%




	int map[XMAPSIZE][YMAPSIZE];
	VideoMode vm(WIDTH, HEIGHT);
	RenderWindow window(vm, "Boa++", (FULLSCREEN) ? Style::Fullscreen : true);
	float minSize = fmin(WIDTH / XMAPSIZE,HEIGHT/YMAPSIZE);


	
	Texture headTex;
	headTex.loadFromFile("graphics/snakeHeadUp.png");
	headTex.setSmooth(true);
	Texture meatTex;
	meatTex.loadFromFile("graphics/meat.png");
	meatTex.setSmooth(true);

	Texture tileTex;
	tileTex.loadFromFile("graphics/tile.bmp");
	Sprite tile[XMAPSIZE*YMAPSIZE];

	for (int a = 0; a < XMAPSIZE; a++)
	{
		for (int b = 0; b < YMAPSIZE; b++)
		{
			tile[a + b * XMAPSIZE].setTexture(tileTex);
			tile[a + b * XMAPSIZE].setPosition(a*minSize+ minSize/2, b*minSize+ minSize/2);
			tile[a + b * XMAPSIZE].setScale(Vector2f(minSize / 32, minSize / 32));
			Vector2f scale = tile[a + b * XMAPSIZE].getScale();
			tile[a + b * XMAPSIZE].setOrigin(Vector2f(16,16));
			if (a == 0 || b == 0 || a == XMAPSIZE - 1 || b == YMAPSIZE - 1)
			{
				map[a][b] = 1;
				tile[a + b * XMAPSIZE].setColor(Color::Red);
			}
			else
			{
				map[a][b] = 0;
				tile[a + b * XMAPSIZE].setColor(Color::Blue);
			}
		}
	}

	Vector2i snake[XMAPSIZE*YMAPSIZE];
	Vector2i position = Vector2i(XMAPSIZE/2, YMAPSIZE/2);
	snake[0].x = position.x;
	snake[0].y = position.y;
	map[snake[0].x][snake[0].y] = 4;


	Clock clock;

	SoundBuffer moveBuffer;
	moveBuffer.loadFromFile("sound/move.wav");
	Sound move;
	move.setBuffer(moveBuffer);

	SoundBuffer exploBuffer;
	exploBuffer.loadFromFile("sound/explo.wav");
	Sound explo;
	explo.setBuffer(exploBuffer);

	SoundBuffer pickBuffer;
	pickBuffer.loadFromFile("sound/pick.wav");
	Sound pick;
	pick.setBuffer(pickBuffer);


	Vector2i meal = Vector2i(XMAPSIZE/2 + 1, YMAPSIZE/2 + 1);
	map[meal.x][meal.y] = 3;

	while (window.isOpen())
	{


		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Left) && dirConfirm!= direction::RIGHT)
		{
			dir = direction::LEFT;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right) && dirConfirm != direction::LEFT)
		{
			dir = direction::RIGHT;
		}
		if (Keyboard::isKeyPressed(Keyboard::Up) && dirConfirm != direction::DOWN)
		{
			dir = direction::UP;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down) && dirConfirm != direction::UP)
		{
			dir = direction::DOWN;
		}

		/////////////////////////////////////
		//function Update
		Time dt = clock.getElapsedTime();

		if (STEPTIME < dt.asSeconds())
		{
			dirConfirm = dir;
			clock.restart();
			move.play();

			if (dir == direction::LEFT)
				position.x--;
			else if(dir == direction::RIGHT)
				position.x++;
			else if (dir == direction::UP)
				position.y--;
			else
				position.y++;

			bool meat=false;

			//Game Over
			if (map[position.x][position.y] == 1 || map[position.x][position.y] == 2)
			{
				explo.play();

				window.close();
			}
			//Meal
			else if (map[position.x][position.y] == 3)
			{
				meat = true;
				STEPTIME /= FASTER;
				pick.play();
			}


			Vector2i tmp;
			Vector2i tmp2;
			for (int a = 0; a < (XMAPSIZE - 1)*(YMAPSIZE - 1); a++)
			{
				if (a == 0)
				{
					tmp = snake[0];
					map[snake[0].x][snake[0].y] = 0;
					snake[0].x = position.x;
					snake[0].y = position.y;
					map[snake[0].x][snake[0].y] = 4;
				}
				else if (snake[a].x != 0)
				{
					map[snake[a].x][snake[a].y] = 0;
					tmp2 = snake[a];
					snake[a] = tmp;
					tmp = tmp2;
					map[snake[a].x][snake[a].y] = 2;
				}
				else if (meat)
				{
					map[snake[a].x][snake[a].y] = 0;
					snake[a] = tmp;
					map[snake[a].x][snake[a].y] = 2;
					meat = false;
					//new meal
					Vector2i freePos[(XMAPSIZE-1)*(YMAPSIZE-1)];
					int freePosCounter = 0;

					for (int q = 0; q < XMAPSIZE; q++)
					{
						for (int w = 0; w < YMAPSIZE; w++)
						{
							if (map[q][w] == 0)
							{
								freePos[freePosCounter].x = q;
								freePos[freePosCounter].y = w;
								freePosCounter++;
							}
						}
					}

					std::vector<Vector2i> freePosCut(freePosCounter);

					for (int i = 0; i < freePosCounter; i++)
					{
						freePosCut[i] = freePos[i];
					}

					srand(time(NULL));
					int r = rand() % freePosCounter;
					meal = Vector2i(freePosCut[r].x, freePosCut[r].y);
					map[meal.x][meal.y] = 3;
					////////////////////////////////////////////////////
				}
				else
					break;
			}
			
			
		}
		/////////////////////////////////////

		for (int a = 1; a < XMAPSIZE - 1; a++)
		{
			for (int b = 1; b < YMAPSIZE - 1; b++)
			{
				if (map[a][b] == 0)//empty
				{
					tile[a + b * XMAPSIZE].setTexture(tileTex);
					tile[a + b * XMAPSIZE].setColor(Color::Blue);
				}
				else if (map[a][b] == 2)//snake
				{
					tile[a + b * XMAPSIZE].setColor(Color::Green);
					tile[a + b * XMAPSIZE].setTexture(tileTex);
				}
				else if (map[a][b] == 4)//snake head
				{
					tile[a + b * XMAPSIZE].setColor(Color(255,255,255,255));
					tile[a + b * XMAPSIZE].setTexture(headTex);
					if (dirConfirm == direction::LEFT)
						tile[a + b * XMAPSIZE].setRotation(270);
					else if (dirConfirm == direction::DOWN)
						tile[a + b * XMAPSIZE].setRotation(180);					
					else if (dirConfirm == direction::RIGHT)
						tile[a + b * XMAPSIZE].setRotation(90);					
					else
						tile[a + b * XMAPSIZE].setRotation(0);
				}
				else//meat
				{
					tile[a + b * XMAPSIZE].setTexture(meatTex);
					tile[a + b * XMAPSIZE].setColor(Color(255, 255, 255, 255));
				}
			}
		}


		window.clear();

		for (int a = 0; a < XMAPSIZE; a++)
		{
			for (int b = 0; b < YMAPSIZE; b++)
			{
				window.draw(tile[a + b * XMAPSIZE]);
			}
		}

		window.display();
	}
	sleep(milliseconds(400));
	return 0;
}