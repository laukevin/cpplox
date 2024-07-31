from typing import List
import argparse

BASE_CLS_TEMPLATE = """
#pragma once
#include <memory>
#include <utility>

#include "token.h"

namespace CppLox {{

{class_declarations}

// Base Visitor interface
class Base{base_class}Visitor
{{
public:
{visitor_methods}
}};

// Template Visitor interface
template <typename R>
class {base_class}Visitor : public Base{base_class}Visitor
{{
public:
{visitor_methods}
}};

// Base Expr class
class {base_class} 
{{
public:
virtual ~{base_class}() = default;

virtual void accept(Base{base_class}Visitor &visitor) const = 0;
}};

using {base_class}Ptr = std::unique_ptr<{base_class}>;

{derived_classes}

}}
"""


DERIVED_CLS_TEMPLATE = """
class {visitor_class} : public {base_class} 
{{
public:
{constructor}

void accept(Base{base_class}Visitor &visitor) const override
{{
  visitor.visit{visitor_class}{base_class}(*this);
}}

{members}
}};

using {visitor_class}Ptr = std::unique_ptr<{visitor_class}>;
"""


def _build_derived_class(base_cls, visitor_cls, constructor_args):
    constructor_args = constructor_args.strip()
    constructor_members = [c.strip() for c in constructor_args.split(",")]
    constructor_pairs = [c.split(" ") for c in constructor_members]

    constructor_members = [f"{c};" for c in constructor_members]
    constructor_members = "\n".join(constructor_members)

    constructor_params = ", ".join(
        [f"{c[1]}(std::move({c[1]}))" for c in constructor_pairs]
    )
    constructor = f"{visitor_cls} ({constructor_args}) : {constructor_params} {{}}"

    return DERIVED_CLS_TEMPLATE.format(
        base_class=base_cls,
        visitor_class=visitor_cls,
        constructor=constructor,
        members=constructor_members,
    )


def _build_visitor_method(base_cls, visitor_cls):
    return f"virtual void visit{visitor_cls}{base_cls}(const {visitor_cls} &expr) = 0;"


def _build_visitor_methods(base_cls, visitor_classes: List[str]):
    return "\n".join(
        _build_visitor_method(base_cls, visitor_cls) for visitor_cls in visitor_classes
    )


def _build_class_declaration(visitor_classes):
    return "\n".join([f"class {visitor_cls};" for visitor_cls in visitor_classes])


def generate_cpp(output_dir, base_class, vistor_class_info):
    visitor_class_names = [info[0] for info in vistor_class_info]
    class_declarations = _build_class_declaration(visitor_class_names)
    derived_classes = [
        _build_derived_class(base_class, info[0], info[1]) for info in vistor_class_info
    ]
    derived_classes = "\n\n".join(derived_classes)

    cpp = BASE_CLS_TEMPLATE.format(
        base_class=base_class,
        visitor_methods=_build_visitor_methods(base_class, visitor_class_names),
        derived_classes=derived_classes,
        class_declarations=class_declarations,
    )
    cpp_file_path = f"{output_dir}/{base_class}.h"
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
                "Grouping : ExprPtr expression",
                "Literal  : LiteralType value",
                "Unary    : Token op, ExprPtr right",
            ],
        }
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
