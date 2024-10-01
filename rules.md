# Rules

- Spaces are not taken into consideration, except for two consecutive numbers (`2 2` = 4, `22` = 22)
- The addition and substraction signs (`+` and `-`) are only considered to be a minus if:
	- It has another sign before (`2 + - 2x` is `2 + (-2) * x`, but `2 - 2x` is `2 - 2 * x`).
	- It's located right after an opening parenthesis (`2 - (-3)`).
	- Located at the beginning of the expression (`-3x ...`)
- When two or more items (numbers, variables or expressions) are together with no operation between them, a multiplication is assumed but doesn't guarantee prevalence over higher-level operations. For example, `8 / 2x` is interpreted as `(8 / 2) * x` and `3x^2` means `3 * (x^2)`.
- The exponent sign (`^`) can only be followed by a number. These are **NOT** valid: `x^x`, `x^(2)`, `x^2x`.
- Consecutive exponents (`2^3^4`) are stacked (`2^3^4 = 2^81`).
- **`[TO BE CONSIDERED]`** If at any point an exponent greater than 2 or lower than 0 **on a variable** is reached, the program will return an error (`x^2 * x / x` will fail while `x^2 * (x / x)` and `x^2 / x * x` won't).
**PROTECT THE MALLOCS!!!**