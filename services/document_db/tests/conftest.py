import pytest
from typing import Any

from utils.test_utils.fixtures import FactoryBase, ServiceBase
from utils.test_utils.mocks import MockAny, DateTimeMock


class DocumentDbService(ServiceBase):
    def __init__(self):
        port = 5555
        super().__init__(port=port)

    def cleanup(self):
        path = '/api/v1/documents/clear'
        response = self.post(path)
        if response.status_code != 200:
            raise RuntimeError(f'cleanup via {path} returned unexpected return code: {response.status_code}')


class DocumentFactory(FactoryBase):
    def __init__(self, service: DocumentDbService) -> None:
        super().__init__(service)

    def create(self, name = 'doc', owner = 'me', namespace = 'local', payload = 'data') -> Any:
        document = {
            'name': name,
            'owner': owner,
            'namespace': namespace,
            'payload': payload,
        }
        response = self._service.post('/api/v1/documents/create', body=document)
        assert(response.status_code == 200)
        document = response.json()
        assert(document == {
            'id': MockAny(),
            'created': MockAny(),
            'updated': MockAny(),
            'name': name,
            'owner': owner,
            'namespace': namespace,
        })
        return document



@pytest.fixture
def document_db():
    service = DocumentDbService()     
    yield service
    service.cleanup()


@pytest.fixture
def document_factory(document_db):
    return DocumentFactory(document_db)

