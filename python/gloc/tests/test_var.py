# Tests for var are in their own file, because var pollutes global namespace.
# make z1 with call-depth = 1
from gloc.greet import greet

def test_greet():
    greet()
    assert(True)

