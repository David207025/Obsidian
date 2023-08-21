package obsidian.core;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.awt.event.ActionListener;

import obsidian.core.utils.Graphics;
import obsidian.core.utils.Canvas;
import obsidian.core.utils.Input;


class ActionInput implements ActionListener {
    public ArrayList<Component> objects = new ArrayList<>();
    public ArrayList<Integer> activatedApps = new ArrayList<>();
    private obsidian.core.Frame window;
    public ActionInput(Frame win) {
        this.window = win;
    }
    public void testForAction() {

    }
    @Override
    public void actionPerformed(ActionEvent e) {
        for (int i = 0; i < objects.size(); i++) {
            if (e.getSource() == objects.get(i)) {
                activatedApps.add(i);
            }
        }
    }
}

public abstract class Frame {
    private JFrame frame = new JFrame();
    private Canvas canvas;

    public JFrame getFrame() {
        return frame;
    }

    public ActionInput get_action_input() {
        return actionInput;
    }

    public Input get_key_input() {
        return keyInput;
    }
    private ActionInput actionInput = new ActionInput(this);

    private static GraphicsEnvironment environment = GraphicsEnvironment.getLocalGraphicsEnvironment();
    private static Rectangle bounds = environment.getMaximumWindowBounds();

    public static Dimension get_max_window_size() {
        return new Dimension(bounds.width, bounds.height);
    }
    private boolean is_open = false;
    private String title = "";
    private final Input keyInput = new Input();
    private boolean debugging = false;

    public boolean is_open() {
        return is_open;
    }

    public boolean debugging() {
        return debugging;
    }



    public void set_debugging(boolean debugging) {
        this.debugging = debugging;
    }

    public Dimension get_frame_size() {
        return new Dimension(this.frame.getWidth(), this.frame.getHeight());
    }
    public String get_name() {   return title;    };

    public Input get_input() {
        return keyInput;
    }

    public JFrame get_frame() {return this.frame;}
    public int get_fps() {return this.canvas.get_fps();};



    public abstract void create();
    public abstract void paint(Graphics g);

    public abstract void destroy();

    public void start(int width,
                     int height,
                     String title,
                     Color bg_color, boolean stop_on_frame_close) {
        this.is_open = true;
        this.title = title;

        this.frame.setVisible(true);
        this.frame.setSize(width, height);

        this.frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                destroy();
                if (stop_on_frame_close) {
                    System.exit(0);
                }
            }
        });

        this.frame.setLayout(null);
        this.frame.addKeyListener(keyInput);
        try {
            create();
        } catch (Exception e) {}
        this.frame.setVisible(true);
        this.frame.setResizable(true);
        this.frame.setLocation(-7, 0);
        this.frame.setTitle(title);
        this.frame.setVisible(true);
        this.canvas = new Canvas(p -> paint(new Graphics(this.get_frame_size().width - 17, this.get_frame_size().height - 40, this.canvas.get_graphics())));
        this.canvas.set_background_color(bg_color);
        this.canvas.setBounds(0, 0, this.get_frame_size().width, this.get_frame_size().height - 37);

        this.frame.add(canvas);
        this.frame.addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                canvas.setPreferredSize(new Dimension(frame.getSize().width, frame.getSize().height - 37));
                canvas.setBounds(0, 0, frame.getSize().width, frame.getSize().height - 37);
            }
        });

    }
}
