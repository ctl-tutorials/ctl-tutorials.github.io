// filter kernel - with three additional arguments
kernel void filter( read_only image3d_t oldVol,
                    global float* newVol,
                    uint z,
                    float thresh1,
                    float thresh2,
                    float thresh3)
{
    // get IDs
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int4 vox = (int4)(x, y, z, 0);

    // categorize voxel
    const float refVal = read_imagef(oldVol, vox).x;
    float newVal = 0.0;
    if(refVal < thresh1)
        newVal = 0.0f;
    else if(refVal < thresh2)
        newVal = 1.0f;
    else if(refVal < thresh3)
        newVal = 2.0f;

    //void write_bufferf(global float* buffer, int4 coord, float color, image3d_t image)
    write_bufferf(newVol, vox, newVal, oldVol);
}
