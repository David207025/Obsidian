package obsidian.renderer_3d.utils;

public class Matrix {
    private double[][] points = {{0}};
    private int columns = points[0].length;
    private int rows = points.length;
    public Matrix() {
        this.points = new double[][]{{0}};
        this.columns = points[0].length;
        this.rows = points.length;
    }

    public Matrix(double[][] pts) {
        this.points = pts;
        this.columns = points[0].length;
        this.rows = points.length;
    }

    public double[][] get_points() {
        return points;
    }

    public void set_points(double[][] points) {
        this.points = points;
    }

    public int get_columns() {
        return columns;
    }

    public int get_rows() {
        return rows;
    }

    public static class Math {
        public static Matrix multiply(Matrix m1, Matrix m2) {
            if (m1.columns != m2.rows) return null;
            double[][] result = new double[m1.rows][m2.columns];

            for (int r1 = 0; r1 < m1.rows; r1++) {
                for (int c1 = 0; c1 < m1.columns; c1++) {
                    for (int c2 = 0; c2 < m2.columns; c2++) {
                        result[r1][c2] += m1.get_points()[r1][c1] * m2.get_points()[c1][c2];
                    }
                }
            }
            return new Matrix(result);
        }
    }
}
