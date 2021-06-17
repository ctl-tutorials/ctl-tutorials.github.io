// filter kernel - with three additional arguments
kernel void filter( read_only image3d_t oldVol,
                    global float* newVol,
                    uint z,
                    global float* thresholds,
                    uint numThresholds)
{
    // get IDs
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int4 vox = (int4)(x, y, z, 0);

    // categorize voxel
    const float refVal = read_imagef(oldVol, vox).x;
    float newVal = 0.0;
    for(uint thr = 0; thr < numThresholds; ++thr)
        if(refVal > thresholds[thr])
            newVal = (float) (thr+1);

    //void write_bufferf(global float* buffer, int4 coord, float color, image3d_t image)
    write_bufferf(newVol, vox, newVal, oldVol);
}
