#pragma once

#include <vector>

// ======================================================================
// Pathfinding.h
//
// Grid-based breadth-first search (BFS). Deliberately generic - it only
// knows about a width/height and a walkability test, not about
// Game.cpp's specific map data or tile values - so it isn't tied to
// the ghost AI that currently happens to be its only caller.
//
// BFS (rather than Dijkstra or A*) is enough here because every
// tile-to-tile step costs exactly 1. On a uniform-cost grid, BFS
// already finds the shortest path on its own, without needing a
// weighted-edge algorithm or a distance heuristic.
// ======================================================================

namespace pathfinding
{
    struct GridPos
    {
        int col, row;

        bool operator==(const GridPos& other) const
        {
            return col == other.col && row == other.row;
        }
    };

    // Returns true if (col, row) can be walked through.
    using IsWalkableFn = bool (*)(int col, int row);

    // Full breadth-first search from 'start' to 'goal' over a grid of
    // the given size. Fills 'outPath' with every step from (but not
    // including) 'start' up to and including 'goal', in order.
    //
    // Returns false (and leaves outPath empty) if no path exists, if
    // 'start' or 'goal' are out of bounds, or if 'start' itself isn't
    // walkable. Returns true with an empty outPath if start == goal
    // (i.e. "no move needed").
    bool FindPathBFS(
        GridPos start,
        GridPos goal,
        int gridWidth,
        int gridHeight,
        IsWalkableFn isWalkable,
        std::vector<GridPos>& outPath
    );

    // Convenience wrapper around FindPathBFS() for callers (like the
    // ghost AI) that only need to know the very next tile to step
    // onto, not the whole route. Returns 'start' unchanged if already
    // at 'goal', or if no path could be found at all.
    GridPos FindNextStepBFS(
        GridPos start,
        GridPos goal,
        int gridWidth,
        int gridHeight,
        IsWalkableFn isWalkable
    );
}
