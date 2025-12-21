#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "ComponentPool.hpp"
#include "TextRenderComponent.hpp"
#include "PositionComponent.hpp"

static constexpr int HORIZONTAL_START = -10;
static constexpr int HORIZONTAL_END = 10;
static constexpr int VERTICAL_START = -10;
static constexpr int VERTICAL_END = 10;

/**
 * @note: when two entities overlap on the same position in the grid,
 *      the latter one in the for loop is the one that gets displayed
 */
void textGridRenderSystem(
    ComponentPool<TextRenderComponent>& renderPool,
    ComponentPool<PositionComponent>& positionPool
) {
    const int num_columns = HORIZONTAL_END - HORIZONTAL_START + 1;
    const int num_rows = VERTICAL_END - VERTICAL_START + 1;

    std::vector<std::string> renderGrid(num_rows, std::string(num_columns, ' '));

    for (std::size_t i = 0; i < renderPool.getSize(); ++i) {
        Entity e = renderPool.entities[i];
        
        if (!positionPool.has(e)) continue;

        auto& renderData = renderPool.data[i];
        auto& position = positionPool.get(e);

        int x = static_cast<int>(position.x);
        int y = static_cast<int>(position.y);

        bool x_in_range = (x >= HORIZONTAL_START && x <= HORIZONTAL_END);
        bool y_in_range = (y >= VERTICAL_START && y <= VERTICAL_END);

        if (x_in_range && y_in_range) {
            // we need to map the (x, y) position into grid coordinates
            int col_index = x - HORIZONTAL_START;
            int row_index = VERTICAL_END - y;

            renderGrid[row_index][col_index] = renderData.symbol;
        }
    }

    int padded_num_columns = num_columns * 2 + 2;

    std::cout << std::string(padded_num_columns, '-') << '\n';

    for (std::string& row : renderGrid) {
        std::string padded_row;
        padded_row.reserve(row.size() * 2);
        padded_row += '|';
        for (char col : row) {
            padded_row += col;
            padded_row += ' ';
        }
        padded_row += '|';
        std::cout << padded_row << '\n';
    }
    
    std::cout << std::string(padded_num_columns, '-') << '\n';
}