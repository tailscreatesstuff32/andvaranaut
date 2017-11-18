#include "Overview.h"

#include "util.h"

#include <SDL2/SDL.h>

Overview xinit()
{
    Overview ov;
    xzero(ov);
    ov.x = -1;
    ov.y = -1;
    ov.w = 32;
    ov.h = 32;
    ov.party = WALLING;
    ov.acceleration = 3.33;
    ov.speed = 20.0;
    return ov;
}

// When x and y are both -1 then then the left mouse is not pressed.
static Overview reset(Overview ov)
{
    ov.x = ov.y = -1;
    return ov;
}

Overview xupdate(Overview ov, const Input input, const int xres, const int textures)
{
    // Selecting either 1, 2, or 3 will change the overview party to either
    // the flooring, walling, or ceiling tiles.
    if(input.key[SDL_SCANCODE_1]) ov.party = FLORING;
    if(input.key[SDL_SCANCODE_2]) ov.party = WALLING;
    if(input.key[SDL_SCANCODE_3]) ov.party = CEILING;
    // Selection wheel update.
    if(input.key[SDL_SCANCODE_Q]) ov.wheel--;
    if(input.key[SDL_SCANCODE_E]) ov.wheel++;
    // Selection wheel lower clamp.
    if(ov.wheel < 0) ov.wheel = 0;
    // Selection wheel upper clamp.
    const int seen = xres / ov.w;
    if(textures - ov.wheel < seen) ov.wheel = textures - seen;
    // The left mouse button will update the x and y tile position of the overview.
    if(input.l)
    {
        // Overview global tiles.
        ov.x = (input.x - ov.px) / ov.w;
        ov.y = (input.y - ov.py) / ov.h;
        // Overview relative tiles.
        const int x = input.x / ov.w;
        const int y = input.y / ov.h;
        // If the zeroth row was chosen, then:
        if(y == 0)
        {
            // Reset the overview global tiles to -1.
            ov = reset(ov);
            // Selection update and...
            ov.selected = ov.wheel + x;
        }
    }
    // Map editing checks to see if the overview global tile selection
    // is out of map range or not before continuing. Reset here if left input is not pressed.
    else ov = reset(ov);
    // The right mouse button will pan the overview x and y pixels.
    if(input.r)
    {
        ov.px += input.dx;
        ov.py += input.dy;
    }
    // The right mouse button does not have to be used to pan.
    // The arrow keys can be used too with an acceleration effect.
    if(input.key[SDL_SCANCODE_W]
    || input.key[SDL_SCANCODE_S]
    || input.key[SDL_SCANCODE_D]
    || input.key[SDL_SCANCODE_A])
    {
        if(input.key[SDL_SCANCODE_W]) ov.velocity.y += ov.acceleration;
        if(input.key[SDL_SCANCODE_S]) ov.velocity.y -= ov.acceleration;
        if(input.key[SDL_SCANCODE_D]) ov.velocity.x -= ov.acceleration;
        if(input.key[SDL_SCANCODE_A]) ov.velocity.x += ov.acceleration;
    }
    else ov.velocity = xmul(ov.velocity, 1.0 - ov.acceleration / ov.speed);
    // Top speed check.
    if(xmag(ov.velocity) > ov.speed) ov.velocity = xmul(xunt(ov.velocity), ov.speed);
    // Due to integer rounding, velocities less than one must be ignored.
    if(abs(ov.velocity.x) < 1) ov.velocity.x = 0;
    if(abs(ov.velocity.y) < 1) ov.velocity.y = 0;
    // Add velocity to pan.
    ov.px += ov.velocity.x;
    ov.py += ov.velocity.y;
    return ov;
}
