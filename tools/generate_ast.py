from typing import List
import argparse

FILE_TEMPLATE = """
#pragma once
#include <memory>
#include <utility>
#include <any>

#include "token.h"

using namespace std;

namespace CppLox {{

{forward_declarations}

{visitor_cls_declarations}

{derived_cls_declarations}
}}
"""

VISITOR_CLS_TEMPLATE = """

class Base{base_cls}Visitor
{{
public:
virtual ~Base{base_cls}Visitor() = default;
}};

template <typename R>
class {base_cls}Visitor : public Base{base_cls}Visitor
{{
public:
    {virtual_methods}
}};

class {base_cls}
{{
public:
    virtual ~{base_cls}() = default;
    virtual std::any accept(Base{base_cls}Visitor &visitor) const = 0;
}};

using {base_cls}Ptr = std::unique_ptr<{base_cls}>;
"""

DERIVED_CLS_TEMPLATE = """
struct {visitor_cls} : public {base_cls}
{{

{constructor}

std::any accept(Base{base_cls}Visitor &visitor) const override
{{
  auto visitor_ptr = dynamic_cast<{base_cls}Visitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visit{visitor_cls}{base_cls}(this));
  return std::any();
}}

{members}

}};

using {visitor_cls}Ptr = std::unique_ptr<{visitor_cls}>;
"""


def _build_forward_declarations(visitor_class_names: List[str]) -> str:
    return "\n".join(
        [f"class {visitor_class};" for visitor_class in visitor_class_names]
    )


def _build_visitor_methods(base_class: str, visitor_class_names: List[str]) -> str:
    return "\n".join(
        [
            f"    virtual R visit{visitor_class}{base_class}(const {visitor_class} *{base_class.lower()}) = 0;"
            for visitor_class in visitor_class_names
        ]
    )


def _build_member_list(member_list: List[str]) -> str:
    return "\n".join([f"    {member};" for member in member_list])


def _build_constructor(visitor_class, member_list: List[str]) -> str:
    member_names = [member.split()[1].strip() for member in member_list]
    return f"{visitor_class}({', '.join(member_list)}) : {', '.join([f'{name}(std::move({name}))' for name in member_names])} {{}}"


def _build_derived_class(base_class, visitor_class, member_list):
    return DERIVED_CLS_TEMPLATE.format(
        base_cls=base_class,
        visitor_cls=visitor_class,
        constructor=_build_constructor(visitor_class, member_list),
        members=_build_member_list(member_list),
    )


def generate_cpp(output_dir, base_class, vistor_class_info):
    visitor_class_names = [info[0] for info in vistor_class_info]
    forward_declartions = _build_forward_declarations(visitor_class_names)
    visitor_cls_declarations = VISITOR_CLS_TEMPLATE.format(
        base_cls=base_class,
        virtual_methods=_build_visitor_methods(base_class, visitor_class_names),
    )
    derived_classes = "\n".join(
        [
            _build_derived_class(base_class, info[0], info[1].split(","))
            for info in vistor_class_info
        ]
    )

    cpp = FILE_TEMPLATE.format(
        forward_declarations=forward_declartions,
        visitor_cls_declarations=visitor_cls_declarations,
        derived_cls_declarations=derived_classes,
    )

    cpp_file_path = f"{output_dir}/{base_class.lower()}.h"
    with open(cpp_file_path, "w") as f:
        f.write(cpp)


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Generate AST classes for a given grammar"
    )

    parser.add_argument(
        "-o", "--output", type=str, help="The directory to output the AST classes"
    )

    return parser.parse_args()


def generate_files(output_dir):
    ast_list = [
        {
            "base_class": "Expr",
            "visitor_classes": [
                "Binary   : ExprPtr left, Token op, ExprPtr right",
                "Call     : ExprPtr callee, Token paren, vector<ExprPtr> arguments",
                "Get      : ExprPtr object, Token name",
                "Set      : ExprPtr object, Token name, ExprPtr value",
                "Super    : Token keyword, Token method",
                "Grouping : ExprPtr expression",
                "Literal  : LiteralType value",
                "This     : Token keyword",
                "Unary    : Token op, ExprPtr right",
                "Variable : Token name",
                "Assign   : Token name, ExprPtr value",
                "Logical  : ExprPtr left, Token op, ExprPtr right",
            ],
        },
        {
            "base_class": "Stmt",
            "visitor_classes": [
                "Block      : vector<StmtPtr> statements",
                "Class      : Token name, ExprPtr superclass, vector<StmtPtr> methods",
                "Expression : ExprPtr expression",
                "Print      : ExprPtr expression",
                "Return     : Token keyword, ExprPtr value",
                "Var        : Token name, ExprPtr initializer",
                "Function   : Token name, vector<Token> params, vector<StmtPtr> body",
                "If         : ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch",
                "While      : ExprPtr condition, StmtPtr body",
            ],
        },
    ]

    for ast in ast_list:
        visitor_class_info = [info.split(":") for info in ast["visitor_classes"]]
        visitor_class_info = [
            (info[0].strip(), info[1].strip()) for info in visitor_class_info
        ]
        generate_cpp(output_dir, ast["base_class"], visitor_class_info)


if "__main__" == __name__:
    parser = parse_arguments()
    generate_files(parser.output)
