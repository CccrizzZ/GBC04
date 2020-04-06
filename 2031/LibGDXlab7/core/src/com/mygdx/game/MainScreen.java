package com.mygdx.game;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.scenes.scene2d.actions.MoveToAction;
import com.badlogic.gdx.scenes.scene2d.actions.RepeatAction;


public abstract class MainScreen extends ScreenBeta {
    ActorBeta gdxHappyFace;
    ActorBeta sonic;
    MoveByAction moveBy ;

    MainScreen2 sc2;

    @Override
    public void initialize() {
        gdxHappyFace = new ActorBeta(Gdx.graphics.getWidth() / 2, Gdx.graphics.getHeight() / 2, mainStage);

        gdxHappyFace.loadTexture("badlogic.jpg");

        sonic = new ActorBeta(Gdx.graphics.getWidth() / 2, Gdx.graphics.getHeight() / 2, mainStage);

        sonic.loadTexture("sonic.png");

        sonic.setScale(0.5f);

        sonic.setPosition(Gdx.graphics.getWidth() / 5, Gdx.graphics.getHeight() / 5);

        MoveToAction moveTo = new MoveToAction();
        moveTo.setPosition(Gdx.graphics.getWidth() / 7, Gdx.graphics.getHeight() / 7);
        moveTo.setDuration(1.0f);
        //   gdxHappyFace.addAction(moveTo);

        moveBy = new MoveByAction();
        moveBy.setAmount(400.0f, 0.0f);
        moveBy.setDuration(1.2f);

        RepeatAction repeatAction = new RepeatAction();
        repeatAction.setCount(RepeatAction.FOREVER);
        repeatAction.setAction(moveBy);
        gdxHappyFace.addAction(repeatAction);

        sc2 = new MainScreen2() {

        };
    }


    @Override
    public void update(float dt) {
        if (gdxHappyFace.getX() > Gdx.graphics.getWidth()){

            Game1.setActiveScreen(sc2);
        }



    }
}
