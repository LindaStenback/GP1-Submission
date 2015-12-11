#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cFontMgr.h"
#include "cSprite.h"
#include "asteroidsGame.h"
#include "cButton.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;
	
	//When removed aliens reach 0 (will create GAME OVER)
	int removedaliens = 0;


    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	//The example OpenGL code
    windowOGL theOGLWnd;
	
    //Attach our the OpenGL window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);

    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	/* initialize random seed: */
	srand((unsigned int)time(NULL));

	// Create vector array of textures
	LPCSTR texturesToUse[] = { "Images\\Alien1.png", "Images\\Alien2.png", "Images\\Alien3.png", "Images\\Alien4.png", "Images\\Laserbullet.png" };
	for (int tCount = 0; tCount < 5; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(texturesToUse[tCount]);
	}

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/PixellandThemeSong.wav", "Audio/Laser_Shoot.wav", "Audio/ExplosionAlienAndSpace.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[3] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/minecraft.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);
	theFontMgr->addFont("minecraft", gameFonts[2], 24);

	// Create vector array of textures

	for (int astro = 0; astro < 20; astro++) //How many aliens in the game
	{
		//sets the movement of the asteroids and renders the texture
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos(glm::vec2(windowWidth / (rand() % 5 + 1), 100.0f + (rand() % 5 + 1))); //changes where the aliens spawn
		theAsteroids[astro]->setSpriteTranslation(glm::vec2(0.0f, 10.0f)); //changes the speed of the enemies
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
		theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
		theAsteroids[astro]->setSpriteCentre();
		theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
		theAsteroids[astro]->setActive(true);
		theAsteroids[astro]->setMdlRadius();
	}

	//Change the background
	cTexture textureBkgd;
	textureBkgd.createTexture("Images\\PinkBackground.png");
	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgd.getTexture());
	spriteBkgd.setTextureDimensions(textureBkgd.getTWidth(), textureBkgd.getTHeight());

	//Change the Rocket sprite
	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\Rocket.png");
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); // Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(512.0f, 670.0f)); //Position of the sprite
	rocketSprite.setTexture(rocketTxt.getTexture());
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth(), rocketTxt.getTHeight());
	rocketSprite.setSpriteCentre();
	rocketSprite.setRocketVelocity(glm::vec2(0.0f, 0.0f));

	// Attach sound manager to rocket sprite
	rocketSprite.attachSoundMgr(theSoundMgr);

	// Create vector array of textures
	vector<cTexture*> textureBkgList;
	LPCSTR bkgTexturesToUse[] = { "Images/IntroBackgroundwithTitle.png", "Images/Instructionsbackground.png", "Images/Pinkbackground.png", "Images/BackgroundEND.png" };
	for (int tCount = 0; tCount < 4; tCount++)
	{
		textureBkgList.push_back(new cTexture());
		textureBkgList[tCount]->createTexture(bkgTexturesToUse[tCount]);
	}

	//Sets the backgrounds for the menus

	//Start menu bkgd
	cBkGround spriteStartBkgd;
	spriteStartBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStartBkgd.setTexture(textureBkgList[0]->getTexture());
	spriteStartBkgd.setTextureDimensions(textureBkgList[0]->getTWidth(), textureBkgList[0]->getTHeight());
	
	//Instruction menu bkgd
	cBkGround spriteInstructionBkgd;
	spriteInstructionBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteInstructionBkgd.setTexture(textureBkgList[1]->getTexture());
	spriteInstructionBkgd.setTextureDimensions(textureBkgList[1]->getTWidth(), textureBkgList[1]->getTHeight());

	//End menu bkgd
	cBkGround spriteEndBkgd;
	spriteEndBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteEndBkgd.setTexture(textureBkgList[3]->getTexture());
	spriteEndBkgd.setTextureDimensions(textureBkgList[3]->getTWidth(), textureBkgList[3]->getTHeight());

	// Create vector array of button textures
	vector<cTexture*> btnTextureList;
	LPCSTR btnTexturesToUse[] = { "Images/Buttons/Exitbutton.png", "Images/Buttons/Helpbutton.png", "Images/Buttons/GoBackbutton.png", "Images/Buttons/Playbutton.png", "Images/Buttons/Controlbutton.png", "Images/Buttons/Replaybutton.png" };
	for (int tCount = 0; tCount < 6; tCount++)
	{
		btnTextureList.push_back(new cTexture());
		btnTextureList[tCount]->createTexture(btnTexturesToUse[tCount]);
	}

	//Sets the texture for the buttons

	//Instruction button
	cButton instructionsButton;
	instructionsButton.attachInputMgr(theInputMgr);
	instructionsButton.setTexture(btnTextureList[1]->getTexture());
	instructionsButton.setTextureDimensions(btnTextureList[1]->getTWidth(), btnTextureList[1]->getTHeight());

	//Exit button
	cButton exitButton;
	exitButton.attachInputMgr(theInputMgr);
	exitButton.setTexture(btnTextureList[0]->getTexture());
	exitButton.setTextureDimensions(btnTextureList[0]->getTWidth(), btnTextureList[0]->getTHeight());

	//Playbutton
	cButton playButton;
	playButton.attachInputMgr(theInputMgr);
	playButton.setTexture(btnTextureList[3]->getTexture());
	playButton.setTextureDimensions(btnTextureList[3]->getTWidth(), btnTextureList[3]->getTHeight());

	//Go back/return button
	cButton gobackButton;
	gobackButton.attachInputMgr(theInputMgr);
	gobackButton.setTexture(btnTextureList[2]->getTexture());
	gobackButton.setTextureDimensions(btnTextureList[2]->getTWidth(), btnTextureList[2]->getTHeight());

	//Puts out a text of your own choice
	string outputMsg;
	string strMsg[] = { "", "", "", "", "" };

	//Defines different states in the game and allows us to call on them
	gameState theGameState = MENU;
	btnTypes theBtnType = EXIT;
	bool  respawn = false;
	string strScore = "";
	
    //This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); //Process any window events

        //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Menu Structure
		switch (theGameState)
		{
		case MENU:
			//renders start bkgd
			spriteStartBkgd.render();

			// render button and reset clicked to false
			playButton.setSpritePos(glm::vec2(375.0f, 300.0f));
			instructionsButton.setSpritePos(glm::vec2(375.0f, 375.0f));
			exitButton.setSpritePos(glm::vec2(375.0f, 450.0f));
			playButton.render();
			instructionsButton.render();
			exitButton.render();

			//When pressing playbutton/instruction button PLAYING and INSTRUCTIONS will update the gamestate
			theGameState = playButton.update(theGameState, PLAYING);
			theGameState = instructionsButton.update(theGameState, INSTRUCTIONS);
			exitButton.update();

			//puts out a message in a specific font
			//outputMsg = strMsg[2];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));
			//outputMsg = strMsg[0];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(100, 100, 0.0f));
			//outputMsg = strMsg[1];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(100, 150, 0.0f));
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
			break;
		case INSTRUCTIONS:
			spriteInstructionBkgd.render();

			// render button and reset clicked to false
			playButton.setSpritePos(glm::vec2(750.0f, 680.0f));
			gobackButton.setSpritePos(glm::vec2(30.0f, 680.0f));
			playButton.render();
			gobackButton.render();

			//When pressing playbutton/instruction button PLAYING and INSTRUCTIONS will update the gamestate
			theGameState = playButton.update(theGameState, PLAYING);
			theGameState = gobackButton.update(theGameState, MENU);

			//puts out a message in a specific font
			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));

			break;
		case PLAYING:
			spriteBkgd.render(); //renders sprite bkgd
			if (respawn)
			{
				if (theAsteroids.size() > 0)
				{
					theAsteroids.clear();
				}
				for (int astro = 0; astro < 20; astro++) //How many aliens in the game
				{
					//sets the movement of the asteroids and renders the texture
					theAsteroids.push_back(new cAsteroid);
					theAsteroids[astro]->setSpritePos(glm::vec2(windowWidth / (rand() % 5 + 1), 100.0f / (rand() % 5 + 1)));
					theAsteroids[astro]->setSpriteTranslation(glm::vec2(0.0f, 25.0f)); //changes the speed of the enemies
					int randAsteroid = rand() % 4;
					theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
					theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
					theAsteroids[astro]->setSpriteCentre();
					theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
					theAsteroids[astro]->setActive(true);
					theAsteroids[astro]->setMdlRadius();
				}
				respawn = false;
			}

			rocketSprite.update(elapsedTime);
			
			{
				//when to spawn asteroids and when to remove them
				vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
				while (asteroidIterator != theAsteroids.end())
				{
					if ((*asteroidIterator)->isActive() == false)
					{
						asteroidIterator = theAsteroids.erase(asteroidIterator);
						//Removed aliens gets added to the amount of eliminated enemies (for it to become GAME OVER)
						removedaliens++;
					}
					else
					{
						(*asteroidIterator)->update(elapsedTime);
						(*asteroidIterator)->render();
						++asteroidIterator;
					}
				}
			}

			//If 20 aliens removed then the END gamestate will be called
			if (removedaliens == 20)
			{
				theGameState = (theGameState, END);
			}

			rocketSprite.render();
			//theFontMgr->getFont("minecraft")->printText("SPAAAAACE !!!", FTPoint(0.0f, -1.0f, 0.0f));

			// render button and reset clicked to false
			exitButton.setSpritePos(glm::vec2(750.0f, 20.0f));
			exitButton.render();
			theGameState = exitButton.update(theGameState, END);

			outputMsg = strMsg[2];
			strScore = to_string(score);
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));
			theFontMgr->getFont("minecraft")->printText(strScore.c_str(), FTPoint(500, 35, 0.0f));

			break;
		case END:
			spriteEndBkgd.render(); //renders End bkgd

			// render button and reset clicked to false
			playButton.setClicked(false);
			exitButton.setClicked(false);

			//Sets sprite for the buttons and their placement
			playButton.setSpritePos(glm::vec2(375.0f, 475.0f));
			exitButton.setSpritePos(glm::vec2(375.0f, 555.0f));
			playButton.render();
			exitButton.render();

			//when buttons clicked the game updates into either PLAYING or ends the game
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update();

			//puts out a message in a specific font
			//outputMsg = strMsg[2];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));
			//outputMsg = strMsg[3];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(100, 100, 0.0f));
			//outputMsg = strMsg[4];
			//theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(100, 150, 0.0f));

			//if exit button is clicked the game closes and returns it to its original state
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
			if (playButton.getClicked())
			{
				respawn = true;
				score = 0;
				removedaliens = 0;
			}
			break;
		}

		pgmWNDMgr->swapBuffers();
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);
    }

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
