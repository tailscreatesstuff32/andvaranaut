#include "Map.h"

#include "Sprites.h"
#include "Title.h"
#include "util.h"

#include <math.h>

Map xzmap()
{
    static Map map;
    return map;
}

static char** reset(char** block, const int rows, const int cols, const int blok)
{
    for(int row = 0; row < rows; row++)
    for(int col = 0; col < cols; col++)
        block[row][col] = blok;
    return block;
}

static char** mnew(const int rows, const int cols, const int blok)
{
    char** block = xtoss(char*, rows);

    for(int row = 0; row < rows; row++)
        block[row] = xtoss(char, cols);

    return reset(block, rows, cols, blok);
}

Map xmgen(const int rows, const int cols, const Points trapdoors, const Points interests, const int grid)
{
    // Randomize sheers.
    const Sheer md = {
        0.0f,
        md.a + (float) (rand() % 2)
    };
    const Sheer up = {
        md.b + 1.0f,
        up.a + (float) (rand() % 2)
    };

    // Construct map.
    Map map = xzmap();

    map.rows = rows;
    map.cols = cols;

    map.ceiling = mnew(map.rows, map.cols, '#');
    map.walling = mnew(map.rows, map.cols, '#');
    map.floring = mnew(map.rows, map.cols, '"');

    map.trapdoors = trapdoors;
    map.rooms = xrsinit(interests);

    map.upper = up;
    map.middle = md;
    map.grid = grid;

    return map;
}

// Lookup theme.
Theme lutheme(const Map map, const Point where)
{
    for(int i = 0; i < map.rooms.count; i++)
        if(xeql(where, map.rooms.wheres[i], map.grid))
            return map.rooms.themes[i];
    return NO_THEME;
}

// Theme string.
static char* themestr(const Map map, const Point where)
{
    return xthname(lutheme(map, where));
}

// Returns true if change in theme with respect to position.
static int themech(const Map map, const Point where)
{
    int change = false;

    // Last theme from last game frame.
    static Theme last = NO_THEME;

    // Update current theme to this game frame.
    const Theme now = lutheme(map, where);

    // If the last theme is not the current theme then
    // there was a theme change.
    if(now != last)
        change = true;

    // The theme change is overrided if the point is
    // going to where there is no theme (like a room to a coridoor).
    if(now == NO_THEME && last != NO_THEME)
        change = false;

    // Update the last theme.
    last = lutheme(map, where);

    return change;
}

// Theme title.
void xmthemett(const Map map, const Point where, const Timer tm)
{
    // If there was a theme change in the map with the player position...
    if(themech(map, where))
        // Set the title for the current room theme.
        xttset(tm.renders, tm.renders + 120, false, themestr(map, where));
}

int xmisportal(char** block, const Point where)
{
    return xblok(where, block) == '~';
}

int xmout(const Map map, const Point where)
{
    return (int) where.x >= map.cols || (int) where.x < 0
        || (int) where.y >= map.rows || (int) where.y < 0;
}

void xmedit(const Map map, const Overview ov)
{
    // Placing - Out of bounds check.
    if(xmout(map, ov.where))
        return;

    const int ascii = ov.selected + ' ';

    // If the ascii is an alpha character then it is a sprite
    if(xsissprite(ascii))
        return;

    // Otherwise place the ascii character.
    const int x = ov.where.x;
    const int y = ov.where.y;
    if(ov.party == FLORING) map.floring[y][x] = ascii;
    if(ov.party == WALLING) map.walling[y][x] = ascii;
    if(ov.party == CEILING) map.ceiling[y][x] = ascii;
}

void xmroom(const Map map, const Point where, const int w, const int h, const Party p)
{
    for(int i = -w; i <= w; i++)
    for(int j = -h; j <= h; j++)
    {
        const int xx = where.x + i;
        const int yy = where.y + j;
        switch(p)
        {
        case WALLING: map.walling[yy][xx] = ' '; break;
        case CEILING: map.ceiling[yy][xx] = ' '; break;
        case FLORING:
            // Will only dig out if nothing is above.
            if(map.walling[yy][xx] == ' ')
                map.floring[yy][xx] = ' ';
            break;

        default:
            xbomb("room: party not supported\n");
            break;
        }
    }
}

void xmpole(const Map map, const Point where, const int ascii)
{
    const int x = where.x;
    const int y = where.y;
    map.ceiling[y][x] = map.walling[y][x] = map.floring[y][x] = ascii;
}

// Supports for trapdoors are only put down for ceiling parties.
static void supports(const Map map, const int x, const int y, const Party p)
{
    if(p == CEILING)
    {
        map.walling[y + 1][x + 1] = map.floring[y + 1][x + 1] =
        map.walling[y + 1][x - 1] = map.floring[y + 1][x - 1] =
        map.walling[y - 1][x - 1] = map.floring[y - 1][x - 1] =
        map.walling[y - 1][x + 1] = map.floring[y - 1][x + 1] = '#';
    }
}

void xmplatform(const Map map, const int x, const int y, const Party p)
{
    for(int j = -1; j <= 1; j++)
    for(int k = -1; k <= 1; k++)
    {
        const int yy = j + y;
        const int xx = k + x;
        switch(p)
        {
        case FLORING: map.floring[yy][xx] = '"'; break;
        case CEILING: map.ceiling[yy][xx] = '#'; break;
        default:
            xbomb("platform: party not supported\n");
            break;
        }
    }
}

static void trapdoor(const Map map, const int x, const int y, const Party p)
{
    switch(p)
    {
    case FLORING: map.floring[y][x] = '~'; break;
    case CEILING: map.ceiling[y][x] = '~'; break;
    default:
        xbomb("trapdoor: party not supported\n");
        break;
    }
}

void xmtrapdoors(const Map map, const Points trapdoors, const Party p)
{
    for(int i = 0; i < trapdoors.count; i++)
    {
        const Point where = trapdoors.point[i];

        const int x = where.x;
        const int y = where.y;

        xmplatform(map, x, y, p);
        supports(map, x, y, p);
        trapdoor(map, x, y, p);
    }
}

void xmcorridor(const Map map, const Point a, const Point b)
{
    const Point step = xsub(b, a);
    const Point delta = {
        step.x > 0.0f ? 1.0f : step.x < 0.0f ? -1.0f : 0.0f,
        step.y > 0.0f ? 1.0f : step.y < 0.0f ? -1.0f : 0.0f,
    };

    // X and Y steppings.
    const int sx = fabsf(step.x);
    const int sy = fabsf(step.y);

    // Delta step amount.
    const int dx = delta.x;
    const int dy = delta.y;

    // Step along the X deltas first and then the Y deltas.
    int x = a.x;
    int y = a.y;
    for(int i = 0; i < sx; i++) map.walling[y][x += dx] = ' ';
    for(int i = 0; i < sy; i++) map.walling[y += dy][x] = ' ';
}

static void cross(const Map map, const int x, const int y, const char a, const char b)
{
    const int end = map.grid / 2;
    if(map.walling[y + 0][x + end] == a) map.walling[y + 0][x + end] = b;
    if(map.walling[y + 0][x - end] == a) map.walling[y + 0][x - end] = b;
    if(map.walling[y + end][x + 0] == a) map.walling[y + end][x + 0] = b;
    if(map.walling[y - end][x + 0] == a) map.walling[y - end][x + 0] = b;
}

// Adds doors to block corridoors.
static void door(const Map map, const int x, const int y)
{
    cross(map, x, y, ' ', '!');
}

// Adds a passge to a formerly blocked corridoor.
static void pass(const Map map, const int x, const int y)
{
    cross(map, x, y, '!', ' ');
}

// Barricades a room from other rooms only if there are agents in the room.
void xmbarricade(const Map map)
{
    // Barricade first.
    for(int i = 0; i < map.rooms.count; i++)
    {
        const Point mid = map.rooms.wheres[i];
        door(map, mid.x, mid.y);
    }
    // Then remove barricades when rooms have zero agents left.
    for(int i = 0; i < map.rooms.count; i++)
    {
        const Point mid = map.rooms.wheres[i];

        if(map.rooms.agents[i] == 0)
            pass(map, mid.x, mid.y);
    }
}
