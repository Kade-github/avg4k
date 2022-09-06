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
    float skewYTL = 0, skewYTR = 0;
    float skewYBL = 0, skewYBR = 0;
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

    bool sparrow = Game::noteskin->sparrowImg;

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

    if (ModManager::doMods)
    {
        if (ArrowEffects::reverse[lane] >= 0.5)
            downscroll = true;
        else
            downscroll = false;

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


    Texture* texture = NULL;

    // get quant

    if (!sparrow)
    {
        if (!Game::noteskin->disableQuant)
        {

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
        }
        else
        {
            switch (lane)
            {
            case 0:
                texture = Game::noteskin->left;
                break;
            case 1:
                texture = Game::noteskin->down;
                break;
            case 2:
                texture = Game::noteskin->up;
                break;
            case 3:
                texture = Game::noteskin->right;
                break;
            }
        }
    }
    //Rendering::SetClipRect(&clipThingy);

    Shader* sh = customShader;

    if (!customShader)
        sh = GL::genShader;

    int activeH = 0;


    
    dstRect.h = rect.h;

    dstRect.x = x;
    dstRect.y = y;

    float drawAngle = 0;

    float white = 0;

    float ogAlpha = alpha;

    if (ModManager::doMods)
    {
        ArrowEffects::Arrow a = ArrowEffects::finishEffects(obj->x, obj->y, noteOffset, obj->type, -1, position, diff);
        dstRect.x = a.x;
        dstRect.y = a.y;
        white = a.whiteV;
        dstRect.a = a.opac;
        ogAlpha = dstRect.a;
        drawAngle = a.rot;

        Game::instance->whiteShader->GL_Use();

        if (white > dstRect.a)
            white = dstRect.a;

        Game::instance->whiteShader->SetUniform("yomamam", white);
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

        if (amountToDraw > 1)
        {

            for (int i = 0; i < amountToDraw; i++)
            {
                Rect square;
                square.w = 64 * size;
                square.h = 64 * size;
                square.x = x;
                square.y = line.y + ((64 * size) * i);
                if (downscroll)
                    square.y = (line.y) - ((64 * size) * i);

                float perc = (float)(i + 1.f) / (float)amountToDraw;

                float holdBeat = beat + ((endBeat - beat) * perc);

                holdBody body;
                body.beat = holdBeat;
                if (ModManager::doMods)
                {
                    float ttime = currentChart->getTimeFromBeat(holdBeat, currentChart->getSegmentFromBeat(holdBeat));
                    float holdDiff = ttime - position;
                    float cmodHold = calcCMod(holdDiff);
                    ArrowEffects::Arrow a = ArrowEffects::finishEffects(obj->x, obj->y, cmodHold, obj->type, time, position, holdDiff);
                    square.x = a.x;
                    square.y = a.y;

                    if (bodies.size() == 0)
                    {
                        if (!downscroll)
                        {
                            body.skewTL = -(square.x - dstRect.x);
                            body.skewTR = -((square.x + square.w) - (dstRect.x + (64 * size)));
                            body.skewYTL = -(square.y - (dstRect.y + (32 * size)));
                            body.skewYTR = -(square.y - (dstRect.y + (32 * size)));
                        }
                        else
                        {
                            body.skewBL = -(square.x - dstRect.x);
                            body.skewBR = -((square.x + square.w) - (dstRect.x + (64 * size)));
                        }
                    }
                    else
                    {
                        holdBody& lastBody = bodies.back();
                        if (!downscroll)
                        {
                            body.skewTL = -(square.x - lastBody.x);
                            body.skewTR = -((square.x + square.w) - (lastBody.x + lastBody.w));
                            lastBody.skewYBL = -((lastBody.y + lastBody.h) - square.y);
                            lastBody.skewYBR = -((lastBody.y + lastBody.h) - square.y);
                        }
                        else
                        {
                            body.skewBL = -(square.x - lastBody.x);
                            body.skewBR = -((square.x + square.w) - (lastBody.x + lastBody.w));
                            lastBody.skewYTL = (-(lastBody.y - (square.y + square.h / 2)));
                            lastBody.skewYTR = (-(lastBody.y - (square.y + square.h / 2)));

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
            for (holdBody& body : bodies)
            {
                std::vector<GL_Vertex> verts;

                if (!sparrow)
                {
                    verts.push_back({ body.x + body.skewTL, body.y + body.skewYTL,
                        0, 0,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tl
                    if (!downscroll)
                    {
                        verts.push_back({ body.x + body.skewBL, body.y + body.h + body.skewYBL,
                            0, -1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                    }
                    else
                    {
                        verts.push_back({ body.x + body.skewBL, body.y + body.h + body.skewYBL,
                            0, 1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                    }
                    verts.push_back({ body.x + body.skewTR + body.w, body.y + body.skewYTR,
                        1, 0,
                       (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tr
                    verts.push_back({ body.x + body.skewTR + body.w, body.y + body.skewYTR,
                        1, 0,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tr
                    if (!downscroll)
                    {
                        verts.push_back({ body.x + body.skewBL, body.y + body.h + body.skewYBL,
                            0, -1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                        verts.push_back({ body.x + body.skewBR + body.w, body.y + body.h + body.skewYBR,
                            1, -1,
                           (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //br
                    }
                    else
                    {
                        verts.push_back({ body.x + body.skewBL, body.y + body.h + body.skewYBL,
                            0, 1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                        verts.push_back({ body.x + body.skewBR + body.w, body.y + body.h + body.skewYBR,
                            1, 1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //br
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
                    {
                        Rendering::PushQuad(verts, Game::noteskin->hold, GL::genShader);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            for (GL_Vertex& vert : verts)
                                vert.a = 1;
                            Rendering::PushQuad(verts, Game::noteskin->hold, Game::instance->whiteShader);
                            for (GL_Vertex& vert : verts)
                                vert.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                    }
                    else
                    {
                        Rendering::PushQuad(verts, Game::noteskin->holdend, GL::genShader);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            for (GL_Vertex& vert : verts)
                                vert.a = 1;
                            Rendering::PushQuad(verts, Game::noteskin->holdend, Game::instance->whiteShader);
                            for (GL_Vertex& vert : verts)
                                vert.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                    }
                }
                else
                {
                    AvgSparrow* sparrow = Game::noteskin->sparrow;

                    animTime += Game::deltaTime;
                    frame = (animTime * fps / 1000);

                    std::string anim = "";

                    bool noEnd = i != bodies.size() - 1;

                    switch (lane)
                    {
                    case 0:
                        if (noEnd)
                            anim = Game::noteskin->holdLeft;
                        else
                            anim = Game::noteskin->holdEndLeft;
                        break;
                    case 1:
                        if (noEnd)
                            anim = Game::noteskin->holdDown;
                        else
                            anim = Game::noteskin->holdEndDown;
                        break;
                    case 2:
                        if (noEnd)
                            anim = Game::noteskin->holdUp;
                        else
                            anim = Game::noteskin->holdEndUp;
                        break;
                    case 3:
                        if (noEnd)
                            anim = Game::noteskin->holdRight;
                        else
                            anim = Game::noteskin->holdEndRight;
                        break;
                    }

                    int sizee = sparrow->animations[anim].frames.size();
                    if (frame > sizee - 1)
                    {
                        animTime = 0;
                    }
                    if (frame > sizee - 1)
                        frame = 0;

                    AvgFrame fr = sparrow->getRectFromFrame(anim, frame);

                    float realWidth = (fr.srcRect.w * Game::noteskin->sparrowImg->width);
                    float ogH = (fr.srcRect.h * Game::noteskin->sparrowImg->height);
                    float realHeight = (fr.srcRect.h * Game::noteskin->sparrowImg->height);

                    realWidth = (64 * size) / 3;

                    float newX = body.x + ((32 * size) - (realWidth / 2));

                    realHeight = 64 * size;
                    if (i == bodies.size() - 1 && ogH > 32)
                    {
                        realHeight = (ogH * size) / 2;
                        if (downscroll)
                            body.y += (32 * size);
                    }

                    srcRect = fr.srcRect;
                    verts.push_back({ newX + body.skewTL, body.y + body.skewYTL,
                        srcRect.x, srcRect.y,
                    (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tl
                    verts.push_back({ newX + body.skewBL, body.y + realHeight + body.skewYBL,
                        srcRect.x, srcRect.y + srcRect.h,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                    verts.push_back({ newX + body.skewTR + realWidth, body.y + body.skewYTR,
                        srcRect.x + srcRect.w, srcRect.y,
                       (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tr
                    verts.push_back({ newX + body.skewTR + realWidth, body.y + body.skewYTR,
                        srcRect.x + srcRect.w, srcRect.y,
                       (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //tr
                    verts.push_back({ newX + body.skewBL, body.y + realHeight + body.skewYBL,
                        srcRect.x, srcRect.y + srcRect.h,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //bl
                    verts.push_back({ newX + body.skewBR + realWidth, body.y + realHeight + body.skewYBR,
                        srcRect.x + srcRect.w, srcRect.y + srcRect.h,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); //br
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
                    if (!downscroll)
                        Rendering::PushQuad(verts, Game::noteskin->sparrowImg, GL::genShader);
                    else
                        Rendering::PushQuad(verts, Game::noteskin->sparrowImg, GL::genShader, newX, body.y, realWidth, realHeight, 180);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        for (GL_Vertex& vert : verts)
                            vert.a = 1;
                        if (!downscroll)
                            Rendering::PushQuad(verts, Game::noteskin->sparrowImg, Game::instance->whiteShader);
                        else
                            Rendering::PushQuad(verts, Game::noteskin->sparrowImg, Game::instance->whiteShader, newX, body.y, realWidth, realHeight, 180);
                        for (GL_Vertex& vert : verts)
                            vert.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                }

                Rendering::SetClipRect(NULL);
                i++;
            }
        }
    }

    //Rendering::SetClipRect(NULL);
    if (active) {
        if (sparrow)
        {
            AvgSparrow* sparrow = Game::noteskin->sparrow;

            animTime += Game::deltaTime;
            frame = (animTime * fps / 1000);

            std::string anim = "";

            switch (lane)
            {
            case 0:
                anim = Game::noteskin->leftA;
                break;
            case 1:
                anim = Game::noteskin->downA;
                break;
            case 2:
                anim = Game::noteskin->upA;
                break;
            case 3:
                anim = Game::noteskin->rightA;
                break;
            }

            int size = sparrow->animations[anim].frames.size();
            if (frame > size - 1)
            {
                animTime = 0;
            }
            if (frame > size - 1)
                frame = 0;

            AvgFrame fr = sparrow->getRectFromFrame(anim, frame);
            srcRect = fr.srcRect;

            Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->sparrowImg, sh, drawAngle);
            if (white != 0)
            {
                Rendering::drawBatch();
                dstRect.a = 1;
                Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->sparrowImg, Game::instance->whiteShader, drawAngle);
                dstRect.a = ogAlpha;
                Rendering::drawBatch();
            }
        }
        else
        {
            if (Game::noteskin->rotate) {
                switch (lane) {
                case 0: // left
                    Rendering::PushQuad(&dstRect, &srcRect, texture, sh, 90 + drawAngle);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        dstRect.a = 1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, 90 + drawAngle);
                        dstRect.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                    break;
                case 1: // down
                    Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        dstRect.a = 1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle);
                        dstRect.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                    break;
                case 2: // up
                    srcRect.h = -1;
                    Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        dstRect.a = 1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle);
                        dstRect.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                    srcRect.h = 1;
                    break;
                case 3: // right
                    Rendering::PushQuad(&dstRect, &srcRect, texture, sh, -90 + drawAngle);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        dstRect.a = 1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, -90 + drawAngle);
                        dstRect.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                    break;
                }
            }
            else {
                Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle);
                if (white != 0)
                {
                    Rendering::drawBatch();
                    dstRect.a = 1;
                    Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle);
                    dstRect.a = ogAlpha;
                    Rendering::drawBatch();
                }
            }

        }
    }

    if (selected)
    {
        dstRect.a = 0.6;
        Rendering::PushQuad(&dstRect, &srcRect, NULL, sh);
        dstRect.a = alpha;
    }

    MUTATE_END
}