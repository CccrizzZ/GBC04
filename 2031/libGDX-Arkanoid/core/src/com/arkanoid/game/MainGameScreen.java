package com.arkanoid.game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.scenes.scene2d.actions.RepeatAction;

import java.lang.reflect.Array;
import java.util.Arrays;

public abstract class MainGameScreen extends ScreenBeta {
    BitmapFont font;
    SpriteBatch batch;


    ActorBeta player;
    ActorBeta ball;
    ActorBeta bg;

    ActorBeta brick;
    ActorBeta bricks[];


    MoveByAction ballMovement;


    int score;

    @Override
    public void initialize() {

        batch = new SpriteBatch();
        font = new BitmapFont();
        font.getData().setScale(5,5);
        score = 0;

        bg = new ActorBeta(0,0,mainStage);
        bg.loadTexture("bg.png");
        bg.sizeBy(Gdx.graphics.getWidth(),Gdx.graphics.getHeight());

        player = new ActorBeta(200,50, mainStage);
        player.loadTexture("player.png");
        player.sizeBy(200,40);


        ball = new ActorBeta(200,150,mainStage);
        ball.loadTexture("ball.png");
        ball.sizeBy(30);

        ballMovement = new MoveByAction();
        ballMovement.setAmount(0,-500);
        ballMovement.setDuration(5);

        RepeatAction repeatAction = new RepeatAction();
        repeatAction.setCount(RepeatAction.FOREVER);
        repeatAction.setAction(ballMovement);
        ball.addAction(repeatAction);

        bricks = new ActorBeta[20];

        int temp = 0;
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 5; j++){
                brick = new ActorBeta(200*j+Gdx.graphics.getWidth()/14, 100*i+1000, mainStage);
                brick.loadTexture("bricks.png");
                brick.sizeBy(Gdx.graphics.getWidth()/12,30);
                bricks[temp] = brick;
                temp++;

            }
        }
    }


    @Override
    public void update(float dt) {
        score++;

        if (ball.getY()<player.getY()+player.getHeight()){
            ballMovement.setAmount(0,1600);
            ballMovement.setDuration(5);
        }
        if (ball.getY()+ball.getHeight()>Gdx.graphics.getHeight()){
            ballMovement.setAmount(0,-500);
            ballMovement.setDuration(5);

        }
    }

    @Override
    public void render(float delta) {
        super.render(delta);


        batch.begin();
        font.draw(batch, "Score", Gdx.graphics.getWidth() / 2 - "Score".length()*16, 1700);
        font.draw(batch, Integer.toString(score), Gdx.graphics.getWidth() / 2 - Integer.toString(score).length()*16, 1600);
        batch.end();
    }

    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {


        return super.touchDown(screenX, screenY, pointer, button);
    }

    @Override
    public boolean touchDragged(int screenX, int screenY, int pointer) {

        player.setPosition(screenX,50);

        if (player.getX() > Gdx.graphics.getWidth()-player.getWidth()){
            player.setPosition(Gdx.graphics.getWidth()-player.getWidth(),50);
        }
        if (player.getX() < 0){
            player.setPosition(0,50);
        }
        if (ball.getX() < 0){
            ball.setX(0);
        }
        if (ball.getX() > Gdx.graphics.getWidth() - ball.getWidth()){
            ball.setX(Gdx.graphics.getWidth() - ball.getWidth());
        }


        return super.touchDragged(screenX, screenY, pointer);
    }




    public void rebound(){

    }
}
