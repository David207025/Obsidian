package obsidian.core;

import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import obsidian.core.utils.graphics.Polygon;

import javax.imageio.ImageIO;

public class Sprite {
    public BufferedImage bits = new BufferedImage(1, 1, BufferedImage.TYPE_INT_RGB);
    public Rectangle2D area = new Rectangle2D.Double(0, 0, 0, 0);
    private Graphics2D g;
    public String name = "";

    public void load(String address) {
        try {
            bits = ImageIO.read(new File(new File(address).getAbsolutePath()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public void crop_from_sprite(Sprite sheet, int x, int y, int w, int h) {
        if (x > sheet.bits.getWidth(null) || x < 0)
            return;
        else if (w > sheet.bits.getWidth(null) || w < 0)
            return;
        else if (y > sheet.bits.getHeight(null) || y < 0)
            return;
        else if (h > sheet.bits.getHeight(null) || h < 0)
            return;
        bits = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        area = new Rectangle2D.Double(0, 0, w, h);

        for (int ph = 0; ph < h; ph++) {
            for (int pw = 0; pw < w; pw++) {
                Color c = new Color(sheet.bits.getRGB(x + pw, y + ph), true);

                bits.setRGB(pw, ph, c.getRGB());

            }
        }

    }

    public void resize_sprite(int newWidth, int newHeight) {
        Image newImg = bits.getScaledInstance(newWidth, newHeight, BufferedImage.SCALE_SMOOTH);
        BufferedImage bimage = new BufferedImage(newImg.getWidth(null), newImg.getHeight(null), BufferedImage.TYPE_INT_ARGB);

        // Draw the image on to the buffered image
        Graphics2D bGr = bimage.createGraphics();
        bGr.drawImage(newImg, 0, 0, null);
        bGr.dispose();
        area = new Rectangle2D.Double(0, 0, newWidth, newHeight);

        bits = bimage;

    }

    public void create(int x, int y, String name) {
        this.bits = new BufferedImage(x, y, BufferedImage.TYPE_INT_ARGB);
        area = new Rectangle2D.Double(0, 0, x, y);
        this.name = name;
        this.g = (Graphics2D)bits.getGraphics();
    }

    public void set_name(String name) { this.name = name; };

    public void draw_pixel(int x, int y, Color color) {
        g.setColor(color);
        g.fill(new java.awt.Rectangle(x, y, 1, 1));

    }

    public void draw_line(int x1, int y1, int x2, int y2, Color color) {
        g.setColor(color);
        g.drawLine(x1, y1, x2, y2);
    }

    public void draw_polygon(int x[], int y[], Color color) {
        g.setColor(color);
        g.drawPolygon(x, y, x.length);
    }

    public void draw_oval(int x, int y, int x_radius, int y_radius, Color color) {
        g.setColor(color);

        g.drawOval(x, y, x_radius, y_radius);
    }

    public void fill_polygon(int x[], int y[], Color color) {
        g.setColor(color);
        g.fillPolygon(x, y, x.length);

    }

    public void fill_oval(int x, int y, int x_radius, int y_radius, Color color) {
        g.setColor(color);

        g.fillOval(x, y, x_radius, y_radius);
    }

    public void draw_polygon(Polygon polygon, Color color) {
        g.setColor(color);
        java.awt.Polygon poly = new java.awt.Polygon();

        for (int i = 0; i < polygon.get_points().length; i++) {
            poly.addPoint((int)polygon.get_points()[i].get_x(), (int)polygon.get_points()[i].get_y());
        }
        g.draw(poly);

    }

    public void fill_polygon(Polygon polygon, Color color) {
        g.setColor(color);
        java.awt.Polygon poly = new java.awt.Polygon();
        for (int i = 0; i < polygon.get_points().length; i++) {
            poly.addPoint((int)polygon.get_points()[i].get_x(), (int)polygon.get_points()[i].get_y());
        }
        g.fill(poly);

    }

}