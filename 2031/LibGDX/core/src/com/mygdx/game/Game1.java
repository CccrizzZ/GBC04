package com.mygdx.game;

public class Game1 extends GameBeta {

    MainScreen sc1;

    @Override
    public void create() {
        super.create();

        sc1 = new MainScreen() {};


        setActiveScreen(sc1);
    }

}
