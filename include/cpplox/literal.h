#include <utility>
#include <expr.h>
#include <token.h>

namespace CppLox {

class Literal : public Expr {
public:
    Literal(LiteralType value) : value(std::move(value)) {}

    LiteralType value;
};

} // namespace CppLox
