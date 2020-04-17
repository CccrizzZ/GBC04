package com.arkanoid.game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.scenes.scene2d.Actor;
import com.badlogic.gdx.scenes.scene2d.Group;
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

    Group bricks;

    MoveByAction ballMovement;


    int score;

    float BallVelocityX;
    float BallVelocityY;

    @Override
    public void initialize() {

        batch = new SpriteBatch();
        font = new BitmapFont();
        font.getData().setScale(5,5);
        score = 0;

        BallVelocityX = 10;
        BallVelocityY = 10;

        bg = new ActorBeta(0,0,mainStage);
        bg.loadTexture("bg.png");
        bg.sizeBy(Gdx.graphics.getWidth(),Gdx.graphics.getHeight());

        player = new ActorBeta(200,50, mainStage);
        player.loadTexture("player.png");
        player.sizeBy(200,40);


        ball = new ActorBeta(200,50+player.getX()/2,mainStage);
        ball.loadTexture("ball.png");
        ball.sizeBy(30);




        ballMovement = new MoveByAction();
        ballMovement.setAmount(0,-500);
        ballMovement.setDuration(5);

        RepeatAction repeatAction = new RepeatAction();
        repeatAction.setCount(RepeatAction.FOREVER);
        repeatAction.setAction(ballMovement);
//        ball.addAction(repeatAction);

        bricks = new Group();

        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 5; j++){
                brick = new ActorBeta(200*j+Gdx.graphics.getWidth()/14, 100*i+1000, mainStage);

                brick.loadTexture("bricks.png");
                brick.sizeBy(Gdx.graphics.getWidth()/12,30);
                bricks.addActor(brick);
            }
        }


    }


    @Override
    public void update(float dt) {


        ball.setX(ball.getX()+BallVelocityX);
        ball.setY(ball.getY()+BallVelocityY);



        if (ball.getY() + ball.getHeight()/2 >= player.getY()
                && ball.getX() + ball.getWidth()/2 >= player.getX()
                && ball.getX() <= player.getX() + player.getWidth()
                && ball.getY() <= player.getY()+player.getHeight()){
            reboundY();
        }


        if (ball.getY()+ball.getHeight()>Gdx.graphics.getHeight()){
            reboundY();
        }

        if (ball.getX()+ball.getWidth()>Gdx.graphics.getWidth()){
            reboundX();
        }

        if (ball.getX() < 0 && ball.getY() > 0){
            reboundX();
        }

        if (ball.getY() < 0){
            ArkanoidGame.setActiveScreen(new DeathScreen(){});
        }


        for(Actor ab: bricks.getChildren()) {
            if (    ball.getY() + ball.getHeight()/2 >= ab.getY()
                    && ball.getX() + ball.getWidth()/2 >= ab.getX()
                    && ball.getX() <= ab.getX() + ab.getWidth()
                    && ball.getY() <= ab.getY()+ab.getHeight()
            ){
                ab.remove();
                score+=100;
            }


        }

    }

    @Override
    public void render(float delta) {
        super.render(delta);

        batch.begin();
        font.draw(batch, "Score", Gdx.graphics.getWidth() / 2 - "Score".length()*16, 1700);
        font.draw(batch, Integer.toString(score), Gdx.graphics.getWidth() / 2 - Integer.toString(score).length()*16, 1600);
        bricks.draw(batch, 1.0f);
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




    public void reboundX(){

        BallVelocityX = -BallVelocityX;
    }

    public void reboundY(){
        BallVelocityY = -BallVelocityY;
    }

    @Override
    public void dispose() {
        super.dispose();
        if (batch != null)
            batch.dispose();
        batch = null;
    }
}
