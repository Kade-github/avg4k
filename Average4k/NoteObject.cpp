#include "NoteObject.h"

#include "Gameplay.h"

#include <mutex>
#include "Judge.h"

#include "SongSelect.h"
#include "Helpers.h"
#include "ArrowEffects.h"

struct holdBody {
    float x, y;
    float w, h;
    float skewTL = 0, skewTR = 0;
    float skewBL = 0, skewBR = 0;
    float beat;
};

NoteObject::NoteObject()
{
    setX(-400);
    setY(-400);
    w = 64;
    h = 64;
    Object::currentId++;
    id = Object::currentId;
    cmod = Game::save->GetDouble("scrollspeed");

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
        {
            Gameplay::instance->currentModId++;
            modId = Gameplay::instance->currentModId;
            modY = y;
            modX = x;
        }
}

float NoteObject::calcCMod(float diff)
{

    float bps = (cmod / 60) / Gameplay::rate;

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
            bps = (modCMOD / 60) / Gameplay::rate;

    float noteOffset = (bps * (diff / 1000)) * (64 * size);

    return noteOffset;
}

void NoteObject::draw() {
    if (!drawCall)
        return;

    MUTATE_START
    float position = rTime;

    float currentBeat = currentChart->getBeatFromTime(position, currentChart->getSegmentFromTime(position));

    Rect receptor;

    ReceptorObject* obj = connectedReceptor;

    receptor.x = obj->x;
    receptor.y = obj->y;
    receptor.w = obj->w;
    receptor.h = obj->h;

    bpmSegment bruh = currentChart->getSegmentFromBeat(beat);

    float wh = currentChart->getTimeFromBeat(beat, bruh);

    float diff = (wh)-(rTime);

    float noteOffset = calcCMod(diff);

    cmodOffset = noteOffset;

    bool downscroll = Game::save->GetBool("downscroll");

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
        {
            if (ArrowEffects::reverse[lane] >= 0.5)
                downscroll = true;
            else
                downscroll = false;
        }

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
        {
            receptor.x = obj->modX;
            receptor.y = obj->modY;
        }

    if (downscroll)
        rect.y = (receptor.y - noteOffset);
    else
        rect.y = (receptor.y + noteOffset);

    x = obj->x;
    y = rect.y;


    Rect dstRect;
    Rect srcRect;
    dstRect.x = x;
    dstRect.y = y;
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

    //Rendering::SetClipRect(&clipThingy);

    Shader* sh = customShader;

    if (!customShader)
        sh = GL::genShader;

    int activeH = 0;


    
    dstRect.h = rect.h;

    dstRect.x = x;
    dstRect.y = y;

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
        {
            ArrowEffects::Arrow a = ArrowEffects::finishEffects(obj->x, obj->y, noteOffset, obj->type, -1, position, obj->modY);
            dstRect.x = a.x;
            dstRect.y = a.y;
        }


    Rect line;
    line.r = 255;
    line.g = 255;
    line.b = 255;

    if (endBeat != -1 && !missHold)
    {

        float ewh = currentChart->getTimeFromBeat(endBeat, bruh);

        float difff = (ewh)-(wh);

        float endPos = calcCMod(difff);

        line.h = endPos;
        line.w = 4;
        line.x = x + (32 * size);
        line.y = dstRect.y + (32 * size);
        if (downscroll)
            line.y = dstRect.y - (32 * size);

        //Rendering::PushQuad(&line, &srcRect, NULL, NULL);

        int amountToDraw = 1 + (endPos / (64 * size));

        int lastSkewStart;
        int lastSkewEnd;

        std::vector<holdBody> bodies;


        for (int i = 0; i < amountToDraw; i++)
        {
            Rect square;
            square.w = 64 * size;
            square.h = 64 * size;
            square.x = x;
            square.y = line.y + ((64 * size) * i);
            if (downscroll)
                square.y = line.y - ((64 * size) * i);

            float perc = (float)(i + 1.f) / (float)amountToDraw;

            float holdBeat = beat + ((endBeat - beat) * perc);
            if (holdBeat < currentBeat)
            {
                holds++;
                holdPerc = holds / amountToDraw;
            }
 

                holdBody body;
                body.beat = holdBeat;
                if (Gameplay::instance != NULL)
                    if (Gameplay::instance->runModStuff)
                    {
                        float ttime = currentChart->getTimeFromBeat(holdBeat, currentChart->getSegmentFromBeat(holdBeat));
                        float cmodHold = calcCMod(ttime - position);
                        ArrowEffects::Arrow a = ArrowEffects::finishEffects(obj->x, obj->y, cmodHold, obj->type, time, position, obj->modY);
                        square.x = a.x;
                        square.y = a.y;

                        if (bodies.size() == 0)
                        {
                            if (!downscroll)
                            {
                                if (beat < currentBeat)
                                {
                                    body.skewTL = -(square.x - obj->modX);
                                    body.skewTR = -((square.x + square.w) - (obj->modX + dstRect.w));
                                }
                                else
                                {
                                    body.skewTL = -(square.x - dstRect.x);
                                    body.skewTR = -((square.x + square.w) - (dstRect.x + dstRect.w));
                                }
                            }
                            else
                            {
                                if (beat < currentBeat)
                                {
                                    body.skewTL = (square.x - obj->modX);
                                    body.skewTR = ((square.x + square.w) - (obj->modX + dstRect.w));
                                }
                                else
                                {
                                    body.skewTL = (square.x - dstRect.x);
                                    body.skewTR = ((square.x + square.w) - (dstRect.x + dstRect.w));
                                }
                            }
                        }
                        else
                        {
                            holdBody lastBody = bodies.back();
                            if (!downscroll)
                            {
                                body.skewTL = -(square.x - lastBody.x);
                                body.skewTR = -((square.x + square.w) - (lastBody.x + lastBody.w));
                            }
                            else
                            {
                                body.skewTL = (square.x - lastBody.x);
                                body.skewTR = ((square.x + square.w) - (lastBody.x + lastBody.w));
                            }
                        }
                    }

                body.x = square.x;
                body.y = square.y;
                body.w = square.w;
                body.h = square.h;
                bodies.push_back(body);
        }
        int i = 0;
        for (holdBody body : bodies)
        {
            std::vector<GL_Vertex> verts;

            verts.push_back({ body.x + body.skewTL, body.y,
                0, 0,
                1.f,1.f,1.f,1.f }); //tl
            if (!downscroll)
            {
                verts.push_back({ body.x + body.skewBL, body.y + body.h,
                    0, -1,
                    1.f,1.f,1.f,1.f }); //bl
            }
            else
            {
                verts.push_back({ body.x + body.skewBL, body.y + body.h,
                    0, 1,
                    1.f,1.f,1.f,1.f }); //bl
            }
            verts.push_back({ body.x + body.skewTR + body.w, body.y,
                1, 0,
                1.f,1.f,1.f,1.f }); //tr
            verts.push_back({ body.x + body.skewTR + body.w, body.y,
                1, 0,
                1.f,1.f,1.f,1.f }); //tr
            if (!downscroll)
            {
                verts.push_back({ body.x + body.skewBL, body.y + body.h,
                    0, -1,
                    1.f,1.f,1.f,1.f }); //bl
                verts.push_back({ body.x + body.skewBR + body.w, body.y + body.h,
                    1, -1,
                    1.f,1.f,1.f,1.f }); //br
            }
            else
            {
                verts.push_back({ body.x + body.skewBL, body.y + body.h,
                    0, 1,
                    1.f,1.f,1.f,1.f }); //bl
                verts.push_back({ body.x + body.skewBR + body.w, body.y + body.h,
                    1, 1,
                    1.f,1.f,1.f,1.f }); //br
            }

            Rect test;
            test.x = 0;
            test.y = obj->modY + (32 * size);
            test.w = 1280;
            test.h = 720;
            if (downscroll)
            {
                test.y = 0;
                test.h = obj->modY + (32 * size);
            }

            if (holding || body.beat < holdstoppedbeat)
                Rendering::SetClipRect(&test);

            if (i != bodies.size() - 1)
                Rendering::PushQuad(verts, Game::noteskin->hold, GL::genShader);
            else
            {
                Rendering::PushQuad(verts, Game::noteskin->holdend, GL::genShader);
            }
            Rendering::SetClipRect(NULL);
            i++;
        }
    }

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