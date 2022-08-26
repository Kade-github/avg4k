#include "NoteObject.h"

#include "Gameplay.h"

#include <mutex>
#include "Judge.h"

#include "SongSelect.h"


void NoteObject::draw() {
    if (!drawCall)
        return;

    MUTATE_START
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

    Rect dstRect;
    Rect srcRect;
    dstRect.x = obj->x;
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

    Shader* sh = customShader;

    if (!customShader)
        sh = GL::genShader;

    int activeH = 0;

    for (int i = 0; i < heldTilings.size(); i++) {
        holdTile& tile = heldTilings[i];

        if (tile.active)
            activeH++;
    }
    if (activeH != holdsActive)
    {
        holdsActive = activeH;
        //std::cout << "holds active " << holdsActive << std::endl;
    }


        for (int i = 0; i < heldTilings.size(); i++) {
            holdTile& tile = heldTilings[i];
            float time = currentChart->getTimeFromBeat(tile.beat, currentChart->getSegmentFromBeat(tile.beat));

            float diff2 = time - position;

            float offsetFromY = ((bps * (diff2 / 1000)) * (64 * size)) - (32 * size);
            Rect r;
            r.x = obj->x;
            r.y = obj->y + (obj->h / 2);
            r.w = 64 * size;
            r.h = Game::gameHeight;

            tile.rect.y = (receptor.y + (64 * size)) + offsetFromY;
            if (downscroll)
            {
                tile.rect.y = (receptor.y - (64 * size)) - offsetFromY;
                if (!tile.active)
                {
                    r.y = 0;
                    r.h = obj->y + (obj->h / 2);
                    Rendering::SetClipRect(&r);
                }
            }
            else
            {
                if (!tile.active)
                {
                    Rendering::SetClipRect(&r);
                }
            }
            

            dstRect.h = 65 * size;

            dstRect.y = tile.rect.y;

            if (i != heldTilings.size() - 1) {
                if (!downscroll)
                {
                    srcRect.h = -1;
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->hold, sh);
                    srcRect.h = 1;
                }
                else
                {
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->hold, sh);
                }
            }
            else {
                if (!downscroll)
                {
                    srcRect.h = -1;
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->holdend, sh);
                    srcRect.h = 1;
                }
                else
                {
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->holdend, sh);
                }
            }
            Rendering::SetClipRect(NULL);
        }
    dstRect.h = rect.h;

    dstRect.y = rect.y;

    //Rendering::SetClipRect(NULL);


    if (active) {
        if (Game::noteskin->rotate) {
            switch (lane) {
            case 0: // left
                Rendering::PushQuad(&dstRect, &srcRect, texture, sh, 90);
                break;
            case 1: // down
                Rendering::PushQuad(&dstRect, &srcRect, texture, sh);
                break;
            case 2: // up
                srcRect.h = -1;
                Rendering::PushQuad(&dstRect, &srcRect, texture, sh);
                srcRect.h = 1;
                break;
            case 3: // right
                Rendering::PushQuad(&dstRect, &srcRect, texture, sh, -90);
                break;
            }
        }
        else {
            Rendering::PushQuad(&dstRect, &srcRect, texture, sh);
        }

        if (selected)
        {
            dstRect.a = 0.6;
            Rendering::PushQuad(&dstRect, &srcRect, NULL, sh);
            dstRect.a = alpha;
        }
    }

    MUTATE_END
}