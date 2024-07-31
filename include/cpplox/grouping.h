#include <utility>
#include <expr.h>
#include <token.h>

namespace CppLox {

class Grouping : public Expr {
public:
    Grouping(ExprPtr expression) : expression(std::move(expression)) {}

    ExprPtr expression;
};

} // namespace CppLox
