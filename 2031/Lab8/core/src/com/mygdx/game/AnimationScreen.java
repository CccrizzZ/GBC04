package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

import sun.applet.Main;


public abstract class AnimationScreen extends ScreenBeta {

    ActorBeta bg;
    ActorBeta bg2;
    ActorBeta a1;
    MoveByAction a1move;
    MainScreen sc2;

    float scwidth = Gdx.graphics.getWidth();

    @Override
    public void initialize() {


        bg = new ActorBeta(0, 0, mainStage);
        bg.loadTexture("back.png");
        bg2 = new ActorBeta(Gdx.graphics.getWidth(), 0, mainStage);
        bg2.loadTexture("back.png");

        String[] texArr = {"run/run_0.png", "run/run_1.png", "run/run_2.png", "run/run_3.png", "run/run_4.png", "run/run_5.png", "run/run_6.png"};

        a1 = new ActorBeta(50,100,mainStage);
        Animation<TextureRegion> playerAnim = a1.loadAnimationFromFiles(texArr, 0.1f, true);
        a1.setAnimation(playerAnim);


        // a1
        a1move = new MoveByAction();
        a1move.setAmount(Gdx.graphics.getWidth()-a1.getX(), 0.0f);
        a1move.setDuration(5.0f);


        a1.addAction(a1move);
        sc2 = new MainScreen() {};

    }



    @Override
    public void update(float dt) {


        if (bg.getX() + bg.getWidth() < 0){
            bg.setPosition(Gdx.graphics.getWidth(),0);
        }else{
            float temp = bg.getX()-2;
            bg.setPosition(temp,0);
        }

        if (bg2.getX() + bg2.getWidth() < 0){
            bg2.setPosition(Gdx.graphics.getWidth(),0);
        }else{
            float temp2 = bg2.getX()-2;
            bg2.setPosition(temp2, 0);
        }



        if (a1.getX() > scwidth) {



            Game1.setActiveScreen(sc2);
        }

    }
}
