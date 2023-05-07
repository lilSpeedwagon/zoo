from abc import ABC, abstractmethod
from typing import Any, Optional
import requests


DEFAULT_HOST = 'localhost'
DEFAULT_PORT = 80
DEFAULT_TEST_PORT = 5050


class ServiceBase(ABC):
    """
    Base class for service access fixtures. Override host/port in derived class.
    """

    def __init__(self, host = DEFAULT_HOST, port = DEFAULT_PORT, test_port = DEFAULT_TEST_PORT):
        self.host = host
        self.port = port
        self.test_port = test_port
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
        """
        Reset service components system state. If component name is specified - clears the component state.
        """

        body = {}
        if component_name:
            body['component_name'] = component_name
        return requests.post(self.make_uri(self.test_address, '/test-control/reset'), json=body)


class FactoryBase(ABC):
    """
    Base factory class.
    """

    def __init__(self, service: ServiceBase) -> None:
        self._service = service

    @abstractmethod
    def create(self) -> Any:
        pass
