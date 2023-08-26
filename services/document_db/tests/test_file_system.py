import os

from services.document_db.tests.conftest import DocumentDbService, DocumentFactory


DB_DIRECTORY = os.getcwd() + '/../../build/services/document_db/'


def test_reload_from_fs_fetch(document_db: DocumentDbService, document_factory: DocumentFactory):
    """
    Check fetching files after reloading file system components.
    """

    # create some documents
    payloads = [f'data{i}' for i in range(3)]
    docs = [document_factory.create(name=f'doc{i}', payload=payloads[i]) for i in range(3)]

    # update some documents
    docs[0]['name'] = 'new name'
    payloads[0] = 'new payload'
    update_data = {
        'id': docs[0]['id'],
        'name': docs[0]['name'],
        'payload': payloads[0],
    }
    update_response = document_db.post('/api/v1/documents/update', body=update_data)
    assert update_response.status_code == 200
    
    # reset and make sure that documents were reloaded from FS
    reset_response = document_db.reset()
    assert reset_response.status_code == 200

    response = document_db.get(f'/api/v1/documents/list')
    assert response.status_code == 200
    assert response.json() == {'items': docs}

    for doc, payload in zip(docs, payloads):
        response = document_db.get(f'/api/v1/documents/get?id={doc["id"]}')
        assert response.status_code == 200
        assert response.json() == doc | {'payload': payload}


def test_reload_from_fs_index(document_db: DocumentDbService, document_factory: DocumentFactory):
    """
    Check document index restoring after reloading file system component.
    """

    # create some documents
    docs = [document_factory.create(name=f'doc{i}') for i in range(3)]
    
    # reset and make sure that document index is restored
    reset_response = document_db.reset()
    assert reset_response.status_code == 200

    doc = document_factory.create()
    assert doc['id'] == docs[-1]['id'] + 1
