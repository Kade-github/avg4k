#include "NoteObject.h"

#include "Gameplay.h"

#include <mutex>
#include "Judge.h"

#include "SongSelect.h"
#include "Helpers.h"

NoteObject::NoteObject()
{
    setX(-400);
    setY(-400);
    w = 64;
    h = 64;
    Object::currentId++;
    id = Object::currentId;
    cmod = Game::instance->save->GetDouble("scrollspeed");

    if (Gameplay::instance != NULL)
        if (Gameplay::instance->runModStuff)
        {
            Gameplay::instance->currentModId++;
            modId = Gameplay::instance->currentModId;
            modY = y;
            modX = x;
        }
}

// Find the distance the note should be from the receptor

float NoteObject::calcCMod(float diff)
{

    float bps = (cmod / 60) / Gameplay::rate;

    float noteOffset = (bps * (diff / 1000)) * (64 * size);

    return noteOffset;
}

// Find the distance the note should be from the receptor (in beats!)

float NoteObject::calcXMod(float diff, float bpm)
{
    float noteOffset = ((diff * (64 * size)) * (bpm / 200)) * (cmod / 200);

    return noteOffset;
}

void NoteObject::draw() {
    if (!drawCall)
        return;

    bool sparrow = Game::noteskin->sparrowImg;

    MUTATE_START
    float position = rTime;

    float currentBeat = currentChart->getBeatFromTime(position, curSeg);

    Rect receptor;

    ReceptorObject* obj = connectedReceptor;

    receptor.x = obj->x;
    receptor.y = obj->y;
    receptor.w = obj->w;
    receptor.h = obj->h;

    bpmSegment bruh = curSeg;

    float wh = currentChart->getTimeFromBeat(beat, bruh);

    float diff = (wh)-(rTime);
    float noteOffset = calcCMod(diff);

    if (xmod)
    {
        if (currentBeat < beat + stopOffset)
        {
            diff = (beat + stopOffset) - currentBeat;
        }
        else
        {
            diff = beat - currentBeat;
        }
        noteOffset = calcXMod(diff, curSeg.bpm);
    }

    cmodOffset = noteOffset;

    bool downscroll = Game::save->GetBool("downscroll");

    if (ModManager::doMods)
    {
        if (arrowEffects->reverse[lane] >= 0.5)
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
            else if (fmod(beatRow, (192 / 64)) == 0)
                texture = Game::noteskin->sixtyfourth;
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

    if (type == Note_Fake)
        texture = Game::noteskin->fake;

    //Rendering::SetClipRect(&clipThingy);

    Shader* sh = customShader;

    if (!customShader)
        sh = GL::genShader;

    int activeH = 0;


    
    dstRect.h = rect.h;

    dstRect.x = x;
    dstRect.y = y;
    float addAngle = 0;

    if (!blownUp && type == Note_Mine)
        drawAngle += 1;

    float white = 0;

    float ogAlpha = alpha;

    ArrowEffects::Arrow offsetA;

    if (ModManager::doMods)
    {
        std::vector<ArrowEffects::Arrow> ar = arrowEffects->finishEffects(obj->x, obj->y, noteOffset, obj->type, -1, position, diff, beat, currentBeat);
        offsetA = ar[1];
        ArrowEffects::Arrow a = ar[0];

        dstRect.x = a.x;
        dstRect.y = a.y;
        white = a.whiteV;
        dstRect.a = a.opac;
        ogAlpha = dstRect.a;
        addAngle = a.rot;
        size = arrowEffects->noteSize * (0.5 / a.mini);

        dstRect.w = 64 * size;
        dstRect.h = 64 * size;

        Game::instance->whiteShader->GL_Use();

        if (white > dstRect.a)
            white = dstRect.a;

        Game::instance->whiteShader->SetUniform("yomamam", white);
    }


    Rect line;
    line.r = 255;
    line.g = 255;
    line.b = 255;


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

    AvgSparrow* sparrows = NULL;

    if (sparrow)
    {
        sparrows = Game::noteskin->sparrow;
        if (end.name == "")
        {
            std::string anim;
            std::string endAnim;
            switch (lane)
            {
            case 0:
                anim = Game::noteskin->holdLeft;
                endAnim = Game::noteskin->holdEndLeft;
                break;
            case 1:
                anim = Game::noteskin->holdDown;
                endAnim = Game::noteskin->holdEndDown;
                break;
            case 2:
                anim = Game::noteskin->holdUp;
                endAnim = Game::noteskin->holdEndUp;
                break;
            case 3:
                anim = Game::noteskin->holdRight;
                endAnim = Game::noteskin->holdEndRight;
                break;
            }
            end = sparrows->getRectFromFrame(anim, 0);
            hold = sparrows->getRectFromFrame(endAnim, 0);
        }
    }

    if (endBeat != -1 && !missHold)
    {

        float ewh = currentChart->getTimeFromBeat(endBeat, bruh);

        float difff = (ewh)-(wh);

        float endPos = calcCMod(difff);

        float diffBeat = endBeat - beat;

        if (xmod)
        {
            endPos = calcXMod(diffBeat, bruh.bpm);
        }

        line.h = endPos;
        line.w = 4;
        line.x = x + (32 * size);
        line.y = dstRect.y + (32 * size);
        if (downscroll)
            line.y = dstRect.y - (32 * size);

        //Rendering::PushQuad(&line, &srcRect, NULL, NULL);

        int amountToDraw = 1 + (endPos / std::round((64 * size)));

        int lastSkewStart;
        int lastSkewEnd;

        std::vector<holdBody> bodies;



        if (amountToDraw > 1)
        {
            float fBeat = 0.01;
            for (int i = 0; i < amountToDraw; i++)
            {
                float perc = (float)(i) / (float)amountToDraw;

                float holdBeat = beat + ((endBeat - beat) * perc);

                Rect square;
                square.w = 64 * size;
                square.h = 64 * size;
                square.x = x;
                square.y = line.y + ((64 * size) * i);
                if (downscroll)
                    square.y = (line.y) - ((64 * size) * i);

                holdBody body;
                body.beat = holdBeat;

                float offset = 1;

                if (holding && holdBeat < currentBeat)
                {
                    offset = holdBeat / currentBeat;
                }

                float oldX = square.x;
                float oldY = square.y;
                if (ModManager::doMods)
                {
                    bpmSegment hS1 = currentChart->getSegmentFromBeat(holdBeat);
                    bpmSegment hS2 = currentChart->getSegmentFromBeat(holdBeat + fBeat);
                    float ttime = currentChart->getTimeFromBeat(holdBeat, hS1);
                    float ttime2 = currentChart->getTimeFromBeat(holdBeat + fBeat, hS2);


                    float holdDiff = ttime - position;
                    float holdDiff2 = ttime2 - position;
                    float cmodHold = calcCMod(holdDiff);
                    float cmodHold2 = calcCMod(holdDiff2);

                    if (xmod)
                    {
                        if (currentBeat < holdBeat + stopOffset)
                        {
                            holdDiff = (holdBeat + stopOffset) - currentBeat;
                            holdDiff2 = (holdBeat + fBeat  + stopOffset) - currentBeat;
                        }
                        else
                        {
                            holdDiff = holdBeat - currentBeat;
                            holdDiff2 = (holdBeat + fBeat) - currentBeat;
                        }
                        cmodHold = calcXMod(holdDiff, hS1.bpm);
                        cmodHold2 = calcXMod(holdDiff2, hS2.bpm);
                    }

                    ArrowEffects::Arrow offsetA2;
                    std::vector<ArrowEffects::Arrow> real = arrowEffects->finishEffects(obj->x, obj->y, cmodHold, obj->type, time, position, holdDiff, holdBeat, currentBeat);



                    std::vector<ArrowEffects::Arrow> ahead = arrowEffects->finishEffects(obj->x, obj->y, cmodHold2, obj->type, time, position, holdDiff2, holdBeat + fBeat, currentBeat);
                    ArrowEffects::Arrow a = real[0];
                    square.x = a.x;
                    square.y = a.y;


                    float size = Game::instance->save->GetDouble("Note Size") * (0.5 / a.mini);

                    // center the stuff on the arrow path

                    float t = (32 * size);


                    real[0].x += t;
                    ahead[0].x += t;
                    real[0].y += t;
                    ahead[0].y += t;

                    float diffY = (ahead[0].y) - (real[0].y);
                    float diffX = (ahead[0].x) - (real[0].x);

                    // find radians

                    float fAng = atan2f(diffY, diffX);

                    body.deg = fAng;

                    // get the x/y offsets by their angle

                    float xxOffset = t * sin(fAng);
                    float yyOffset = t * cos(fAng);

                    // get the vert pos's

                    if (sparrow)
                    {
                        AvgFrame fr = hold;
                        if (xxOffset > 0)
                            xxOffset -= fr.frameRect.w / 1.75;
                        else
                            xxOffset += fr.frameRect.w / 1.75;
                    }


                    float leftx = (real[0].x - xxOffset);
                    float rightx = (real[0].x + xxOffset);
                    float lefty = (real[0].y + yyOffset);
                    float righty = (real[0].y - yyOffset);


                    if (i != 0)
                    {
                        holdBody& lB = bodies.back();
                        if (!downscroll || sparrow)
                            lB.verts.push_back({ leftx, lefty,
                                0, -1,
                                (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // upscroll bl
                        else
                            lB.verts.push_back({ leftx, lefty,
                                0, -1,
                                (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // downscroll bl
                        if (!downscroll || sparrow)
                            lB.verts.push_back({ rightx, righty,
                                1, -1,
                               (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // upscroll br
                        else
                            lB.verts.push_back({ rightx, righty,
                                1, -1,
                                (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // downscroll br
                        if (i == amountToDraw - 1)
                            break;
                    }

                    body.verts.push_back({ leftx, lefty,
                        0, 0,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // tl
                    body.verts.push_back({ rightx , righty,
                        1, 0,
                       (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // tr
                }
                else
                {
                    body.verts.push_back({ square.x, square.y,
                        0, 0,
                        (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // tl
                    body.verts.push_back({ square.x + square.w, square.y,
                        1, 0,
                       (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // tr
                    if (!downscroll)
                        body.verts.push_back({ square.x, square.y + square.h,
                            0, -1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // bl
                    else
                        body.verts.push_back({ square.x, square.y + square.h,
                            0, 1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // bl
                    if (!downscroll)
                        body.verts.push_back({ square.x + square.w, square.y + square.h,
                            1, -1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // br
                    else
                        body.verts.push_back({ square.x + square.w, square.y + square.h,
                            1, 1,
                            (dstRect.r) / 255.f,(dstRect.g) / 255.f,(dstRect.b) / 255.f,(dstRect.a) }); // br

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
                if (body.y > Game::gameHeight + 100 || body.y < -200 || (body.beat - 0.15 < holdstoppedbeat))
                {
                    i++;
                    continue;
                }

                float dBeats = 8;
                if (xmod)
                    dBeats = (8 * (curSeg.bpm / 100)) * (cmod / 100);
                if (arrowEffects)
                    dBeats = arrowEffects->drawBeats;

                if (body.beat > beat + dBeats)
                    break;

                std::vector<GL_Vertex> verts;


                if (body.beat < currentBeat && holding)
                    Rendering::SetClipRect(&test);

                if (!sparrow)
                {

                    float offset = 1;

                    verts.push_back(body.verts[0]); // tl
                    verts.push_back(body.verts[2]); // bl
                    verts.push_back(body.verts[1]); // tr
                    verts.push_back(body.verts[1]); // tr
                    verts.push_back(body.verts[2]); // bl
                    verts.push_back(body.verts[3]); // br
                    if (i != bodies.size() - 1)
                    {
                        Rendering::PushQuad(verts, Game::noteskin->hold, GL::genShader);
                        Rendering::drawBatch();
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
                        Rendering::drawBatch();
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            for (GL_Vertex& vert : verts)
                                vert.a = 1;
                            Rendering::PushQuad(body.verts, Game::noteskin->holdend, Game::instance->whiteShader);
                            for (GL_Vertex& vert : verts)
                                vert.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                    }
                }
                else
                {
                    bool noEnd = i != bodies.size() - 1;

                    AvgFrame fr;

                    if (!noEnd)
                        fr = hold;
                    else
                        fr = end;

                    srcRect = fr.srcRect;

                    body.verts[0].u = fr.srcRect.x;
                    body.verts[0].v = fr.srcRect.y;
                    body.verts[1].u = fr.srcRect.x + fr.srcRect.w;
                    body.verts[1].v = fr.srcRect.y;
                    body.verts[2].u = fr.srcRect.x;
                    body.verts[2].v = fr.srcRect.y + fr.srcRect.h;
                    body.verts[3].u = fr.srcRect.x + fr.srcRect.w;
                    body.verts[3].v = fr.srcRect.y + fr.srcRect.h;

                    verts.push_back(body.verts[0]); // tl
                    verts.push_back(body.verts[2]); // bl
                    verts.push_back(body.verts[1]); // tr
                    verts.push_back(body.verts[1]); // tr
                    verts.push_back(body.verts[2]); // bl
                    verts.push_back(body.verts[3]); // br

                    if (holding && body.beat < beat)
                        Rendering::SetClipRect(&test);
                    Rendering::PushQuad(verts, Game::noteskin->sparrowImg, GL::genShader);
                    Rendering::drawBatch();
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        for (GL_Vertex& vert : verts)
                            vert.a = 1;
                        Rendering::PushQuad(verts, Game::noteskin->sparrowImg, Game::instance->whiteShader);
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
            AvgFrame fr = sparrows->getRectFromFrame(anim, frame);
            srcRect = fr.srcRect;

            Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->sparrowImg, sh, drawAngle + addAngle);
            if (white != 0)
            {
                Rendering::drawBatch();
                dstRect.a = 1;
                Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->sparrowImg, Game::instance->whiteShader, drawAngle + addAngle);
                dstRect.a = ogAlpha;
                Rendering::drawBatch();
            }
        }
        else
        {
            if (type != Note_Mine)
            {
                if (Game::noteskin->rotate) {
                    switch (lane) {
                    case 0: // left
                        Rendering::PushQuad(&dstRect, &srcRect, texture, sh, 90 + drawAngle + addAngle);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            dstRect.a = 1;
                            Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, 90 + drawAngle + addAngle);
                            dstRect.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                        break;
                    case 1: // down
                        Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle + addAngle);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            dstRect.a = 1;
                            Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle + addAngle);
                            dstRect.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                        break;
                    case 2: // up
                        srcRect.h = -1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle + addAngle);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            dstRect.a = 1;
                            Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle + addAngle);
                            dstRect.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                        srcRect.h = 1;
                        break;
                    case 3: // right
                        Rendering::PushQuad(&dstRect, &srcRect, texture, sh, -90 + drawAngle + addAngle);
                        if (white != 0)
                        {
                            Rendering::drawBatch();
                            dstRect.a = 1;
                            Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, -90 + drawAngle + addAngle);
                            dstRect.a = ogAlpha;
                            Rendering::drawBatch();
                        }
                        break;
                    }
                }
                else {
                    Rendering::PushQuad(&dstRect, &srcRect, texture, sh, drawAngle + addAngle);
                    if (white != 0)
                    {
                        Rendering::drawBatch();
                        dstRect.a = 1;
                        Rendering::PushQuad(&dstRect, &srcRect, texture, Game::instance->whiteShader, drawAngle + addAngle);
                        dstRect.a = ogAlpha;
                        Rendering::drawBatch();
                    }
                }
            }
            else
            {
                if (!blownUp)
                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->mine, sh, drawAngle + addAngle);
                else
                {
                    dstRect.x = receptor.x;
                    dstRect.y = receptor.y;
                    AvgSparrow* sparrow = Game::noteskin->explosionSheet;
                    int size = sparrow->animations["anim"].frames.size();

                    animTime += Game::deltaTime;
                    frame = (animTime * fps / 1000);

                    if (frame > size - 1)
                    {
                        active = false;
                        frame = 0;
                    }
                    

                    AvgFrame fr = sparrow->getRectFromFrame("anim", frame, true);
                    srcRect = fr.srcRect;

                    Rendering::PushQuad(&dstRect, &srcRect, Game::noteskin->explosion, sh, addAngle);
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