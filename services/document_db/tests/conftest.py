import pytest

from utils.test_utils.fixtures import ServiceBase


class DocumentDbService(ServiceBase):
    def __init__(self):
        self.port = 5555
        super().__init__()

    def cleanup(self):
        path = '/api/v1/documents/clear'
        response = self.post(path)
        if response.status_code != 200:
            raise RuntimeError(f'cleanup via {path} returned unexpected return code: {response.status_code}')


@pytest.fixture
def document_db():
    service = DocumentDbService()     
    yield service
    service.cleanup()
