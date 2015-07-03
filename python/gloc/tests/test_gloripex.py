# Tests for var are in their own file, because var pollutes global namespace.

# make z1 with call-depth = 1
from gloc.gloripex import test_function


def test_one():
    test_function()
    assert(True)

