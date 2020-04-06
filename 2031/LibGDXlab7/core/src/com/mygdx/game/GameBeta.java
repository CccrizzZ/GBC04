package com.mygdx.game;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputMultiplexer;

public abstract class GameBeta extends Game {

    private static GameBeta game;

    public GameBeta() {
        game = this;
    }

    @Override
    public void create()
    {
        InputMultiplexer im = new InputMultiplexer();

        Gdx.input.setInputProcessor( im );
    }

    public static void setActiveScreen(ScreenBeta s)
    {
        game.setScreen(s);
    }
}
