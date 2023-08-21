package obsidian.core.utils;

import obsidian.core.Sprite;
import obsidian.core.utils.graphics.Polygon;
import obsidian.core.Frame;

import java.awt.*;

public class Graphics {

    public Graphics2D get_graphics() {
        return graphics2d;
    }

    public void set_graphics(Graphics2D graphics2d) {
        this.graphics2d = graphics2d;
    }


    private Graphics2D graphics2d;

    public Graphics(int width, int height, Graphics2D graphics) {
        this.graphics2d = graphics;
    }

    public void draw_pixel(int x, int y, Color color) {
        graphics2d.setColor(color);
        graphics2d.fill(new java.awt.Rectangle( x, y, 1, 1));
    }

    public void draw_line(int x1, int y1, int x2, int y2, Color color) {
        graphics2d.setColor(color);
        graphics2d.drawLine(x1, y1, x2, y2);
    }

    public void draw_polygon(int x[], int y[], Color color) {
        graphics2d.setColor(color);
        graphics2d.drawPolygon(x, y, x.length);
    }

    public void draw_oval(int x, int y, int x_radius, int y_radius, Color color) {
        graphics2d.setColor(color);

        graphics2d.drawOval(x, y, x_radius, y_radius);
    }

    public void fill_polygon(int x[], int y[], Color color) {
        graphics2d.setColor(color);
        if (x.length != y.length) return;
        graphics2d.fillPolygon(x, y, x.length);

    }

    public void fill_frame(Color color, Frame frame) {
        graphics2d.setColor(color);
        graphics2d.fillPolygon(new int[] {0, 0, frame.get_frame_size().width, frame.get_frame_size().width}, new int[] {0, frame.get_frame_size().height, frame.get_frame_size().height,0}, 4);
    }

    public void fill_oval(int x, int y, int x_radius, int y_radius, Color color) {
        graphics2d.setColor(color);

        graphics2d.fillOval(x, y, x_radius, y_radius);
    }

    public void draw_polygon(Polygon polygon, Color color) {
        graphics2d.setColor(color);
        java.awt.Polygon poly = new java.awt.Polygon();

        for (int i = 0; i < polygon.get_points().length; i++) {
            poly.addPoint((int)polygon.get_points()[i].get_x(), (int)polygon.get_points()[i].get_y());
        }
        graphics2d.draw(poly);

    }

    public void fill_polygon(Polygon polygon, Color color) {
        graphics2d.setColor(color);
        java.awt.Polygon poly = new java.awt.Polygon();
        for (int i = 0; i < polygon.get_points().length; i++) {
            poly.addPoint((int)polygon.get_points()[i].get_x(), (int)polygon.get_points()[i].get_y());
        }
        graphics2d.fill(poly);

    }

    public void draw_sprite(int x, int y, Sprite texture) {
        graphics2d.drawImage(texture.bits, x, y, null);
    }

    public void draw_text(int x, int y, String text, Font font, Color c) {
        graphics2d.setPaint(c);
        graphics2d.setFont(font);
        graphics2d.drawString(text, x, y);
    }
}