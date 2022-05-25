import pytest
import requests

@pytest.fixture
def api_config():
    class ApiConfigService:
        def __init__(self):
            self.host = 'localhost'
            self.port = 80

        @property
        def address(self) -> str:
            return f'{self.host}:{self.port}'

        def make_uri(self, path):
            return 'http://' + self.address + ('' if path.startswith('/') else '') + path

        def get(self, path: str) -> requests.Response:
            return requests.get(self.make_uri(path))

        def post(self, path: str, body: dict) -> requests.Response:
            return requests.post(self.make_uri(path), data=body)
        
    return ApiConfigService()
