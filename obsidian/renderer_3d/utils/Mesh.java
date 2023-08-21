package obsidian.renderer_3d.utils;

public class Mesh {
    private Matrix[] matrices;

    public Mesh(Matrix[] matrices) {
        this.matrices = matrices;
    }

    public Matrix[] getMatrices() {
        return matrices;
    }

    public void setMatrices(Matrix[] matrices) {
        this.matrices = matrices;
    }

    public void set_matrix(Matrix m, int index) {
        matrices[index] = m;
    }

    public Matrix get_middle_coordinates() {
        double middle_x = 0;
        double middle_y = 0;
        double middle_z = 0;
        for (int i = 0; i < matrices.length; i++) {
            middle_x += matrices[i].get_points()[0][0];
            middle_y += matrices[i].get_points()[1][0];
            middle_z += matrices[i].get_points()[2][0];
        }
        return new Matrix(new double[][]{{middle_x}, {middle_y}, {middle_z}});
    }
}
