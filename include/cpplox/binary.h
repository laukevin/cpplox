#include <utility>
#include <expr.h>
#include <token.h>

namespace CppLox {

class Binary : public Expr {
public:
    Binary(ExprPtr left, Token op, ExprPtr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    ExprPtr left;
    Token op;
    ExprPtr right;
};

} // namespace CppLox
