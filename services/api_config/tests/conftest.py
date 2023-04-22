import pytest

from utils.test_utils.fixtures import ServiceBase


@pytest.fixture
def api_config():
    class ApiConfigService(ServiceBase):
        def __init__(self):
            super().__init__()
            self.port = 80

    return ApiConfigService()
