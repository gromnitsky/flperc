CFLAGS := -std=c17 -Wall -Wextra -Wpedantic $(if $(debug),-g,-O2)
flperc:
