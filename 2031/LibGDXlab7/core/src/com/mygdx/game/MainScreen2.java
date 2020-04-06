package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.scenes.scene2d.actions.MoveToAction;
import com.badlogic.gdx.scenes.scene2d.actions.RepeatAction;

public abstract class MainScreen2 extends ScreenBeta {
    ActorBeta gdxHappyFace;
    ActorBeta sonic;
    ActorBeta bg;

    MoveByAction moveBy;
    MoveByAction moveBy2;

    MainScreen3 sc3;

    @Override
    public void initialize() {
        bg = new ActorBeta(0, 0, mainStage);
        bg.loadTexture("back.png");

        gdxHappyFace = new ActorBeta(0, Gdx.graphics.getHeight() / 2, mainStage);
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
        moveBy.setAmount(100.0f, 0.0f);
        moveBy.setDuration(1.2f);

        RepeatAction repeatAction = new RepeatAction();
        repeatAction.setCount(RepeatAction.FOREVER);
        repeatAction.setAction(moveBy);
        gdxHappyFace.addAction(repeatAction);

        moveBy2 = new MoveByAction();
        moveBy2.setAmount(0.0f, 100.0f);
        moveBy2.setDuration(1.2f);

        RepeatAction repeatAction2 = new RepeatAction();
        repeatAction2.setCount(RepeatAction.FOREVER);
        repeatAction2.setAction(moveBy2);
        sonic.addAction(repeatAction2);

        sc3 = new MainScreen3(){

        };
    }

    @Override
    public void update(float dt) {
        if (
                gdxHappyFace.getX() + gdxHappyFace.getWidth()/4 >= sonic.getX() &&
                gdxHappyFace.getY() + gdxHappyFace.getHeight()/4 >= sonic.getY()
        ) {
            Game1.setActiveScreen(sc3);
        }
    }
}
