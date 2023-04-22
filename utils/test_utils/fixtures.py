from typing import Optional
import requests


class ServiceBase:
    """
    Base class for service access fixtures. Override host/port in derived class.
    """

    def __init__(self):
        self.host = 'localhost'
        self.port = 5555
        self.test_port = 5050
        self.reset()

    @property
    def address(self) -> str:
        return f'{self.host}:{self.port}'

    @property
    def test_address(self) -> str:
        return f'{self.host}:{self.test_port}'

    def make_uri(self, address: str, path: str):
        return 'http://' + address + ('' if path.startswith('/') else '') + path

    def get(self, path: str) -> requests.Response:
        return requests.get(self.make_uri(self.address, path))

    def post(self, path: str, body: dict = {}) -> requests.Response:
        return requests.post(self.make_uri(self.address, path), json=body)

    def reset(self, component_name: Optional[str] = None):
        """"""

        body = {}
        if component_name:
            body['component_name'] = component_name
        return requests.post(self.make_uri(self.test_address, '/test-control/reset'), json=body)
