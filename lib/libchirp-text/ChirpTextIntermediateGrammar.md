# Chirp/Chirm Grammar

This document gives a concise overview of the Chirm text format using standard EBNF notation.

## Core grammar

```ebnf
document        = document-item* ;

document-item   = include
                | section
                | user-data
                | comment ;

include         = "@include" , "(" , string , ")" ;

section         = "%" , identifier , "%" , section-item* ;

section-item   = count
                | row
                | property
                | comment ;

count           = "count" , integer ;

row             = "%{" , value-list? , "}%" ;

property        = identifier , "=" , value ;

user-data       = "@" , identifier , "{" , user-data-item* , "}" ;

user-data-item  = user-data
                | property
                | comment ;

value-list      = value , { "," , value } ;

value           = string
                | integer
                | float
                | boolean
                | null
                | identifier
                | tuple ;

tuple           = "(" , value-list? , ")" ;

boolean         = "true"
                | "false" ;

null            = "null" ;

identifier      = identifier-start , identifier-part* ;

identifier-start
                = letter
                | "_" ;

identifier-part = letter
                | digit
                | "_"
                | "."
                | "-"
                | "$"
                | "/" ;

string          = '"' , string-character* , '"' ;

integer         = sign? , digit , digit* ;

float           = sign? , (
                    digit , digit* , "." , digit* , exponent? , float-suffix?
                  | "." , digit , digit* , exponent? , float-suffix?
                  ) ;

exponent        = ("e" | "E") , sign? , digit , digit* ;

float-suffix    = "f" | "F" ;

sign            = "+" | "-" ;

comment         = "#" , character-except-newline* ;

letter          = "A".."Z"
                | "a".."z" ;

digit           = "0".."9" ;
```

## Include paths

The parser treats the include target as a string. The loader applies the include-path policy:

```ebnf
include-path    = absolute-path
                | project-path ;

absolute-path   = "/" , path-component , { "/" , path-component } ;

project-path    = "$/" , path-component , { "/" , path-component } ;

path-component  = identifier-part , identifier-part* ;
```

Examples:

```text
@include("/absolute/path/file.chirm")
@include("$/materials/example_materials.chirm")
```

Ordinary relative paths are not valid under the loader's include-path policy:

```text
@include("materials/file.chirm")
```

## Example

```text
chirp_model

@include("$/materials/example_materials.chirm")

@game
{
    collision_layer = 4
    dynamic = true
    mass = 12.5

    @physics
    {
        enabled = true
    }
}

%vertices%
count 1

%{
    (0.f, 0.f, 0.f, 0.f),
    (0.f, 0.f, 0.f, 0.f),
    (0.f, 0.f),
    (255, 255, 255, 255)
}%

%surfaces%
count 1

%{ 0, SimpleBlend }%
```

## Structural model

```text
document
├── include*
├── section*
│   ├── count?
│   ├── property*
│   └── row*
├── user-data*
│   ├── property*
│   └── user-data*
└── comment*
```

User-data is recursive, so namespaces may be nested to arbitrary depth.

The grammar describes syntax only. Section semantics and the meaning of user-data namespaces are defined by schemas and higher-level adapters.
