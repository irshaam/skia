/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_DSL_CORE
#define SKSL_DSL_CORE

#include "include/private/SkTArray.h"
#include "src/sksl/dsl/DSLBlock.h"
#include "src/sksl/dsl/DSLCase.h"
#include "src/sksl/dsl/DSLExpression.h"
#include "src/sksl/dsl/DSLFunction.h"
#include "src/sksl/dsl/DSLStatement.h"
#include "src/sksl/dsl/DSLType.h"
#include "src/sksl/dsl/DSLVar.h"
#include "src/sksl/dsl/priv/DSLWriter.h"

namespace SkSL {

class Compiler;

namespace dsl {

/**
 * Class which is notified in the event of an error.
 */
class ErrorHandler {
public:
    virtual ~ErrorHandler() {}

    virtual void handleError(const char* msg) = 0;
};

/**
 * Starts DSL output on the current thread using the specified compiler. This must be called
 * prior to any other DSL functions.
 */
void Start(SkSL::Compiler* compiler);

/**
 * Signals the end of DSL output. This must be called sometime between a call to Start() and the
 * termination of the thread.
 */
void End();

/**
 * Installs an ErrorHandler which will be notified of any errors that occur during DSL calls. If
 * no ErrorHandler is installed, any errors will be fatal.
 */
void SetErrorHandler(ErrorHandler* errorHandler);

DSLVar sk_FragColor();

DSLVar sk_FragCoord();

/**
 * break;
 */
DSLStatement Break();

/**
 * continue;
 */
DSLStatement Continue();

/**
 * Creates a variable declaration statement with an initial value.
 */
DSLStatement Declare(DSLVar& var, DSLExpression initialValue = DSLExpression());

/**
 * default: statements
 */
template<class... Statements>
DSLCase Default(Statements... statements) {
    return DSLCase(DSLExpression(), std::move(statements)...);
}

/**
 * discard;
 */
DSLStatement Discard();

/**
 * do stmt; while (test);
 */
DSLStatement Do(DSLStatement stmt, DSLExpression test);

/**
 * for (initializer; test; next) stmt;
 */
DSLStatement For(DSLStatement initializer, DSLExpression test, DSLExpression next,
                 DSLStatement stmt);

/**
 * if (test) ifTrue; [else ifFalse;]
 */
DSLStatement If(DSLExpression test, DSLStatement ifTrue, DSLStatement ifFalse = DSLStatement());

/**
 * return [value];
 */
DSLStatement Return(DSLExpression value = DSLExpression());

/**
 * switch (value) { cases }
 */
template<class... Cases>
DSLStatement Switch(DSLExpression value, Cases... cases) {
    SkSL::ExpressionArray caseValues;
    SkTArray<StatementArray> caseStatements;
    caseValues.reserve_back(sizeof...(cases));
    caseStatements.reserve_back(sizeof...(cases));
    int unused[] = {0, (DSLWriter::Ignore(caseValues.push_back(cases.fValue.release())), 0)...};
    static_cast<void>(unused);
    (caseStatements.push_back(std::move(cases.fStatements)), ...);
    return DSLWriter::ConvertSwitch(value.release(),
                                    std::move(caseValues),
                                    std::move(caseStatements));
}

/**
 * test ? ifTrue : ifFalse
 */
DSLExpression Ternary(DSLExpression test, DSLExpression ifTrue, DSLExpression ifFalse);

/**
 * while (test) stmt;
 */
DSLStatement While(DSLExpression test, DSLStatement stmt);


enum SwizzleComponent : int8_t {
    X = 0, Y = 1, Z = 2, W = 3,
    R = 0, G = 1, B = 2, A = 3,
    ZERO,
    ONE
};

DSLExpression Swizzle(DSLExpression base, SwizzleComponent a);

DSLExpression Swizzle(DSLExpression base, SwizzleComponent a, SwizzleComponent b);

DSLExpression Swizzle(DSLExpression base, SwizzleComponent a, SwizzleComponent b,
                      SwizzleComponent c);

DSLExpression Swizzle(DSLExpression base, SwizzleComponent a, SwizzleComponent b,
                      SwizzleComponent c, SwizzleComponent d);

/**
 * Returns the absolute value of x. If x is a vector, operates componentwise.
 */
DSLExpression Abs(DSLExpression x);

/**
 * Returns true if all of the components of boolean vector x are true.
 */
DSLExpression All(DSLExpression x);

/**
 * Returns true if any of the components of boolean vector x are true.
 */
DSLExpression Any(DSLExpression x);

/**
 * Returns x rounded towards positive infinity. If x is a vector, operates componentwise.
 */
DSLExpression Ceil(DSLExpression x);

/**
 * Returns x clamped to between min and max. If x is a vector, operates componentwise.
 */
DSLExpression Clamp(DSLExpression x, DSLExpression min, DSLExpression max);

/**
 * Returns the cosine of x. If x is a vector, operates componentwise.
 */
DSLExpression Cos(DSLExpression x);

/**
 * Returns the cross product of x and y.
 */
DSLExpression Cross(DSLExpression x, DSLExpression y);

/**
 * Returns x converted from radians to degrees. If x is a vector, operates componentwise.
 */
DSLExpression Degrees(DSLExpression x);

/**
 * Returns the distance between x and y.
 */
DSLExpression Distance(DSLExpression x, DSLExpression y);

/**
 * Returns the dot product of x and y.
 */
DSLExpression Dot(DSLExpression x, DSLExpression y);

/**
 * Returns a boolean vector indicating whether components of x are equal to the corresponding
 * components of y.
 */
DSLExpression Equal(DSLExpression x, DSLExpression y);

/**
 * Returns e^x. If x is a vector, operates componentwise.
 */
DSLExpression Exp(DSLExpression x);

/**
 * Returns 2^x. If x is a vector, operates componentwise.
 */
DSLExpression Exp2(DSLExpression x);

/**
 * If dot(i, nref) >= 0, returns n, otherwise returns -n.
 */
DSLExpression Faceforward(DSLExpression n, DSLExpression i, DSLExpression nref);

/**
 * Returns x rounded towards negative infinity. If x is a vector, operates componentwise.
 */
DSLExpression Floor(DSLExpression x);

/**
 * Returns the fractional part of x. If x is a vector, operates componentwise.
 */
DSLExpression Fract(DSLExpression x);

/**
 * Returns a boolean vector indicating whether components of x are greater than the corresponding
 * components of y.
 */
DSLExpression GreaterThan(DSLExpression x, DSLExpression y);

/**
 * Returns a boolean vector indicating whether components of x are greater than or equal to the
 * corresponding components of y.
 */
DSLExpression GreaterThanEqual(DSLExpression x, DSLExpression y);

/**
 * Returns the 1/sqrt(x). If x is a vector, operates componentwise.
 */
DSLExpression Inversesqrt(DSLExpression x);

/**
 * Returns the inverse of the matrix x.
 */
DSLExpression Inverse(DSLExpression x);

/**
 * Returns the length of the vector x.
 */
DSLExpression Length(DSLExpression x);

/**
 * Returns a boolean vector indicating whether components of x are less than the corresponding
 * components of y.
 */
DSLExpression LessThan(DSLExpression x, DSLExpression y);

/**
 * Returns a boolean vector indicating whether components of x are less than or equal to the
 * corresponding components of y.
 */
DSLExpression LessThanEqual(DSLExpression x, DSLExpression y);

/**
 * Returns the log base e of x. If x is a vector, operates componentwise.
 */
DSLExpression Log(DSLExpression x);

/**
 * Returns the log base 2 of x. If x is a vector, operates componentwise.
 */
DSLExpression Log2(DSLExpression x);

/**
 * Returns the larger (closer to positive infinity) of x and y. If x is a vector, operates
 * componentwise. y may be either a vector of the same dimensions as x, or a scalar.
 */
DSLExpression Max(DSLExpression x, DSLExpression y);

/**
 * Returns the smaller (closer to negative infinity) of x and y. If x is a vector, operates
 * componentwise. y may be either a vector of the same dimensions as x, or a scalar.
 */
DSLExpression Min(DSLExpression x, DSLExpression y);

/**
 * Returns a linear intepolation between x and y at position a, where a=0 results in x and a=1
 * results in y. If x and y are vectors, operates componentwise. a may be either a vector of the
 * same dimensions as x and y, or a scalar.
 */
DSLExpression Mix(DSLExpression x, DSLExpression y, DSLExpression a);

/**
 * Returns x modulo y. If x is a vector, operates componentwise. y may be either a vector of the
 * same dimensions as x, or a scalar.
 */
DSLExpression Mod(DSLExpression x, DSLExpression y);

/**
 * Returns the vector x normalized to a length of 1.
 */
DSLExpression Normalize(DSLExpression x);

/**
 * Returns a boolean vector indicating whether components of x are not equal to the corresponding
 * components of y.
 */
DSLExpression NotEqual(DSLExpression x, DSLExpression y);

/**
 * Returns x raised to the power y. If x is a vector, operates componentwise. y may be either a
 * vector of the same dimensions as x, or a scalar.
 */
DSLExpression Pow(DSLExpression x, DSLExpression y);

/**
 * Returns x converted from degrees to radians. If x is a vector, operates componentwise.
 */
DSLExpression Radians(DSLExpression x);

/**
 * Returns i reflected from a surface with normal n.
 */
DSLExpression Reflect(DSLExpression i, DSLExpression n);

/**
 * Returns i refracted across a surface with normal n and ratio of indices of refraction eta.
 */
DSLExpression Refract(DSLExpression i, DSLExpression n, DSLExpression eta);

/**
 * Returns x clamped to the range [0, 1]. If x is a vector, operates componentwise.
 */
DSLExpression Saturate(DSLExpression x);

/**
 * Returns -1, 0, or 1 depending on whether x is negative, zero, or positive, respectively. If x is
 * a vector, operates componentwise.
 */
DSLExpression Sign(DSLExpression x);

/**
 * Returns the sine of x. If x is a vector, operates componentwise.
 */
DSLExpression Sin(DSLExpression x);

/**
 * Returns a smooth interpolation between 0 (at x=edge1) and 1 (at x=edge2). If x is a vector,
 * operates componentwise. edge1 and edge2 may either be both vectors of the same dimensions as x or
 * scalars.
 */
DSLExpression Smoothstep(DSLExpression edge1, DSLExpression edge2, DSLExpression x);

/**
 * Returns the square root of x. If x is a vector, operates componentwise.
 */
DSLExpression Sqrt(DSLExpression x);

/**
 * Returns 0 if x < edge or 1 if x >= edge. If x is a vector, operates componentwise. edge may be
 * either a vector of the same dimensions as x, or a scalar.
 */
DSLExpression Step(DSLExpression edge, DSLExpression x);

/**
 * Returns the tangent of x. If x is a vector, operates componentwise.
 */
DSLExpression Tan(DSLExpression x);

/**
 * Returns x converted from premultipled to unpremultiplied alpha.
 */
DSLExpression Unpremul(DSLExpression x);

} // namespace dsl

} // namespace SkSL

#endif
