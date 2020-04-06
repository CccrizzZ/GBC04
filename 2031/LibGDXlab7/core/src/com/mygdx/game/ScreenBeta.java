package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputMultiplexer;
import com.badlogic.gdx.InputProcessor;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.audio.Music;
import com.badlogic.gdx.audio.Sound;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.scenes.scene2d.Event;
import com.badlogic.gdx.scenes.scene2d.InputEvent;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.Button;
import com.badlogic.gdx.scenes.scene2d.ui.Button.ButtonStyle;
import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.Label.LabelStyle;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.graphics.g2d.NinePatch;
import com.badlogic.gdx.scenes.scene2d.utils.NinePatchDrawable;
import com.badlogic.gdx.utils.Align;
import com.badlogic.gdx.utils.viewport.FitViewport;


/**
 * Created by markapptist on 2018-10-16.
 */

public abstract class ScreenBeta implements Screen, InputProcessor {

    protected Stage mainStage;

    //UI

    //STAGE
    protected Stage uiStage;

    //TABLE TO ORGANIZE LAYOUT
    Table uiTable;

    //LABEL
    Label label;
    LabelStyle labelStyle;

    //BUTTON
    Button button;
    ButtonStyle buttonStyle;
    Texture buttonTex;
    TextureRegion buttonRegion;

    //SOUNDS
    Music defaultBackgroundMusic;
    Sound defaultSoundEffect;

    //BOOLEANS
    boolean isPaused;

    int score;

    //CONSTRUCTOR
    ScreenBeta()
    {

        isPaused = false;

        mainStage = new Stage();
        uiStage = new Stage();

        /***
         * TODO: USE THE TABLE BELOW TO SET THE BUTTONS ON BOTH START SCREEN AND GAME SCREEN
         */

        uiTable = new Table();
        uiTable.setFillParent(true);
        uiTable.align(Align.center);
        uiStage.addActor(uiTable);

        //INITIALIZE A DEFAULT BUTTON
        buttonStyle = new ButtonStyle();
        button = new Button(buttonStyle);
        button.setTransform(true);

        //INITIALIZE A LABEL
        labelStyle = new LabelStyle();
        labelStyle.fontColor = Color.BLACK;

//        label = new Label("LABEL", labelStyle);

        uiStage.addActor(button);
//        uiStage.addActor(label);

        //INITIALIZE TABLE
        /**
         * TODO: PLAY WITH THE TABLE UNTIL THINGS ARE ALIGNED PROPERLY ON BOTH SCREENS
         */
        uiTable.padTop(30);
        uiTable.add(button).padBottom(100);
        uiTable.row();
        uiTable.add(label);

        initialize();


    }

    public abstract void initialize();

    /**
     *  Called when this becomes the active screen in a Game.
     *  Set up InputMultiplexer here, in case screen is reactivated at a later time.
     */
    @Override
    public void show() {

        //GET the InputMultiplexer
        InputMultiplexer im = (InputMultiplexer)Gdx.input.getInputProcessor();

        //Add InputProcessor to the screen
        im.addProcessor(this);

        //Add InputProcessor to the stage
        im.addProcessor(mainStage);
        im.addProcessor(uiStage);
    }

    /**
     *  Called when this is no longer the active screen in a Game.
     *  Screen class and Stages no longer process input.
     *  Other InputProcessors must be removed manually.
     */
    @Override
    public void hide() {

        //Get InputProcessor
        InputMultiplexer im = (InputMultiplexer)Gdx.input.getInputProcessor();

        //Remove InputProcessor
        im.removeProcessor(this);
        im.removeProcessor(mainStage);
        im.removeProcessor(uiStage);
    }

    @Override
    public void resume() {

    }

    @Override
    public void pause() {

    }

    public abstract void update(float dt);

    @Override
    public void render(float delta) {

        score++;

        //PAUSE LOGIC
        if(isPaused)
            delta = 0;
        else {
            delta = Math.min(delta, 1/30.0f);
        }

        mainStage.act(delta);
        uiStage.act(delta);

        update(delta);
        mainStage.setDebugAll(true);

        Gdx.gl.glClearColor(0, 0.5f, 0.5f, 1.0f);
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

        mainStage.draw();
        uiStage.draw();

        uiTable.setDebug(true);
    }

    public boolean isTouchDownEvent(Event e) {
        return (e instanceof InputEvent) && ((InputEvent)e).getType().equals(InputEvent.Type.touchDown);
    }

    @Override
    public void resize(int width, int height) {

    }

    @Override
    public void dispose() {

    }

    @Override
    public boolean mouseMoved(int screenX, int screenY) {
        return false;
    }

    @Override
    public boolean touchDragged(int screenX, int screenY, int pointer) {

        return false;
    }

    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {

        return false;
    }

    @Override
    public boolean touchUp(int screenX, int screenY, int pointer, int button) {

        return false;
    }

    @Override
    public boolean keyDown(int keycode) {
        return false;
    }

    @Override
    public boolean keyUp(int keycode) {
        return false;
    }

    @Override
    public boolean keyTyped(char character) {
        return false;
    }

    @Override
    public boolean scrolled(int amount) {
        return false;
    }
}
