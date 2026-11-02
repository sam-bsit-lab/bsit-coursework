#pragma once

#include "GameView.hpp"
#include "VideoPoker.hpp"

class TuiView : public GameView
{
private:
    VideoPoker& game;

public:
    explicit TuiView(VideoPoker& game);
    void run() override;
};
