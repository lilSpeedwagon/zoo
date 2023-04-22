import pytest

from utils.test_utils.fixtures import ServiceBase

@pytest.fixture
def dummy_service():
    class DummyService(ServiceBase):
        def __init__(self):
            super().__init__()
            self.port = 1111

    return DummyService()
