#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <memory>
#include <set>
#include <cassert>

#define REGISTER_BINARY_OPERATOR(op) \
GlValue<T> operator op(const GlValue<T> &other) const {\
    if (m_context.m_mode == Mode::Codegen) {                                                \
        std::string my_str = PopStringValue();                                              \
        m_context.m_stack.push_back(my_str + " " + #op + " " + other.PopStringValue());     \
        return GlValue<T>(m_context);                                                  \
    }                                                                                       \
    return GlValue<T>(m_context);                                                      \
}

#define REGISTER_BINARY_BOOL_OPERATOR(op)                                                   \
GlValue<bool> operator op(const GlValue<T> &other) const {                        \
    if (m_context.m_mode == Mode::Codegen) {                                                \
        std::string my_str = PopStringValue();                                              \
        m_context.m_stack.push_back(my_str + " " + #op + " " + other.PopStringValue());     \
        return GlValue<bool>(m_context);                                               \
    }                                                                                       \
    return GlValue<bool>(m_context);                                                   \
}

#define PP_SEQ_N() 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define NUM_ARGS_(...) PP_ARG_N(__VA_ARGS__)
#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__,PP_SEQ_N())

#define FUNCTION_ARGS_CREF_4(type, name, ...) const GlValue<type> & name
#define FUNCTION_ARGS_CREF_3(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_4(__VA_ARGS__))
#define FUNCTION_ARGS_CREF_2(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_3(__VA_ARGS__))
#define FUNCTION_ARGS_CREF_1(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_2(__VA_ARGS__))

#define FUNCTION_GEN_TYPES_4(type, name, ...) type
#define FUNCTION_GEN_TYPES_3(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_4(__VA_ARGS__))
#define FUNCTION_GEN_TYPES_2(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_3(__VA_ARGS__))
#define FUNCTION_GEN_TYPES_1(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_2(__VA_ARGS__))

#define FUNCTION_GEN_NAMES_4(type, name, ...) name
#define FUNCTION_GEN_NAMES_3(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_4(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_2(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_3(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_1(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_2(__VA_ARGS__))

#define FUNCTION_GEN_NAMES_4_STR(type, name, ...) #name
#define FUNCTION_GEN_NAMES_3_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_4_STR(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_2_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_3_STR(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_1_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_2_STR(__VA_ARGS__))

#define FUNCTION_GEN_ARG_4(type, name, ...) GlValue<type>(*this, #name, name.m_value)
#define FUNCTION_GEN_ARG_3(type, name, ...) GlValue<type>(*this, #name, name.m_value) __VA_OPT__(, FUNCTION_GEN_ARG_4(__VA_ARGS__))
#define FUNCTION_GEN_ARG_2(type, name, ...) GlValue<type>(*this, #name, name.m_value) __VA_OPT__(, FUNCTION_GEN_ARG_3(__VA_ARGS__))
#define FUNCTION_GEN_ARG_1(type, name, ...) GlValue<type>(*this, #name, name.m_value) __VA_OPT__(, FUNCTION_GEN_ARG_2(__VA_ARGS__))

#define _FUNC_GEN_TYPE(ret_type, func, ...) \
    GlFunctionGenerator<ret_type, FUNCTION_GEN_TYPES_1(__VA_ARGS__)>

#define _FUNC_GEN_(ret_type, func, ...) \
    _FUNC_GEN_TYPE(ret_type, func, __VA_ARGS__)(*m_active_context, #func, FUNCTION_GEN_NAMES_1_STR(__VA_ARGS__))

#define GLSL_FUNCTION(ret_type, func, ...)                                                  \
    GlValue<ret_type> func(FUNCTION_ARGS_CREF_1(__VA_ARGS__)) {                                  \
        PutArgsToStack(FUNCTION_GEN_NAMES_1(__VA_ARGS__));\
        return _gl_func_ ## func(FUNCTION_GEN_ARG_1(__VA_ARGS__), _FUNC_GEN_(ret_type, func, __VA_ARGS__), NUM_ARGS(__VA_ARGS__)/2);                                                        \
    }                                                                                       \
    GlValue<ret_type> _gl_func_ ## func(FUNCTION_ARGS_CREF_1(__VA_ARGS__),  \
        _FUNC_GEN_TYPE(ret_type, func, __VA_ARGS__) _func_gen, \
        int _num_args, ret_type _ret_type = ret_type())

#define GLSL_RETURN(ret)        \
    _func_gen.Complete(_num_args, (_func_gen.m_ready ? GlValue<decltype(_ret_type)>(*this) : ret));       \
    return GlValue<decltype(_ret_type)>(*this)


#define ENABLE_TYPE(type)       \
template<>                      \
std::string TypeStr<type>() {   \
    return #type;               \
}

template<typename T>
std::string TypeStr();

ENABLE_TYPE(int);

ENABLE_TYPE(float);

ENABLE_TYPE(bool);

template<typename... T>
std::vector<std::string> TypesStr() {
    return {TypeStr<T>()...};
}

class GlShader {
public:
    static GlShader *m_active_context; // todo: remove this somehow?

    virtual void main() {}

    std::string GetCode() {
        m_mode = Mode::Codegen;
        m_code_buffer = "";
        m_active_context = this;

        main();

        return m_code_buffer;
    }

protected:
    enum class Mode {
        Execution,
        Codegen
    } m_mode = Mode::Execution;

    std::string StackPop() {
        assert(!m_stack.empty());
        auto val = std::move(m_stack.back());
        m_stack.pop_back();
        return val;
    }

    template<typename T>
    struct GlValue {
    public:
        std::string m_name;
        bool m_isOnStack = true;
        bool m_needParenthesis = false;
        GlShader &m_context;
        T m_value = T();

        explicit GlValue(GlShader &context, bool needParenthesis = true)
                : m_context(context), m_isOnStack(true), m_needParenthesis(needParenthesis) {}

        GlValue(GlShader &context, std::string name, T value)
                : m_context(context), m_name(std::move(name)), m_isOnStack(false), m_value(std::move(value)) {}

        [[nodiscard]] std::string PopStringValue() const {
            std::string r = m_isOnStack ? m_context.StackPop() : m_name;
            return m_needParenthesis ? "(" + r + ")" : r;
        }

        GlValue &operator=(const GlValue<T> &other) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine(m_name + " = " + other.PopStringValue() + ";");
            } else {
                // todo:
            }
            return *this;
        }

        REGISTER_BINARY_OPERATOR(+);

        REGISTER_BINARY_OPERATOR(-);

        REGISTER_BINARY_OPERATOR(*);

        REGISTER_BINARY_OPERATOR(%);

        REGISTER_BINARY_OPERATOR(/);

        REGISTER_BINARY_OPERATOR(|);

        REGISTER_BINARY_OPERATOR(&);

        REGISTER_BINARY_BOOL_OPERATOR(==);

        REGISTER_BINARY_BOOL_OPERATOR(<);

        REGISTER_BINARY_BOOL_OPERATOR(>);

        REGISTER_BINARY_BOOL_OPERATOR(<=);

        REGISTER_BINARY_BOOL_OPERATOR(>=);

        REGISTER_BINARY_BOOL_OPERATOR(!=);
    };

    struct ElseStatement {
        GlShader &m_context;

        explicit ElseStatement(GlShader &context) : m_context(context) {}

        void Else(const std::function<void(void)> &func) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine(" else {", true);
                m_context.IncCodeIndentation();
                func();
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
            }
        }
    };

    struct ThenStatement {
        GlShader &m_context;

        explicit ThenStatement(GlShader &context) : m_context(context) {}

        ElseStatement Then(const std::function<void(void)> &func) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine("{", true);
                m_context.IncCodeIndentation();
                func();
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
            }
            return ElseStatement(m_context);
        }
    };

    ThenStatement If(const GlValue<bool> &condition) {
        if (m_mode == Mode::Codegen) {
            AddCodeLine("if (" + condition.PopStringValue() + ")");
        }
        // todo:
        return ThenStatement(*this);
    }

    template<typename T>
    GlValue<T> Make(const std::string &name, const T &val = T()) {
        if (m_mode == Mode::Codegen) {
            AddCodeLine(TypeStr<T>() + " " + name + " = " + std::to_string(val) + ";");
        }
        return GlValue<T>(*this, name, val);
    }

    void AddCodeLine(const std::string &line, bool append_to_previous = false) {
        if (append_to_previous) {
            if (!m_code_buffer.empty() && m_code_buffer.back() == '\n')
                m_code_buffer.pop_back();
            m_code_buffer += line + "\n";
            return;
        }
        m_code_buffer += m_indentation + line + "\n";
    }

    void IncCodeIndentation() {
        m_indentation += "    ";
    }

    void DecCodeIndentation() {
        if (m_indentation.size() >= 4)
            m_indentation.resize(m_indentation.size() - 4);
    }

    template<typename RetType, typename ... ArgTypes>
    struct GlFunctionGenerator {
        GlShader &m_context;
        bool m_ready = false;
        std::string m_code_buffer_copy;
        std::string m_indentation_copy;
        std::string m_name;

        template<typename ... T>
        GlFunctionGenerator(GlShader &context, const std::string &name, T ...args):m_context(context), m_name(name) {
            std::vector<std::string> names = {args...};
            std::vector<std::string> types = TypesStr<ArgTypes...>();
            std::string signature = TypeStr<RetType>() + " " + name + "(";
            assert(names.size() == types.size());
            for (size_t i = 0; i < names.size(); i++) {
                signature += types[i] + " " + names[i];
                if (i + 1 < names.size())
                    signature += ", ";
            }
            signature += ")";

            if (context.m_func_ready.find(signature) == context.m_func_ready.end()) {
                context.m_func_ready.insert(signature);
                m_code_buffer_copy = std::move(m_context.m_code_buffer);
                m_indentation_copy = std::move(m_context.m_indentation);
                m_context.AddCodeLine(signature + " {");
                m_context.IncCodeIndentation();
            } else {
                m_ready = true;
            }
        }

        template<typename T>
        void Complete(int num_args, const GlValue<T> & ret) {
            if (!m_ready) {
                m_context.AddCodeLine("return " + ret.PopStringValue() + ";");
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
                m_context.m_code_buffer += m_code_buffer_copy;
                m_context.m_indentation = m_indentation_copy;
            }
            std::string call = m_name + "(";
            for (int i = 0; i < num_args; i++) {
                call += m_context.StackPop();
                if (i + 1 < num_args)
                    call += ", ";
            }
            call += ")";
            m_context.m_stack.emplace_back(call);
        }
    };

    void PutArgsToStack() {}

    template<typename T, typename ... Args>
    void PutArgsToStack(const GlValue<T> & arg, Args ... args) {
        std::string s = arg.m_name;
        if (arg.m_isOnStack) {
            s = StackPop();
        }
        PutArgsToStack(args...);
        m_stack.push_back(s);
    }

    std::set<std::string> m_func_ready;
    std::string m_indentation;
    std::string m_code_buffer;
    std::vector<std::string> m_stack; // used while generating code
};

GlShader *GlShader::m_active_context = nullptr;


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

int main() {
    MyShader shader;
    std::cout << "=== GLSL CODE ===" << std::endl;
    std::cout << shader.GetCode() << std::endl;
    std::string a;
    return 0;
}
