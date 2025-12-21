#pragma once

#include <chrono>
#include <thread>

void clearTerminal() {
    // clear screen and move cursor to home position
    std::cout << "\033[2J\033[H"; 
}

void overwriteTerminal() {
    // Move cursor to home position
    std::cout << "\033[H";   
}

void delay(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}