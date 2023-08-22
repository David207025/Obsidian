package obsidian.renderer_3d.utils;

public class Object {
    private Mesh[] meshes;

    public Object(Mesh[] meshes) {
        this.meshes = meshes;
    }

    public Mesh[] get_meshes() {
        return meshes;
    }

    public void set_meshes(Mesh[] meshes) {
        this.meshes = meshes;
    }

    public void set_mesh(Mesh m, int index) {
        meshes[index] = m;
    }
}
