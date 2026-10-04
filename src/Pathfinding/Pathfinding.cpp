#include "Pathfinding.h"

#include <queue>

namespace pathfinding
{
    static bool InBounds(GridPos p, int gridWidth, int gridHeight)
    {
        return p.col >= 0 && p.row >= 0 && p.col < gridWidth && p.row < gridHeight;
    }

    bool FindPathBFS(GridPos start, GridPos goal, int gridWidth,int gridHeight,
        IsWalkableFn isWalkable,
        std::vector<GridPos>& outPath)
    {
        outPath.clear();

        if (!InBounds(start, gridWidth, gridHeight) || !InBounds(goal, gridWidth, gridHeight))
            return false;

        if (!isWalkable(start.col, start.row))
            return false;

        if (start == goal)
            return true; 

        auto index = [gridWidth](GridPos p) { return p.row * gridWidth + p.col; };

        std::vector<bool> visited(static_cast<size_t>(gridWidth) * gridHeight, false);
        std::vector<int>  parent(static_cast<size_t>(gridWidth) * gridHeight, -1);

        std::queue<GridPos> frontier;
        frontier.push(start);
        visited[index(start)] = true;
        static const int kDX[4] = { 0, 0, -1, 1 };
        static const int kDY[4] = { -1, 1, 0, 0 };

        bool found = false;

        while (!frontier.empty() && !found)
        {
            GridPos current = frontier.front();
            frontier.pop();

            for (int d = 0; d < 4 && !found; d++)
            {
                GridPos next{ current.col + kDX[d], current.row + kDY[d] };

                if (!InBounds(next, gridWidth, gridHeight)) continue;
                if (visited[index(next)]) continue;
                if (!isWalkable(next.col, next.row)) continue;

                visited[index(next)] = true;
                parent[index(next)] = index(current);
                frontier.push(next);

                if (next == goal) found = true;
            }
        }

        if (!visited[index(goal)])
            return false;

        std::vector<GridPos> reversePath;
        int cur = index(goal);
        while (cur != index(start))
        {
            reversePath.push_back({ cur % gridWidth, cur / gridWidth });
            cur = parent[cur];
        }

        outPath.assign(reversePath.rbegin(), reversePath.rend());
        return true;
    }

    GridPos FindNextStepBFS(
        GridPos start,
        GridPos goal,
        int gridWidth,
        int gridHeight,
        IsWalkableFn isWalkable)
    {
        std::vector<GridPos> path;

        if (!FindPathBFS(start, goal, gridWidth, gridHeight, isWalkable, path))
            return start;

        if (path.empty())
            return start;

        return path[0];
    }
}
