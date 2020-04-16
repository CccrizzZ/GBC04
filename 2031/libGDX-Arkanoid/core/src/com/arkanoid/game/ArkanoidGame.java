package com.arkanoid.game;

public class ArkanoidGame extends GameBeta {
    MainGameScreen s1;

    @Override
    public void create() {
        super.create();

        s1 = new MainGameScreen(){};
        setActiveScreen(s1);
    }
}
