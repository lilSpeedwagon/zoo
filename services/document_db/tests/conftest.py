import pytest

from zootest.fixtures import ServiceBase

@pytest.fixture
def document_db():
    class DocumentDbService(ServiceBase):
        def __init__(self):
            self.port = 5555
            super().__init__()
            
    return DocumentDbService()
