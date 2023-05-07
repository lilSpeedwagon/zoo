from datetime import datetime, timedelta

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


class DateTimeMock:
    """
    Mock allows you compare datetime objects (or ISO-formatted strings) with specified accuracy.
    Use it when you want to check whether a datetime object is differs
    from the target value not more than on the specified timedelta.
    """

    def __init__(self, datetime_: datetime, accuracy: timedelta = timedelta(minutes=1)) -> None:
        self.datetime_ = datetime_
        self.accuracy = accuracy

    def __eq__(self, other: object) -> bool:
        if isinstance(other, datetime):
            return self.datetime_ - self.accuracy <= other <= self.datetime_ + self.accuracy
        if isinstance(other, str):
            from dateutil.parser import parse
            try:
                dt = parse(other, ignoretz=True)
            except (TypeError, ValueError):
                return False
            return self.__eq__(dt)
        return False

    def __repr__(self) -> str:
        return f'<DateTimeMock datetime={self.datetime_} accuracy={self.accuracy}>'

    @staticmethod
    def from_now(accuracy: timedelta = timedelta(minutes=1)):
        return DateTimeMock(datetime_=datetime.now(), accuracy=accuracy)

    @staticmethod
    def from_utc_now(accuracy: timedelta = timedelta(minutes=1)):
        return DateTimeMock(datetime_=datetime.utcnow(), accuracy=accuracy)
