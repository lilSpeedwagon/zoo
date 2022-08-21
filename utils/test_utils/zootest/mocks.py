
class MockAny:
    # Returns True on assertion with any other object.
    # May be used for assertion non-determined data
    # (random values, date, time, etc.).
    def __eq__(self, other):
        return True


class MockAnyString:
    # Returns True on comparison with any other str object.
    def __eq__(self, other):
        return isinstance(other, str)
