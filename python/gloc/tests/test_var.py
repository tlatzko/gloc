# Tests for var are in their own file, because var pollutes global namespace.

# make z1 with call-depth = 1
import gloc_wrapper

def test_wrapper():
    gloc_wrapper.list_test([1, 3, 4])

def test_one():
    assert(True)

