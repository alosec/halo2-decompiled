// @flags /O2 /GL-
// Havok (third-party physics library): an accessor that returns a small
// descriptor by value (hidden return pointer).

struct descriptor_2fd210
{
    void *data;
    bool valid;
    long stride;

    descriptor_2fd210(void *data_, long stride_)
    {
        data = data_;
        stride = stride_;
        valid = true;
    }
};

struct object_2fd210
{
    char unknown0[0xd0];
    void *data;

    descriptor_2fd210 get_descriptor() const;
};

// @retail 0x2fd210
descriptor_2fd210 object_2fd210::get_descriptor() const
{
    return descriptor_2fd210(data, 0x10);
}
