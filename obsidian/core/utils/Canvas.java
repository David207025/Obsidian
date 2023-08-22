package obsidian.core.utils;

import java.awt.*;
import java.awt.image.BufferStrategy;
import java.time.Duration;
import java.time.Instant;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;

public class Canvas extends java.awt.Canvas {
    public BufferStrategy get_buffer_strategy() {
        return bs;
    }

    public Graphics2D get_graphics() {
        return g2d;
    }

    public int get_fps() {
        return fps;
    }

    private final Consumer<Void> renderingLoop;
    private BufferStrategy bs = getBufferStrategy();
    private Graphics2D g2d;


    private Color bg_color = new Color(0, 0, 0);

    public Dimension get_end_pixel_pos() {
        return new Dimension(getWidth() - 1, getHeight() - 1);
    }

    public void set_background_color(Color color) {
        this.bg_color = color;
    }
    private Thread thread;

    public Thread get_thread() {return thread;};
    private final AtomicBoolean running = new AtomicBoolean(false);
    private int fps;

    public Canvas(Consumer<Void> loop) {
        this.renderingLoop = loop;
    }

    @Override
    public void addNotify() {
        super.addNotify();
        createBufferStrategy(3);
        start();
    }

    @Override
    public void removeNotify() {
        stop();
        super.removeNotify();
    }

    public void start() {
        stop();
        run();
    }

    protected void stop() {
        if (running.get()) {
            running.set(false);
        }
        thread = null;
    }

    protected void run() {
        stop();

        running.set(true);
        thread = new Thread(new Runnable() {
            private Instant lastTick;
            private int frameCount;
            @Override
            public void run() {
                while (running.get()) {
                    if (lastTick != null) {
                        Duration duration = Duration.between(lastTick, Instant.now());
                        if (duration.toMillis() >= 1000) {
                            lastTick = Instant.now();
                            fps = frameCount;
                            frameCount = 0;
                        } else {
                            frameCount++;
                        }
                    } else {
                        lastTick = Instant.now();
                    }

                    render();

                    // Comment out these lines and see where it takes you
                    /*try {
                        Thread.sleep(0);
                    } catch (InterruptedException ex) {
                    }
                    */

                }
            }
        });
        thread.start();
    }

    protected void render() {
        this.isDoubleBuffered();
        while (bs == null) {
            bs = getBufferStrategy();

        }
        do {
            // The following loop ensures that the contents of the drawing buffer
            // are consistent in case the underlying surface was recreated
            do {
                // Get a new graphics context every time through the loop
                // to make sure the strategy is validated
                try {


                    g2d = (Graphics2D) bs.getDrawGraphics();
                    RenderingHints rh = new RenderingHints(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    g2d.setRenderingHints(rh);

                    g2d.setColor(bg_color);
                    g2d.fillRect(0, 0, this.getWidth(), this.getHeight());


                    renderingLoop.accept(null);
                    g2d.dispose();
                    // Dispose the graphics

                } catch (Exception e) {
                    System.out.println(e);
                }

                // Repeat the rendering if the drawing buffer contents
                // were restored
            } while (bs.contentsRestored());

            // Display the buffer
            if (this.isDisplayable()) {
                bs.show();
            }
            // Repeat the rendering if the drawing buffer was lost
        } while (bs.contentsLost());
    }
}

