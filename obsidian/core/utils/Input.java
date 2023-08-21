package obsidian.core.utils;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class Input implements KeyListener {
    private boolean[] key_pressed = new boolean[2500];

    public boolean getKey(int keyCode) {
        return key_pressed[keyCode];
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    @Override
    public void keyPressed(KeyEvent e) {
        key_pressed[e.getKeyCode()] = true;
    }

    @Override
    public void keyReleased(KeyEvent e) {
        key_pressed[e.getKeyCode()] = false;
    }
}