package com.arkanoid.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public abstract class DeathScreen extends ScreenBeta {
    BitmapFont font;
    SpriteBatch batch;

    @Override
    public void initialize() {

        batch = new SpriteBatch();
        font = new BitmapFont();
        font.getData().setScale(5,5);

    }

    @Override
    public void update(float dt) {

    }

    @Override
    public void render(float delta) {
        super.render(delta);
        Gdx.gl.glClearColor( 0.5f, 0.2f, 0.2f, 1 );
        Gdx.gl.glClear( GL20.GL_COLOR_BUFFER_BIT | GL20.GL_DEPTH_BUFFER_BIT );
        batch.begin();
        font.draw(batch, "Wasted", Gdx.graphics.getWidth() / 2 - "Wasted".length()*18, 1200);
        font.draw(batch, "Press screen to restart", Gdx.graphics.getWidth() / 2 - "Press screen to restart".length()*16, 1100);
        batch.end();

    }

    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {
        ArkanoidGame.setActiveScreen(new MainGameScreen() {});
        return super.touchDown(screenX, screenY, pointer, button);
    }
}
