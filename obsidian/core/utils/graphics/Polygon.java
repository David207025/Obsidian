package obsidian.core.utils.graphics;

import java.util.ArrayList;

public class Polygon{

    public Polygon(ArrayList<Coord> coords) {
        this.points = coords.toArray(new Coord[0]);
    }

    public Coord[] get_points() {
        return points;
    }

    public void set_points(Coord[] points) {
        this.points = points;
    }

    private Coord[] points;

    public Polygon(Coord... points) {
        this.points = points;
    }

    public Polygon() {

    }
}