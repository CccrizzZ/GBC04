package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.scenes.scene2d.actions.MoveToAction;
import com.badlogic.gdx.scenes.scene2d.actions.RepeatAction;


public abstract class MainScreen3 extends ScreenBeta {
    BitmapFont font;
    SpriteBatch batch;

    ActorBeta bg;
    ActorBeta bg2;

    ActorBeta gdxHappyFace;
    ActorBeta sonic;

    MoveByAction moveBy;
    MoveByAction moveBy2;

    @Override
    public void initialize() {
        bg = new ActorBeta(0, 0, mainStage);
        bg.loadTexture("back.png");
        bg2 = new ActorBeta(Gdx.graphics.getWidth(), 0, mainStage);
        bg2.loadTexture("back.png");

        batch = new SpriteBatch();
        font = new BitmapFont();
        font.getData().setScale(5,5);



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
        moveBy.setAmount(100.0f, 100.0f);
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

    }


    @Override
    public void render(float delta) {
        super.render(delta);

        batch.begin();
        font.draw(batch, "Collided", Gdx.graphics.getWidth() / 2, 500);
        batch.end();
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




        if (gdxHappyFace.getX() + gdxHappyFace.getWidth()/5 >= sonic.getX() && gdxHappyFace.getY() + gdxHappyFace.getHeight()/5 >= sonic.getY()) {
            gdxHappyFace.remove();
            sonic.remove();
        }



    }
}
