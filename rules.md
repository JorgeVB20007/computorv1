# Rules

- Spaces are not taken into consideration, except for two consecutive numbers (`2 2` = Error, `22` = 22)
- The addition and substraction signs (`+` and `-`) are only considered to be a minus if:
	- It has another sign before (`2 + - 2x` is `2 + (-2) * x`, but `2 - 2x` is `2 - 2 * x`).
	- It's located right after an opening parenthesis (`2 - (-3)`).
	- Located at the beginning of the expression (`-3x ...`)
- When two or more items (numbers, variables or expressions) are together with no operation between them, a multiplication is assumed but doesn't guarantee prevalence over higher-level operations. For example, `8 / 2x` is interpreted as `(8 / 2) * x` and `3x^2` means `3 * (x^2)`.
- The exponent sign (`^`) can only be followed by a number. These are **NOT** valid: `x^x`, `x^(2)`, `x^2x`.
- Consecutive exponents (`2^3^4`) are stacked (`2^3^4 = 2^81`).
- Divisions of expressions inside parentheses such as `(x + 1) / (x - 1)` are not valid unless both expressions are the same, as they're not a continuous function anymore.
- Some operations will exit the program if a variable's exponent `< 0` or `> 2`.
