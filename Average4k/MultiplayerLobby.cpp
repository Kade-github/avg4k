#include "MultiplayerLobby.h"
#include "AvgSprite.h"

bool MultiplayerLobby::inLobby = false;
lobby MultiplayerLobby::CurrentLobby;
bool MultiplayerLobby::isHost = false;


void MultiplayerLobby::refreshLobby(lobby l)
{
	MUTATE_START
	CurrentLobby = l;
	inLobby = true;

	for (person p : people)
	{
		removeObj(p.display);
		delete p.avatar;
	}

	people.clear();

	for (int i = 0; i < l.PlayerList.size(); i++)
	{
		player& p = l.PlayerList[i];
		person per;
		per.display = new Text(82, 192 + (46 * i), p.Name, 24, "NotoSans-Regular");
		per.display->create();
		Texture* t = Steam::getAvatar(p.AvatarURL.c_str());
		AvgSprite* spr = new AvgSprite(0, 0, t);
		if (t)
			per.avatar = spr;
		else
			per.avatar = NULL;
		add(per.display);
		add(spr);
		people.push_back(per);
	}

	for (person& p : people)
	{
		Color c;
		c.r = 255;
		c.g = 128;
		c.b = 128;
		p.display->color = c;
		p.display->setText(p.display->text);
	}

	helpDisplay->setText("Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ") " + (isHost ? "You are the host!" : ""));
	//warningDisplay->setText("");
	MUTATE_END
}

void MultiplayerLobby::onSteam(std::string s) {
	VM_START
	if (s == "chartAquired")
	{
		SongSelect::currentChart = Game::steam->downloadedChart;
		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 0;

		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
	}
	VM_END
}

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	if (!this)
		return;
	VM_START
	//MUTATE_START
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketUpdateLobbyChart cc;
	SPacketStatus f;
	CPacketHostStartGame start;
	msgpack::unpacked result;

	msgpack::object obj;
	switch (pt)
	{
	case eSPacketStatus:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(f);
		switch (f.code)
		{
		case 803:
			// we host pog
			std::cout << "host me" << std::endl;
			isHost = true;
			refreshLobby(CurrentLobby);
			break;
		case 1337:
			Color c;
			c.r = 0;
			c.g = 255;
			c.b = 0;

			if (!isHost)
				return;

			warningDisplay->color = c;
			warningDisplay->setText("Everyone has the chart, you can now start. (Press enter to start, LSHIFT to reselect chart)");
			break;
		case 8876:
			Color cc;
			cc.r = 255;
			cc.g = 0;
			cc.b = 0;
			warningDisplay->color = cc;

			if (!isHost)
				warningDisplay->setText("Unable to start, some players do not have the chart! " + std::string((SongSelect::currentChart != NULL ? "You have it!" : "You do not have it!")));
			else
				warningDisplay->setText("Unable to start, some players do not have the chart! press enter to start when they do, press shift to reselect.");
			break;
		}

		if (f.code >= 9000)
		{
			std::cout << "got status for " << f.code << std::endl;
			int p = f.code - 9000;
			if (p < people.size())
			{
				std::cout << "player " << p << " is now green!" << std::endl;

				Color c;
				c.r = 128;
				c.g = 255;
				c.b = 128;

				person& pe = people[p];

				pe.display->color = c;
				pe.display->setText(pe.display->text);
			}
		}

		break;
	case eSPacketUpdateLobbyData:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(update);

		std::cout << "refresh lobby" << std::endl;

		refreshLobby(update.Lobby);
		break;
	case eSPacketWtfAmInReply:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(reply);

		std::cout << "refresh lobby" << std::endl;

		isHost = reply.isHost;

		refreshLobby(reply.Lobby);
		break;
	case eSPacketUpdateLobbyChart:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(cc);

		SongSelect::selectedDiffIndex = cc.diff;

		Game::steam->LoadWorkshopChart((uint64_t)cc.chartID);

		for (person& p : people)
		{
			Color c;
			c.r = 255;
			c.g = 128;
			c.b = 128;
			p.display->color = c;
			p.display->setText(p.display->text);
		}

		// tell the server we aint got it lol (if we dont :))
		break;
	case eSPacketStartLobbyGame:
		std::cout << "start!" << std::endl;
		
		Game::instance->transitionToMenu(new Gameplay());
		SongSelect::currentChart = Game::steam->downloadedChart;


		for (person p : people)
		{
			removeObj(p.display);
			delete p.avatar;
		}

		people.clear();
		break;
	}
	VM_END
	//MUTATE_END
	
}

MultiplayerLobby::MultiplayerLobby(lobby l, bool hosted, bool backFromSelect = false)
{
	VM_START
	waitingForStart = backFromSelect && hosted;
	isHost = hosted;
	CurrentLobby = l;
	VM_END
}

void MultiplayerLobby::create() {
	addCamera(Game::mainCamera);

	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
	add(sprite);
	AvgRect* rect = new AvgRect(0, 0, 1280, 720);
	rect->alpha = 0.3;
	add(rect);

	helpDisplay = new Text(24, 100, "Lobby: " + CurrentLobby.LobbyName + " (" + std::to_string(CurrentLobby.Players) + "/" + std::to_string(CurrentLobby.MaxPlayers) + ")", 24, "NotoSans-Regular");
	helpDisplay->create();
	add(helpDisplay);

	warningDisplay = new Text(24, 125, "", 16, "NotoSans-Regular");
	Color c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	warningDisplay->color = c;
	warningDisplay->create();
	add(warningDisplay);



	CPacketWtfAmIn fuck;
	fuck.Order = 0;
	fuck.PacketType = eCPacketWtfAmIn;

	Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);

	refreshLobby(CurrentLobby);

	if (waitingForStart)
	{
		CPacketHostChangeChart chart;
		chart.chartID = (uint64_t)SongSelect::selectedSong->steamHandle;
		std::cout << "telling the server to start " << SongSelect::selectedSong->steamHandle << std::endl;
		chart.diff = SongSelect::selectedDiffIndex;
		chart.Order = 0;
		chart.PacketType = eCPacketHostChangeChart;
		Multiplayer::sendMessage<CPacketHostChangeChart>(chart);

		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 1;

		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
	}
	created = true;
}

void MultiplayerLobby::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START
	CPacketHostStartGame start;
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			CPacketLeave leave;
			leave.Order = 0;
			leave.PacketType = eCPacketLeave;

			Multiplayer::sendMessage<CPacketLeave>(leave);

			Game::instance->transitionToMenu(new MultiplayerLobbies());

			for (person p : people)
			{
				removeObj(p.display);
				delete p.avatar;
			}
			people.clear();
			inLobby = false;
			break;
		case SDLK_RETURN:
			if (!isHost)
				return;

			if (waitingForStart)
			{
				start.Order = 0;
				start.PacketType = eCPacketHostStartGame;

				Multiplayer::sendMessage<CPacketHostStartGame>(start);
			}
			else
			{

				Game::instance->transitionToMenu(new SongSelect());


				for (person p : people)
				{
					removeObj(p.display);
					delete p.avatar;
				}

				people.clear();

			}
			break;
		case SDLK_LSHIFT:
			if (!isHost && !waitingForStart)
				return;
			Game::instance->transitionToMenu(new SongSelect());


			for (person p : people)
			{
				removeObj(p.display);
				delete p.avatar;
			}

			people.clear();

			break;
	}
	MUTATE_END
}

void MultiplayerLobby::update(Events::updateEvent event)
{
	
}

void MultiplayerLobby::postUpdate(Events::updateEvent event)
{
	MUTATE_START
	for (person& p : people)
	{
		AvgSprite* spr = p.avatar;
		spr->x = p.display->x - 58;
		spr->y = p.display->y - 8;
		spr->w = 46;
		spr->h = 46;
	}
	MUTATE_END
}