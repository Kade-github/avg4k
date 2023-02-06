#pragma once
#include "includes.h"
#include "SMFile.h"
#include "MainMenu.h"
#include "ReceptorObject.h"
#include "Chart.h"
#include "AvgSprite.h"
#include "ArrowEffects.h"

struct holdBody {
	float x, y;
	float w, h;
	float skewTL = 0, skewTR = 0;
	float skewBL = 0, skewBR = 0;
	float skewYTL = 0, skewYTR = 0;
	float skewYBL = 0, skewYBR = 0;
	float beat;
	float deg = 0;

	bool wasHidden = false;

	

	std::vector<GL_Vertex> verts = {};
};


struct holdTile {
	float time;
	double beat;
	bool active;
	bool fucked;
	int index;
	SDL_FRect rect;

	float ogX, ogY;

	float modX, modY;

	float endX = 0;
	float endY = 0;

	bool operator<(const holdTile& a) const
	{
		return beat < a.beat;
	}
};

class NoteObject : public Object
{
	private:
		float animStartTime = 0;
		float animTime = 0;
	public:

		ArrowEffects* arrowEffects;

		bool blownUp = false;

		float drawAngle = 0;

		bpmSegment curSeg;

		bool xmod = false;

		float tempStopOffset = 0;
		float stopTime = -1;

		int frame;
		int fps = 24;

		bool loop;
		bool animationFinished;

		NoteObject();
		~NoteObject() {
		
			delete thingy;
		};

		Texture* holdTexture;

		bool holding;

		float fuckTimer;
		int holds;
		float holdstoppedbeat = -1;
		float holdstoppedtime = -1;
		float holdPerc;

		bool missHold;

		note* connected;
		Text* debugText;

		int modId = 0;
		float cmodOffset;

		AvgGroup* thingy;

		bool fboMode = true;
		bool drawCall = true;


		float calcCMod(float diff);
		float calcXMod(float diff, float bpm);
		Shader* customShader;

		Chart* currentChart;

		float time;

		bool selected = false;

		ReceptorObject* connectedReceptor;

		float stopOffset;

		int holdHeight;

		AvgFrame end;
		AvgFrame hold;

		float size;

		int holdsActive = 0;
		bool wasHit = false;
		bool clapped;

		float tailBeat = 0;

		bool debug = false;
		double beat = 0;
		float endTime = 0;
		float fboX = 0;
		double endBeat = 0;
		float rTime = 0;
		bool active = true;
		bool destroyed = false;
		int lane = 0;
		noteType type = noteType::Note_Normal;
		SDL_FRect rect;
		std::vector<holdTile> heldTilings;

		float reverse = 0;

		float endX = 0;
		float endY = 0;

		float modX = 0;
		float modY = 0;

		float cmod;

		float modCMOD;
		float endCMOD;

		virtual void draw();
};

