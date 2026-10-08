inline void* operator new(unsigned int, void* address) { return address; }

struct c_image_probe {
    int value;
    explicit c_image_probe(int initial) : value(initial) {}
    virtual int read() { return value; }
    virtual int add(int amount) { return value + amount; }
};

extern "C" int h2_image_probe_value() {
    return sizeof(c_image_probe) == 8 ? 0x48493221 : -1;
}

/* Force the Microsoft vftable and constructor into the guest image. */
extern "C" void h2_image_probe_construct(c_image_probe* object, int value) {
    new (object) c_image_probe(value);
}
