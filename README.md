```cpp
class MyShader : public GlShader {
public:

    GLSL_FUNCTION(int, func, int, x, int, c) {
        GLSL_RETURN(x * x + c);
    }

    GLSL_FUNCTION(int, func_2, int, x, int, y) {
        GLSL_RETURN(func(x + y, x - y));
    }

    void main() override {
        auto x = Make<int>("x", 5);
        auto y = Make<int>("y", 1);
        auto c = Make<bool>("c", false);
        x = x + x;
        If(x < y).Then([&] {
            x = x + y;
            y = x * y;
        }).Else([&] {
            y = func_2(y + x, x * y);
            y = func_2(x, x + y);
        });
    }
};
```
`MyShader().GetCode()` will generate:
```
int func(int x, int c) {
    return ((x * x) + c);
}

int func_2(int x, int y) {
    return (func(x + y, x - y));
}

void main() {
    int x = 5;
    int y = 1;
    bool c = 0;
    x = (x + x);
    if ((x < y)){
        x = (x + y);
        y = (x * y);
    } else {
        y = (func_2(y + x, x * y));
        y = (func_2(x, x + y));
    }
}
```