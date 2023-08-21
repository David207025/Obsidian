package obsidian.core.utils.graphics;

import obsidian.core.utils.graphics.Matrix;

public class Coord {
    private double x = 0;
    private double y = 0;

    public double get_x() {
        return x;
    }

    public void set_x(double x) {
        this.x = x;
    }

    public double get_y() {
        return y;
    }

    public void set_y(double y) {
        this.y = y;
    }

    public Coord(double x, double y) {
        this.x = x;
        this.y = y;
    }
    public Coord() {
        x = 0;
        y = 0;
    }

    public Coord(Matrix mat, int x_index, int y_index) {
        x = mat.get_points()[x_index][0];
        y = mat.get_points()[y_index][0];
    }

}