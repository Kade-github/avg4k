#include "StartMenu.h"
#include "SoundManager.h"
#include "TweenManager.h"
#include "MainMenu.h"
Channel* ch;

int steps = 0;
float dddtime = 0;

void StartMenu::create()
{
	// create misc and kadedev logos
	opgl = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, Noteskin::getStartElement(Game::noteskin, "MiscLogo.png"));
	kadedev = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, Noteskin::getStartElement(Game::noteskin, "KadeDevTeam.png"));

	opgl->x = (Game::gameWidth / 2) - (opgl->w / 2);
	opgl->y = (Game::gameHeight / 2) - (opgl->h / 2);

	kadedev->x = (Game::gameWidth / 2) - (kadedev->w / 2);
	kadedev->y = (Game::gameHeight / 2) - (kadedev->h / 2);

	// stuff
	opgl->alpha = 0;
	kadedev->alpha = 0;

	add(opgl);
	add(kadedev);

	// create and play start song
	ch = SoundManager::createChannel(Noteskin::getMusicElement(Game::noteskin, "MenuTheme.wav"), "prevSong");
	ch->play();

	ch->bpm = 155;

	Tweening::TweenManager::createNewTween("opglLogo", opgl, Tweening::tt_Alpha, 6000, 0, 1, NULL, Easing::EaseInSine);
}

void StartMenu::update(Events::updateEvent ev)
{
	dddtime += Game::deltaTime;
	float beat = ((dddtime) / 1000) * (ch->bpm / 60);

	// events

	switch ((int)beat)
	{
	case 16:
		if (steps == 0)
		{
			steps++;
			Tweening::TweenManager::createNewTween("opglLogo", opgl, Tweening::tt_Alpha, 750, 1, 0, NULL, Easing::EaseInSine);
			Tweening::TweenManager::createNewTween("kadeLogo", kadedev, Tweening::tt_Alpha, 6000, 0, 1, NULL, Easing::EaseInSine);
		}
		break;
	case 32:
		if (steps == 1)
		{
			steps++;
			ch->setPos(12038);
			Game::instance->transitionToMenu(new MainMenu());
		}
	}

}

void StartMenu::keyDown(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_RETURN && steps != 4)
	{
		steps = 4;
		ch->setPos(12038);
		Game::instance->transitionToMenu(new MainMenu());
	}
}
