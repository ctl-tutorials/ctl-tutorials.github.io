// the filter kernel function - with four additional arguments
kernel void filter( read_only image3d_t oldProj,
                    global float* newProj,
                    uint view,
                    float rectTopLeftX,
                    float rectTopLeftY,
                    float rectBottomRightX,
                    float rectBottomRightY)
{
    // get IDs
    const int u = get_global_id(0);
    const int v = get_global_id(1);

    // single module case (using auto combine in host code)
    const int4 pix = (int4)(u, v, 0, 0);

    // set pixels outside the rect to 0.0
    if(u < rectTopLeftX || u > rectBottomRightX || v < rectTopLeftY || v > rectBottomRightY)
        write_bufferf(newProj, pix, 0.0f, oldProj);
    else
    {
        const float origPixelValue = read_imagef(oldProj, pix).x;
        write_bufferf(newProj, pix, origPixelValue, oldProj);
    }
}
