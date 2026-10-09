/* Native menu rendering does not use the Xbox cache-prediction system yet.
 * The recovered callers use this as a best-effort preload hint, so fail
 * conservatively without fabricating loaded texture, geometry, or audio state.
 * Actual resource ownership remains at the explicit host rendering boundary. */
struct s_predicted_resource_block;

bool function_16e5e0(s_predicted_resource_block const *block, short mode)
{
    (void)block;
    (void)mode;
    return false;
}
