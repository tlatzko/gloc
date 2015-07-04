# Tests for var are in their own file, because var pollutes global namespace.

# make z1 with call-depth = 1
from gloc.gloripex import test_function, World


def test_one():
    test_function()
    assert(True)


def test_class():
    w = World("hello")
    s = w.greet()
    assert(s == "hello")
