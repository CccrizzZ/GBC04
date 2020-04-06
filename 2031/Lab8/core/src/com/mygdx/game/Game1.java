package com.mygdx.game;

public class Game1 extends GameBeta {

    AnimationScreen sc1;
    @Override
    public void create() {
        super.create();

        sc1 = new AnimationScreen() {};

        setActiveScreen(sc1);
    }

}
