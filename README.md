# CppGLSL 
[**Experiment**] [**Work in progress**]


#### Todo
- [ ] glm vectors, matrices etc (swizzling etc.)
- [ ] glsl functions (min, max, step, sign etc)
- [ ] uniforms
- [ ] textures (samplers)
- [ ] structures (!)
- [ ] for loop
- [ ] native glsl code injection

Code example to show current features:

```cpp
struct MyShaderIn : public GlShaderIn {
    VAR_IN(int, pixel);
    VAR_IN(int, texel);
};

struct MyShaderOut : public GlShaderOut {
    VAR_OUT(int, color);
};

struct MyShader : public GlShader<MyShaderIn, MyShaderOut> {
    GLSL_FUNCTION(int, fib, int, n) {
        If(n < 2).Then([&] {
            Return(n);
        });
        Return(fib(n - 1) + fib(n - 2));
    }

    void main(MyShaderIn in, MyShaderOut &out) {
        VAR(int, i, 0);

        While(i < 16).Do([&] {
            Print(fib(i));
            i = i + 1;
            If(i == 10).Then([&] {
                Break();
            });
        });

        out.color = fib(i);
    }
};
```
`MyShader().GetCode()` will generate:
```
in int pixel;
in int texel;
out int color;

int fib(int n) {
    if ((n<2)) {
        return n;
    }
    return (fib((n-1))+fib((n-2)));
}

void main() {
    int i = 0;
    while ((i<16)) {
        i = (i+1);
        if ((i==10)) {
            break;
        }
    }
    color = fib(i);
}
```
And you can execute the code with `MyShader().Execute(input)`, code from example will generate this output:
```
(int) fib(i): 0
(int) fib(i): 1
(int) fib(i): 1
(int) fib(i): 2
(int) fib(i): 3
(int) fib(i): 5
(int) fib(i): 8
(int) fib(i): 13
(int) fib(i): 21
(int) fib(i): 34
```