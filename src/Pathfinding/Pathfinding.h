#pragma once

#include <vector>





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
    using IsWalkableFn = bool (*)(int col, int row);

    bool FindPathBFS(GridPos start, GridPos goal, int gridWidth, int gridHeight, IsWalkableFn isWalkable, std::vector<GridPos>& outPath);

    GridPos FindNextStepBFS(GridPos start, GridPos goal, int gridWidth, int gridHeight, IsWalkableFn isWalkable);
}
