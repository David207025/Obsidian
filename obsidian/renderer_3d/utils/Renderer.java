package obsidian.renderer_3d.utils;

import obsidian.core.utils.Graphics;

import java.awt.*;

public class Renderer {
    private final Matrix scaling_factor;

    private final Camera camera = new Camera(0, 0, -15);

    public Matrix camera_pos = new Matrix(new double[][]{{camera.getX()}, {camera.getY()}, {camera.getZ()}});
    public Renderer(Matrix scaling_factor) {
        this.scaling_factor = scaling_factor;
    }

    public Camera get_camera() {return camera;}


    public void render_mesh(Mesh mesh, Color color, boolean filled, Graphics graphics, Dimension windowSize) {

        camera_pos = new Matrix(new double[][]{{camera.getX()}, {camera.getY()}, {camera.getZ()}});
        Mesh m = new Mesh(new Matrix[mesh.getMatrices().length]);
        int[] x_points = new int[mesh.getMatrices().length];
        int[] y_points = new int[mesh.getMatrices().length];
        for (int i = 0; i < mesh.getMatrices().length; i++) {
            double camera_dist_z = 0.05 * (mesh.getMatrices()[i].get_points()[2][0] - camera_pos.get_points()[2][0]);
            double distance = (camera_dist_z * Math.sin(Math.toRadians(90))) / 2;

            double camera_dist_z_middle = 0.05*(mesh.get_middle_coordinates().get_points()[2][0] - camera_pos.get_points()[2][0]) * Math.sin(Math.toRadians(90)) / 2;

            if (camera_dist_z_middle > -1.5 && distance > -0.01) {
                Matrix scale = new Matrix(new double[][]{{scaling_factor.get_points()[0][0] * 60 / distance, scaling_factor.get_points()[0][1], scaling_factor.get_points()[0][2], 0.5}, {scaling_factor.get_points()[1][0], scaling_factor.get_points()[1][1] * 60 / distance, scaling_factor.get_points()[1][2], 0.5}});


                m.set_matrix(Matrix.Math.multiply(scale, new Matrix(new double[][]{{mesh.getMatrices()[i].get_points()[0][0] - camera_pos.get_points()[0][0]}, {mesh.getMatrices()[i].get_points()[1][0] - camera_pos.get_points()[1][0]}, {camera_dist_z}, {0}})), i);
                x_points[i] = (int) (m.getMatrices()[i].get_points()[0][0] + windowSize.getWidth() / 2);
                y_points[i] = (int) (m.getMatrices()[i].get_points()[1][0] + windowSize.getHeight() / 2);
            }
        }
        if (!filled) {
            graphics.draw_polygon(x_points, y_points, color);
        } else {
            graphics.fill_polygon(x_points, y_points, color);
        }
    }

    public void render_object(Object object, Color color, boolean filled, Graphics graphics, Dimension windowSize) {

            for (int j = 0; j < object.get_meshes().length; j++) {
                Mesh mesh = object.get_meshes()[j];

                render_mesh(mesh, color, filled, graphics, windowSize);

        }

    }


}
