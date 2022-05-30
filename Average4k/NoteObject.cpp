#include "NoteObject.h"

#include "Gameplay.h"

#include <mutex>
#include "Judge.h"

#include "SongSelect.h"


void NoteObject::draw() {
    if (!drawCall)
        return;
    float position = rTime;

    Rect receptor;

    Object* obj = connectedReceptor;

    receptor.x = obj->x;
    receptor.y = obj->y;
    receptor.w = obj->w;
    receptor.h = obj->h;

    bpmSegment bruh = currentChart->getSegmentFromBeat(beat);

    float wh = currentChart->getTimeFromBeat(beat, bruh);

    float diff = (wh)-(position);

    float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;

    float noteOffset = (bps * (diff / 1000)) * (64 * size);

    bool downscroll = Game::save->GetBool("downscroll");

    if (downscroll)
        rect.y = (receptor.y - noteOffset);
    else
        rect.y = (receptor.y + noteOffset);
    x = obj->x;
    y = rect.y;

    Texture* texture;

    // get quant

    float beatRow = (beat - stopOffset) * 48;

    if (currentChart->meta.chartType == 1) // osu/quaver
    {
        float pos = (wh / 1000) - (bruh.startTime / 1000);
        float bps = 60 / bruh.bpm;

        beatRow = std::roundf(48 * (pos / bps));
    }
    if (fmod(beatRow, (192 / 4)) == 0)
        texture = Game::noteskin->fourth;
    else if (fmod(beatRow, (192 / 8)) == 0)
        texture = Game::noteskin->eighth;
    else if (fmod(beatRow, (192 / 12)) == 0)
        texture = Game::noteskin->twelfth;
    else if (fmod(beatRow, (192 / 16)) == 0)
        texture = Game::noteskin->sixteenth;
    else if (fmod(beatRow, (192 / 24)) == 0)
        texture = Game::noteskin->twelfth;
    else if (fmod(beatRow, (192 / 32)) == 0)
        texture = Game::noteskin->thirty2nd;
    else
        texture = Game::noteskin->none;

    Rect clipThingy;

    if (fboMode)
        clipThingy.x = 0;
    else
        clipThingy.x = fboX;

    clipThingy.y = rect.y + (32 * size);
    clipThingy.w = 64 * size;
    clipThingy.h = holdHeight * size;

    if (downscroll) {
        clipThingy.y -= holdHeight;
    }

    Rect dstRect;
    Rect srcRect;
    if (fboMode)
        dstRect.x = 0;
    else
        dstRect.x = fboX;
    dstRect.y = rect.y;
    dstRect.w = rect.w;
    dstRect.h = rect.h;
    dstRect.r = 255;
    dstRect.g = 255;
    dstRect.b = 255;
    dstRect.a = alpha;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = 1;
    srcRect.h = 1;

    //Rendering::SetClipRect(&clipThingy);

    if (holdsActive > 1)
    {

        for (int i = 0; i < heldTilings.size(); i++) {
            holdTile& tile = heldTilings[i];
            float time = currentChart->getTimeFromBeat(tile.beat, currentChart->getSegmentFromBeat(tile.beat));

            float diff2 = time - position;

            float offsetFromY = (bps * (diff2 / 1000)) * (64 * size);
            tile.rect.y = (receptor.y + (64 * size)) + offsetFromY;
            if (downscroll)
                tile.rect.y = (receptor.y - (64 * size)) - offsetFromY;

            dstRect.h = 65 * size;

            dstRect.y = tile.rect.y;

            if (i != heldTilings.size() - 1) {
                if (!downscroll)
                {
                    srcRect.h = -1;
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->hold, GL::genShader);
                    srcRect.h = 1;
                }
                else
                {
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->hold, GL::genShader);
                }
            }
            else {
                if (!downscroll)
                {
                    srcRect.h = -1;
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->holdend, GL::genShader);
                    srcRect.h = 1;
                }
                else
                {
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->holdend, GL::genShader);
                }
            }

        }
    }
    dstRect.h = rect.h;

    dstRect.y = rect.y;

    //Rendering::SetClipRect(NULL);

    int activeH = 0;

    for (int i = 0; i < heldTilings.size(); i++) {
        holdTile& tile = heldTilings[i];
        float time = currentChart->getTimeFromBeat(tile.beat, currentChart->getSegmentFromBeat(tile.beat));

        float diff2 = time - position;
        bool condition = diff2 > -Judge::hitWindows[4];

        if (heldTilings[i].active || condition)
            activeH++;
    }
    if (activeH != holdsActive)
        holdsActive = activeH;


    if (active) {
        if (Game::noteskin->rotate) {
            switch (lane) {
            case 0: // left
                Rendering::PushQuad(&dstRect, &srcRect, texture, GL::genShader, 90);
                break;
            case 1: // down
                Rendering::PushQuad(&dstRect, &srcRect, texture, GL::genShader);
                break;
            case 2: // up
                srcRect.h = -1;
                Rendering::PushQuad(&dstRect, &srcRect, texture, GL::genShader);
                srcRect.h = 1;
                break;
            case 3: // right
                Rendering::PushQuad(&dstRect, &srcRect, texture, GL::genShader, -90);
                break;
            }
        }
        else {
            Rendering::PushQuad(&dstRect, &srcRect, texture, GL::genShader);
        }

        if (selected)
        {
            dstRect.a = 0.6;
            Rendering::PushQuad(&dstRect, &srcRect, NULL, GL::genShader);
            dstRect.a = alpha;
        }
    }



}