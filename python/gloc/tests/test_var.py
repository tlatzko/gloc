# Tests for var are in their own file, because var pollutes global namespace.

# make z1 with call-depth = 1
import greet


def test_one():
    greet.greet()
    assert(True)

