package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.scenes.scene2d.actions.MoveByAction;
import com.badlogic.gdx.scenes.scene2d.actions.ParallelAction;
import com.badlogic.gdx.scenes.scene2d.actions.RepeatAction;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.scenes.scene2d.actions.RotateByAction;
import com.badlogic.gdx.scenes.scene2d.actions.ScaleByAction;
import com.badlogic.gdx.scenes.scene2d.actions.SequenceAction;


public abstract class MainScreen extends ScreenBeta {
    ActorBeta gdxHappyFace;
    ActorBeta sonic;
    ActorBeta bg;
    ActorBeta bg2;
    ActorBeta a1;
    ActorBeta a2;
    ActorBeta a3;

    MoveByAction a1move;
    RotateByAction rot180;
    ScaleByAction scale2;
    MoveByAction move;

    RotateByAction rot180_2;
    ScaleByAction scale2_2;
    MoveByAction move_2;

    RotateByAction a1Rot;


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

        a2 = new ActorBeta(50,400, mainStage);
        a2.setAnimation(playerAnim);

        a3 = new ActorBeta(50, 700, mainStage);
        a3.setAnimation(playerAnim);




        rot180 = new RotateByAction();
        rot180.setAmount(180);
        rot180.setDuration(3);

        scale2 = new ScaleByAction();
        scale2.setAmount(2);
        scale2.setDuration(5.0f);

        move = new MoveByAction();
        move.setAmount(Gdx.graphics.getWidth(), 0.0f);
        move.setDuration(5.0f);

        rot180_2 = new RotateByAction();
        rot180_2.setAmount(180);
        rot180_2.setDuration(3);

        scale2_2 = new ScaleByAction();
        scale2_2.setAmount(2);
        scale2_2.setDuration(3.0f);

        move_2 = new MoveByAction();
        move_2.setAmount(Gdx.graphics.getWidth(), 0.0f);
        move_2.setDuration(3.0f);

        ParallelAction a2Action = new ParallelAction(scale2, rot180, move);
        SequenceAction a3Action = new SequenceAction(scale2_2, rot180_2, move_2);

        // a1
        a1Rot = new RotateByAction();
        a1Rot.setAmount(180);
        a1Rot.setDuration(3);
        a1move = new MoveByAction();
        a1move.setAmount(Gdx.graphics.getWidth()-a1.getX(), 0.0f);
        a1move.setDuration(8.0f);
        RepeatAction repeatAction = new RepeatAction();
        repeatAction.setCount(RepeatAction.FOREVER);
        repeatAction.setAction(a1move);

        a1.addAction(repeatAction);
        a3.addAction(a3Action);
        a2.addAction(a2Action);

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



        if (a1.getX() > Gdx.graphics.getWidth()){
            a1move.setAmount(-1000,0);
            a1.addAction(a1Rot);
        }
        if (a1.getX()<0){
            a1move.setAmount(1000,0);
        }
    }
}
