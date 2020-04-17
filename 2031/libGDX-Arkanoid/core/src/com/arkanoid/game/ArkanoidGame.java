package com.arkanoid.game;

public class ArkanoidGame extends GameBeta {
    public MainGameScreen s1;
    public DeathScreen ds;

    @Override
    public void create() {
        super.create();

        s1 = new MainGameScreen(){};
        ds = new DeathScreen() {};
        setActiveScreen(s1);
    }
}
