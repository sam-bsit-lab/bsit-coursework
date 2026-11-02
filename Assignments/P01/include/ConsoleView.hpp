#ifndef CONSOLEVIEW_HPP
#define CONSOLEVIEW_HPP

#include "GameView.hpp"
#include "VideoPoker.hpp"

class ConsoleView : public GameView
{
private:
    VideoPoker& game;

    void printHand() const;

public:
    ConsoleView(VideoPoker& game);

    void run() override;
};

#endif