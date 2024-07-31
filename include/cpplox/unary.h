#include <utility>
#include <expr.h>
#include <token.h>

namespace CppLox {

class Unary : public Expr {
public:
    Unary(Token op, ExprPtr right) : op(std::move(op)), right(std::move(right)) {}

    Token op;
    ExprPtr right;
};

} // namespace CppLox
