#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <memory>
#include <set>
#include <cassert>

#define REGISTER_BINARY_OP(name, op) \
    template<typename S, typename T = decltype(S() op S())>                                            \
    class name ## BinaryOpExprNode : public BinaryOpExprNode<S, T> { \
    public:                                    \
        name ## BinaryOpExprNode(ExprNodePtr<S> left, ExprNodePtr<S> right) \
            : BinaryOpExprNode<S, T>(#op, std::move(left), std::move(right)) {}              \
                                                \
        T GetValue() override {                 \
            return this->m_left->GetValue() op this->m_right->GetValue();                                        \
        }                                        \
    };                                    \
                                          \
    template<typename S, typename T = decltype(S() op S())>                                            \
    friend GlValue<T> operator op(const GlValue<S> &lhs, const GlValue<S> &rhs) {\
        return GlValue<T>(lhs.m_context, std::make_shared<name ## BinaryOpExprNode<S, T>>(lhs.m_expr, rhs.m_expr)); \
    }                                     \
                                          \
    template<typename S, typename T = decltype(S() op S())>                                            \
    friend GlValue<T> operator op(const GlValue<S> &lhs, const S &rhs) {\
        return GlValue<T>(lhs.m_context, std::make_shared<name ## BinaryOpExprNode<S, T>>(lhs.m_expr, std::make_shared<ConstExprNode<S>>(rhs))); \
    }


#define REGISTER_ALL_OP() \
    REGISTER_BINARY_OP(Add, +)      \
    REGISTER_BINARY_OP(Sub, -)      \
    REGISTER_BINARY_OP(Mul, *)      \
    REGISTER_BINARY_OP(Mod, %)      \
    REGISTER_BINARY_OP(Div, /)      \
    REGISTER_BINARY_OP(OrBit, |)    \
    REGISTER_BINARY_OP(AndBit, &) \
    REGISTER_BINARY_OP(Eq, ==) \
    REGISTER_BINARY_OP(Leq, <=) \
    REGISTER_BINARY_OP(Grq, >=) \
    REGISTER_BINARY_OP(Le, <) \
    REGISTER_BINARY_OP(Gr, >) \

#define FUNCTION_ARGS_CREF_4(type, name, ...) const GlValue<type> & name
#define FUNCTION_ARGS_CREF_3(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_4(__VA_ARGS__))
#define FUNCTION_ARGS_CREF_2(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_3(__VA_ARGS__))
#define FUNCTION_ARGS_CREF_1(type, name, ...) const GlValue<type> & name __VA_OPT__(, FUNCTION_ARGS_CREF_2(__VA_ARGS__))
#define FUNCTION_ARGS_CREF(...) __VA_OPT__(FUNCTION_ARGS_CREF_1(__VA_ARGS__))

#define FUNCTION_GEN_GL_VALUE_TYPES_4(type, name, ...) GlValue<type>
#define FUNCTION_GEN_GL_VALUE_TYPES_3(type, name, ...) GlValue<type> __VA_OPT__(, FUNCTION_GEN_GL_VALUE_TYPES_4(__VA_ARGS__))
#define FUNCTION_GEN_GL_VALUE_TYPES_2(type, name, ...) GlValue<type> __VA_OPT__(, FUNCTION_GEN_GL_VALUE_TYPES_3(__VA_ARGS__))
#define FUNCTION_GEN_GL_VALUE_TYPES_1(type, name, ...) GlValue<type> __VA_OPT__(, FUNCTION_GEN_GL_VALUE_TYPES_2(__VA_ARGS__))
#define FUNCTION_GEN_GL_VALUE_TYPES(...) __VA_OPT__(FUNCTION_GEN_GL_VALUE_TYPES_1(__VA_ARGS__))

#define FUNCTION_GEN_TYPES_4(type, name, ...) type
#define FUNCTION_GEN_TYPES_3(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_4(__VA_ARGS__))
#define FUNCTION_GEN_TYPES_2(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_3(__VA_ARGS__))
#define FUNCTION_GEN_TYPES_1(type, name, ...) type __VA_OPT__(, FUNCTION_GEN_TYPES_2(__VA_ARGS__))
#define FUNCTION_GEN_TYPES(...) __VA_OPT__(FUNCTION_GEN_TYPES_1(__VA_ARGS__))

#define FUNCTION_GEN_NAMES_4(type, name, ...) name
#define FUNCTION_GEN_NAMES_3(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_4(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_2(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_3(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_1(type, name, ...) name __VA_OPT__(, FUNCTION_GEN_NAMES_2(__VA_ARGS__))
#define FUNCTION_GEN_NAMES(...) __VA_OPT__(FUNCTION_GEN_NAMES_1(__VA_ARGS__))

#define FUNCTION_GEN_NAMES_4_STR(type, name, ...) #name
#define FUNCTION_GEN_NAMES_3_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_4_STR(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_2_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_3_STR(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_1_STR(type, name, ...) #name __VA_OPT__(, FUNCTION_GEN_NAMES_2_STR(__VA_ARGS__))
#define FUNCTION_GEN_NAMES_STR(...) __VA_OPT__(FUNCTION_GEN_NAMES_1_STR(__VA_ARGS__))

#define FUNCTION_GEN_ARG_4(type, name, ...) GlValue<type>(this, std::make_shared<VarExprNode<type>>(#name, name.GetValue()))
#define FUNCTION_GEN_ARG_3(type, name, ...) GlValue<type>(this, std::make_shared<VarExprNode<type>>(#name, name.GetValue())) __VA_OPT__(, FUNCTION_GEN_ARG_4(__VA_ARGS__))
#define FUNCTION_GEN_ARG_2(type, name, ...) GlValue<type>(this, std::make_shared<VarExprNode<type>>(#name, name.GetValue())) __VA_OPT__(, FUNCTION_GEN_ARG_3(__VA_ARGS__))
#define FUNCTION_GEN_ARG_1(type, name, ...) GlValue<type>(this, std::make_shared<VarExprNode<type>>(#name, name.GetValue())) __VA_OPT__(, FUNCTION_GEN_ARG_2(__VA_ARGS__))
#define FUNCTION_GEN_ARG(...) __VA_OPT__(FUNCTION_GEN_ARG_1(__VA_ARGS__))

#define _FUNC_GEN_TYPE(ret_type, func, ...) \
    GlFunctionGenerator<ret_type __VA_OPT__( ,FUNCTION_GEN_TYPES(__VA_ARGS__))>

#define _FUNC_GEN_(ret_type, func, ...) \
    _FUNC_GEN_TYPE(ret_type, func, __VA_ARGS__)(*this, #func __VA_OPT__(, FUNCTION_GEN_NAMES_STR(__VA_ARGS__)))

#define GLSL_FUNCTION(ret_type, func, ...)                                                  \
    GlValue<ret_type> func(FUNCTION_ARGS_CREF(__VA_ARGS__)) {                               \
        auto func_lambda = [&](FUNCTION_ARGS_CREF(__VA_ARGS__)) { _impl_ ## func(FUNCTION_GEN_NAMES(__VA_ARGS__)); }; \
        if (m_mode == Mode::Codegen) {                                                      \
            auto func_gen = _FUNC_GEN_(ret_type, func, __VA_ARGS__);                        \
            if (!IsFuncGenerated(func_gen.m_signature)) {                                   \
                m_func_ready.insert(func_gen.m_signature);                                                                            \
                func_lambda(FUNCTION_GEN_ARG(__VA_ARGS__));                                                                                \
                func_gen.Complete();                                                         \
            }                                                                                   \
        }\
        return GlValue<ret_type>(this, std::make_shared<FunctionCallExprNode<ret_type, FUNCTION_GEN_GL_VALUE_TYPES(__VA_ARGS__)>>(*this, #func, func_lambda, FUNCTION_GEN_NAMES(__VA_ARGS__)));\
    }         \
    void _impl_ ## func(FUNCTION_ARGS_CREF(__VA_ARGS__))

#define VAR(type, name, ...) auto name = Var<type>(#name __VA_OPT__(,__VA_ARGS__))

#define VAR_IN(type, name) \
    GlShaderContext::GlValue<type> name = Var<type>(#name, "in"); \
    void _add_context_ptr ## name() {m_context_pointers.push_back(&name.m_context);} \
    int _tmp_helper_ ## name =  (_add_context_ptr ## name(),0);

#define VAR_OUT(type, name) \
    GlShaderContext::GlValue<type> name = Var<type>(#name, "out"); \
    void _add_context_ptr ## name() {m_context_pointers.push_back(&name.m_context);} \
    int _tmp_helper_ ## name =  (_add_context_ptr ## name(),0);

#define ENABLE_TYPE(type)       \
template<>                      \
std::string TypeStr<type>() {   \
    return #type;               \
}

#define ENABLE_ALL_TYPES() \
    ENABLE_TYPE(int);  \
    ENABLE_TYPE(float);\
    ENABLE_TYPE(bool); \
    ENABLE_TYPE(double); \


template<typename T>
std::string TypeStr();

template<typename... T>
std::vector<std::string> TypesStr() {
    return {TypeStr<T>()...};
}

template<typename T>
std::string ValStr(const T &val) {
    return std::to_string(val);
}

template<>
std::string ValStr<bool>(const bool &val) {
    return val ? "true" : "false";
}

ENABLE_ALL_TYPES();

class GlShaderContext {
protected:
    enum class Mode {
        Execution,
        Codegen
    } m_mode = Mode::Execution;

    template<typename T>
    struct ExprNode {
        explicit ExprNode(std::string str_val)
                : m_str_value(std::move(str_val)) {}

        [[nodiscard]] std::string GetStringValue() const {
            return m_str_value;
        }

        virtual T GetValue() = 0;

    protected:
        std::string m_str_value;
    };

    template<typename T>
    using ExprNodePtr = std::shared_ptr<ExprNode<T>>;

    template<typename T>
    struct ConstExprNode : public ExprNode<T> {
        explicit ConstExprNode(const T &value)
                : ExprNode<T>(ValStr(value)), m_value(value) {}

        T GetValue() override {
            return m_value;
        }

    protected:
        T m_value;
    };

    template<typename T>
    struct VarExprNode : public ExprNode<T> {
        explicit VarExprNode(std::string name, const T &value = T())
                : ExprNode<T>(name), m_value(value) {}

        T GetValue() override {
            return m_value;
        }

        void Set(const T &value) {
            m_value = value;
        }

    protected:
        T m_value;
    };

    template<typename S, typename T>
    struct BinaryOpExprNode : public ExprNode<T> {
    public:
        BinaryOpExprNode(const std::string &op, ExprNodePtr<S> left, ExprNodePtr<S> right)
                : ExprNode<T>("(" + left->GetStringValue() + op + right->GetStringValue() + ")"),
                  m_left(std::move(left)),
                  m_right(std::move(right)) {}

    protected:
        ExprNodePtr<S> m_left;
        ExprNodePtr<S> m_right;
    };

public:
    template<typename T>
    struct GlValue {
    public:
        ExprNodePtr<T> m_expr;
        GlShaderContext *m_context = nullptr;

        explicit GlValue(ExprNodePtr<T> expr) : m_expr(std::move(expr)) {}

        GlValue(GlShaderContext *context, ExprNodePtr<T> expr)
                : m_context(context), m_expr(std::move(expr)) {}

        [[nodiscard]] std::string GetStringValue() const {
            return m_expr->GetStringValue();
        }

        [[nodiscard]] T GetValue() const {
            return m_expr->GetValue();
        }

        GlValue &operator=(GlValue<T> other) {
            if (this == &other)
                return *this;
            assert(std::dynamic_pointer_cast<VarExprNode<T>>(m_expr));
            auto var_expr = std::dynamic_pointer_cast<VarExprNode<T>>(m_expr);
            var_expr->Set(other.GetValue());
            if (m_context && m_context->m_mode == Mode::Codegen) {
                m_context->AddCodeLine(var_expr->GetStringValue() + " = " + other.GetStringValue() + ";");
            }
            return *this;
        }

        GlValue &operator=(const T &other) {
            assert(std::dynamic_pointer_cast<VarExprNode<T>>(m_expr));
            auto var_expr = std::dynamic_pointer_cast<VarExprNode<T>>(m_expr);
            var_expr->Set(other);
            if (m_context && m_context->m_mode == Mode::Codegen) {
                m_context->AddCodeLine(var_expr->GetStringValue() + " = " + ValStr(other) + ";");
            }
            return *this;
        }

    };

protected:
    struct GlBreakValue {
    };

    template<typename T, typename ... Args>
    struct FunctionCallExprNode : public ExprNode<T> {
        FunctionCallExprNode(GlShaderContext &context, const std::string &name, std::function<void(Args...)> func,
                             Args ... args)
                : ExprNode<T>(name), m_context(&context), m_args(std::make_tuple(args...)), m_func(std::move(func)) {

            this->m_str_value += "(";
            AddArgs(args...);
            this->m_str_value += ")";
        }

        T GetValue() override {
            if (m_context->m_mode == Mode::Codegen) {
                return T();
            }
            try {
                std::apply(m_func, m_args);
            } catch (const GlValue<T> &r) {
                return r.GetValue();
            }
            return T();
        }

    protected:
        template<typename S>
        void AddArgs(const GlValue<S> &arg) {
            this->m_str_value += arg.GetStringValue();
        }

        template<typename S, typename ... ArgsS>
        void AddArgs(const GlValue<S> &arg, ArgsS ... args) {
            this->m_str_value += arg.GetStringValue() + ",";
            AddArgs(args...);
        }

        std::function<void(Args...)> m_func;
        std::tuple<Args...> m_args;
        GlShaderContext *m_context;
    };

    REGISTER_ALL_OP();

    struct ElseStatement {
        GlShaderContext &m_context;
        GlValue<bool> m_condition;

        ElseStatement(GlShaderContext &context, const GlValue<bool> &condition)
                : m_context(context), m_condition(condition) {}

        void Else(const std::function<void(void)> &func) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine(" else {", true);
                m_context.IncCodeIndentation();
                func();
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
            } else {
                if (!m_condition.GetValue()) {
                    func();
                }
            }
        }
    };

    struct ThenStatement {
        GlShaderContext &m_context;
        GlValue<bool> m_condition;

        ThenStatement(GlShaderContext &context, const GlValue<bool> &condition)
                : m_context(context), m_condition(condition) {}

        ElseStatement Then(const std::function<void(void)> &func) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine(" {", true);
                m_context.IncCodeIndentation();
                func();
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
            } else {
                if (m_condition.GetValue()) {
                    func();
                }
            }
            return ElseStatement(m_context, m_condition);
        }
    };

    struct LoopStatement {
        GlShaderContext &m_context;
        GlValue<bool> m_condition;

        explicit LoopStatement(GlShaderContext &context, const GlValue<bool> &condition)
                : m_context(context), m_condition(condition) {}

        void Do(const std::function<void(void)> &func) {
            if (m_context.m_mode == Mode::Codegen) {
                m_context.AddCodeLine(" {", true);
                m_context.IncCodeIndentation();
                func();
                m_context.DecCodeIndentation();
                m_context.AddCodeLine("}");
            } else {
                try {
                    while (m_condition.GetValue()) {
                        func();
                    }
                } catch (GlBreakValue &) {

                }
            }
        }
    };

    template<typename T>
    void Print(const GlValue<T> &var) {
        if (m_mode == Mode::Codegen)
            return;
        std::cout << "(" << TypeStr<T>() << ") " << var.GetStringValue() << ": " << ValStr(var.GetValue()) << std::endl;
    }

    LoopStatement While(const GlValue<bool> &condition) {
        if (m_mode == Mode::Codegen) {
            AddCodeLine("while (" + condition.GetStringValue() + ")");
        }
        return LoopStatement(*this, condition);
    }

    ThenStatement If(const GlValue<bool> &condition) {
        if (m_mode == Mode::Codegen) {
            AddCodeLine("if (" + condition.GetStringValue() + ")");
        }
        return ThenStatement(*this, condition);
    }

    void Break() {
        if (m_mode == Mode::Execution) {
            throw GlBreakValue(); // NOLINT(hicpp-exception-baseclass)
        } else {
            AddCodeLine("break;");
        }
    }

    template<typename T>
    void Return(const GlValue<T> &val) {
        if (m_mode == Mode::Execution) {
            throw val; // NOLINT(hicpp-exception-baseclass)
        } else {
            AddCodeLine("return " + val.GetStringValue() + ";");
        }
    }

    template<typename T>
    void Return(const T &val) {
        if (m_mode == Mode::Execution) {
            throw GlValue<T>(this, std::make_shared<ConstExprNode<T>>(val)); // NOLINT(hicpp-exception-baseclass)
        } else {
            AddCodeLine("return " + ValStr(val) + ";");
        }
    }

    bool IsFuncGenerated(const std::string &signature) {
        return m_func_ready.find(signature) != m_func_ready.end();
    }

    template<typename T>
    GlValue<T> Var(const std::string &name, const T &val = T()) {
        if (m_mode == Mode::Codegen) {
            AddCodeLine(TypeStr<T>() + " " + name + " = " + ValStr(val) + ";");
        }
        return GlValue<T>(this, std::make_shared<VarExprNode<T>>(name, val));
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
        GlShaderContext &m_context;
        std::string m_code_buffer_copy;
        std::string m_indentation_copy;
        std::string m_signature;

        template<typename ... T>
        GlFunctionGenerator(GlShaderContext &context, const std::string &name, T ...args_name):m_context(context) {
            std::vector<std::string> names = {args_name...};
            std::vector<std::string> types = TypesStr<ArgTypes...>();
            m_signature = TypeStr<RetType>() + " " + name + "(";
            assert(names.size() == types.size());
            for (size_t i = 0; i < names.size(); i++) {
                m_signature += types[i] + " " + names[i];
                if (i + 1 < names.size())
                    m_signature += ", ";
            }
            m_signature += ")";

            if (!m_context.IsFuncGenerated(m_signature)) {
                m_code_buffer_copy = std::move(m_context.m_code_buffer);
                m_indentation_copy = std::move(m_context.m_indentation);
                m_context.AddCodeLine(m_signature + " {");
                m_context.IncCodeIndentation();
            }
        }

        void Complete() {
            m_context.DecCodeIndentation();
            m_context.AddCodeLine("}\n");
            m_context.m_code_buffer += m_code_buffer_copy;
            m_context.m_indentation = m_indentation_copy;
        }
    };

    std::set<std::string> m_func_ready;
    std::string m_indentation;
    std::string m_code_buffer;

    friend class GlShaderInOut;
};

template<typename In, typename Out>
class GlShader : public GlShaderContext {
public:

    virtual void main(In in, Out &out) {}

    std::string GetCode() {
        m_mode = Mode::Codegen;
        m_code_buffer = "";

        In in;
        Out out;
        in.InitCodeGen(*this);
        out.InitCodeGen(*this);
        std::string inout_code = std::move(m_code_buffer);
        AddCodeLine("void main() {");
        IncCodeIndentation();
        main(in, out);
        DecCodeIndentation();
        AddCodeLine("}");

        m_code_buffer = inout_code + "\n" + m_code_buffer;

        return m_code_buffer;
    }

    Out Execute(In in) {
        m_mode = Mode::Execution;

        Out out;
        main(in, out);

        return out;
    }

    friend class GlShaderInOut;
};

struct GlShaderInOut {
protected:
    template<typename In, typename Out>
    friend
    class GlShader;

    struct VarInOut {
        std::string prefix;
        std::string type;
        std::string name;
    };

    std::vector<VarInOut> m_variables;
    std::vector<GlShaderContext **> m_context_pointers;

    template<typename In, typename Out>
    void InitCodeGen(GlShader<In, Out> &shader) {
        for (auto &ptr : m_context_pointers) {
            *ptr = &shader;
        }
        for (const auto &[prefix, type, name] : m_variables) {
            std::string line = prefix + " " + type + " " + name + ";";
            shader.AddCodeLine(line);
        }
    }

    template<typename T>
    GlShaderContext::GlValue<T> Var(const std::string &name, const std::string &prefix, const T &val = T()) {
        m_variables.emplace_back(VarInOut{prefix, TypeStr<T>(), name});
        return GlShaderContext::GlValue<T>(std::make_shared<GlShaderContext::VarExprNode<T>>(name, val));
    }
};

struct GlShaderIn : public GlShaderInOut {
private:
};

struct GlShaderOut : public GlShaderInOut {
private:
};

struct MyShaderIn : public GlShaderIn {
    VAR_IN(int, pixel);
    VAR_IN(int, texel);
};

struct MyShaderOut : public GlShaderOut {
    VAR_OUT(int, color);
};

class MyShader : public GlShader<MyShaderIn, MyShaderOut> {
public:

    GLSL_FUNCTION(int, fib, int, n) {
        If(n < 2).Then([&] {
            Return(n);
        });
        Return(fib(n - 1) + fib(n - 2));
    }

    void main(MyShaderIn in, MyShaderOut &out) override {
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

int main() {
    MyShader shader;
    std::cout << "=== GLSL CODE ===" << std::endl;
    std::cout << shader.GetCode() << std::endl;

    shader.Execute(MyShaderIn());
    return 0;
}
