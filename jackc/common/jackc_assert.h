#ifndef JACKC_ASSERT_H
#define JACKC_ASSERT_H

void jackc_assert_fail(const char* expr, const char* file, int line);

#define jackc_assert(expr) ((expr) ? (void)0 : jackc_assert_fail(#expr, __FILE__, __LINE__))

#endif
